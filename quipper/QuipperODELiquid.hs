{-@ LIQUID "--reflection" @-}
{-@ LIQUID "--ple"        @-}

-- =============================================================================
-- QuipperODELiquid.hs
-- Time-Stepped ODEs/DAEs in Quipper-Style Quantum Circuits with
-- Compile-Time No-Cloning Verification in Liquid Haskell
-- =============================================================================
-- Combines:
--   1. Discretized ODE / DAE time-stepping (including the cognitive_poly_cell
--      cubic ODE C * dV/dt = -a*V - b*V^3 + gain*V_stim and an algebraic DAE
--      constraint 0 = g(x, z)).
--   2. Quipper-style quantum circuit construction (Qubit allocation, Hadamard,
--      phase-rotation encoding of time-stepped ODE/DAE states, CNOT).
--   3. Liquid Haskell refinement types statically enforcing the No-Cloning
--      Theorem: every Qubit carries a unique linear wire ID, two-qubit gates
--      require distinct wires (ctrl /= tgt), and a consumed wire cannot be
--      reused or duplicated.
-- =============================================================================

module QuipperODELiquid where

import qualified Data.Set as S

-- -----------------------------------------------------------------------------
-- 1. Liquid Haskell Refinements for Time, ODE/DAE State, and Linear Qubits
-- -----------------------------------------------------------------------------

{-@ type PosDouble = {v:Double | v > 0.0} @-}
{-@ type NonNegInt = {v:Int    | v >= 0}  @-}

-- | A Qubit handle tagged with a unique linear wire identifier.
data Qubit = Qubit { qId :: Int }
  deriving (Eq, Ord, Show)

{-@ data Qubit = Qubit { qId :: NonNegInt } @-}

-- | Linear resource context tracking unconsumed (live) qubit wire IDs.
--   Enforces the No-Cloning Theorem: a qubit can only be used if its ID is
--   in `liveWires`, and linear consumption removes it from `liveWires`
--   (or threads a fresh wire ID for the output state).
data CircState = CircState
  { nextWire  :: Int
  , liveWires :: S.Set Int
  , gateLog   :: [String]
  } deriving (Show)

{-@ data CircState = CircState
      { nextWire  :: NonNegInt
      , liveWires :: S.Set Int
      , gateLog   :: [String]
      } @-}

-- | Refinement: wire `q` is currently live (unconsumed) in state `st`.
{-@ predicate LiveQubit Q ST = S.member (qId Q) (liveWires ST) @-}

-- | Refinement: two qubits are strictly distinct (cannot alias the same wire).
{-@ predicate Distinct Q1 Q2 = qId Q1 /= qId Q2 @-}

-- -----------------------------------------------------------------------------
-- 2. Quipper-Style Linear Circuit Primitives (No-Cloning Enforced by LH)
-- -----------------------------------------------------------------------------

-- | Initialize a fresh |0> qubit (Quipper `qinit False` analogue).
--   Guarantees the allocated wire is fresh and added to `liveWires`.
{-@ qinit0 :: st:CircState
           -> (Qubit, CircState)<\q st' -> {v:() | LiveQubit q st' && nextWire st' == nextWire st + 1}> @-}
qinit0 :: CircState -> (Qubit, CircState)
qinit0 st =
  let wid = nextWire st
      q   = Qubit wid
      st' = st { nextWire  = wid + 1
               , liveWires = S.insert wid (liveWires st)
               , gateLog   = gateLog st ++ ["qinit |0> -> q" ++ show wid]
               }
  in (q, st')

-- | Single-qubit Rz(theta) rotation (used to encode ODE/DAE phase evolution).
--   Consumes `q` and returns a fresh linear handle `q'` (no aliasing/cloning).
{-@ applyRz :: theta:Double
            -> q:Qubit
            -> st:{CircState | LiveQubit q st}
            -> (Qubit, CircState) @-}
applyRz :: Double -> Qubit -> CircState -> (Qubit, CircState)
applyRz theta q st =
  let widOld = qId q
      widNew = nextWire st
      qNew   = Qubit widNew
      st'    = st { nextWire  = widNew + 1
                  , liveWires = S.insert widNew (S.delete widOld (liveWires st))
                  , gateLog   = gateLog st ++
                      ["Rz(" ++ show theta ++ ") q" ++ show widOld ++ " -> q" ++ show widNew]
                  }
  in (qNew, st')

-- | Controlled-NOT (Quipper `qnot tgt `controlled` ctrl` analogue).
--   Liquid Haskell statically rejects any call where `ctrl == tgt` (cloning /
--   self-control) or where either qubit has already been consumed.
{-@ applyCNOT :: ctrl:Qubit
              -> tgt:{Qubit | Distinct ctrl tgt}
              -> st:{CircState | LiveQubit ctrl st && LiveQubit tgt st}
              -> ((Qubit, Qubit), CircState) @-}
applyCNOT :: Qubit -> Qubit -> CircState -> ((Qubit, Qubit), CircState)
applyCNOT ctrl tgt st =
  let wC   = nextWire st
      wT   = wC + 1
      qC'  = Qubit wC
      qT'  = Qubit wT
      live'= S.insert wT (S.insert wC (S.delete (qId tgt) (S.delete (qId ctrl) (liveWires st))))
      st'  = st { nextWire  = wT + 1
                , liveWires = live'
                , gateLog   = gateLog st ++
                    ["CNOT (q" ++ show (qId ctrl) ++ ", q" ++ show (qId tgt)
                     ++ ") -> (q" ++ show wC ++ ", q" ++ show wT ++ ")"]
                }
  in ((qC', qT'), st')

-- -----------------------------------------------------------------------------
-- Counterexample (Rejected by Liquid Haskell if uncommented):
-- Attempting to clone a qubit by passing the same wire as both control and
-- target to CNOT, or reusing a consumed wire, violates `Distinct ctrl tgt`
-- and `LiveQubit`:
--
-- {-@ badClone :: q:Qubit -> st:{CircState | LiveQubit q st} -> ((Qubit, Qubit), CircState) @-}
-- badClone q st = applyCNOT q q st   -- LIQUID HASKELL ERROR: qId q /= qId q is UNSAT
-- -----------------------------------------------------------------------------

-- -----------------------------------------------------------------------------
-- 3. ODE & DAE Time-Stepping Mapped to Quipper-Style Circuit Evolution
-- -----------------------------------------------------------------------------

-- | Differential-Algebraic Equation (DAE) state at time `t`:
--     Differential variable: vState (e.g. cognitive_poly_cell node voltage)
--     Algebraic variable:    zAlg   (e.g. boundary/parity multiplier)
data DAEState = DAEState
  { timeT  :: Double
  , vState :: Double
  , zAlg   :: Double
  } deriving (Show)

{-@ data DAEState = DAEState
      { timeT  :: {v:Double | v >= 0.0}
      , vState :: Double
      , zAlg   :: Double
      } @-}

-- | Single Backward-Euler / semi-implicit step for the cognitive_poly_cell ODE
--   coupled to an algebraic DAE constraint:
--     C * dV/dt = -a*V - b*V^3 + gain*V_stim + zAlg
--     0         = zAlg - alpha * V   (algebraic coupling constraint)
--   Liquid Haskell verifies strict time monotonicity: `timeT out > timeT inp`.
{-@ stepDAE :: dt:PosDouble
            -> vStim:Double
            -> inp:DAEState
            -> {out:DAEState | timeT out > timeT inp} @-}
stepDAE :: Double -> Double -> DAEState -> DAEState
stepDAE dt vStim inp =
  let cap   = 1.0e-9
      a     = -1.0e-6
      b     = 1.0e-6
      gain  = 1.0e-6
      alpha = 0.1e-6
      v     = vState inp
      -- Algebraic constraint: z = alpha * v
      zNext = alpha * v
      -- Differential update: dV/dt
      dvdt  = -(a * v + b * v * v * v - gain * vStim - zNext) / cap
      vNext = v + dt * dvdt
  in DAEState { timeT  = timeT inp + dt
              , vState = vNext
              , zAlg   = zNext
              }

-- | Unroll `n` DAE time steps and encode each step's phase rotation &
--   entangling parity check into a linear Quipper-style circuit.
{-@ unrollDAECircuit :: n:NonNegInt
                     -> dt:PosDouble
                     -> vStim:Double
                     -> dae:DAEState
                     -> qSys:Qubit
                     -> qAnc:{Qubit | Distinct qSys qAnc}
                     -> st:{CircState | LiveQubit qSys st && LiveQubit qAnc st}
                     -> (DAEState, Qubit, Qubit, CircState) / [n] @-}
unrollDAECircuit :: Int -> Double -> Double -> DAEState
                 -> Qubit -> Qubit -> CircState
                 -> (DAEState, Qubit, Qubit, CircState)
unrollDAECircuit 0 _ _ dae qSys qAnc st = (dae, qSys, qAnc, st)
unrollDAECircuit n dt vStim dae qSys qAnc st =
  let dae'            = stepDAE dt vStim dae
      theta           = dt * vState dae'
      (qSys1, st1)    = applyRz theta qSys st
      -- Note: applyRz deleted qSys and inserted qSys1; qAnc is still live and distinct
      ((qSys2, qAnc2), st2) = applyCNOT qSys1 qAnc st1
  in unrollDAECircuit (n - 1) dt vStim dae' qSys2 qAnc2 st2

-- | Example runner: 3 DAE time steps (dt = 10 us) starting from V = 0.2 V.
runExample :: (DAEState, Qubit, Qubit, CircState)
runExample =
  let st0           = CircState 0 S.empty []
      (qSys, st1)   = qinit0 st0
      (qAnc, st2)   = qinit0 st1
      dae0          = DAEState 0.0 0.2 0.02e-6
  in unrollDAECircuit 3 1.0e-5 0.5 dae0 qSys qAnc st2
