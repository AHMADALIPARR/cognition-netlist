# README Claim Index

This index maps the principal claims in the README to their source files and lines, and it records whether each claim is verified by the source, derived from the source, observed, or unresolved. It is the reconciliation record for the monograph.

## How to read this index

- **Verified:** The claim is stated in or directly follows from a source file, cited by file and line.
- **Derived:** The claim is a calculation or derivation from source values, and the derivation is shown in the README.
- **Observed:** The claim is a number read from a data file, with no derivation in the source.
- **Author-tested:** The claim comes from the author's statement, not from a file in the repository. The author is the authority for testing claims.
- **Unresolved:** The claim cannot be established from the repository. The README states it as unknown or as a gap.

## Verified claims

### Identity and architecture

| Claim | Source | Status |
|---|---|---|
| 1,024 neurons, degree 8, 4,096 edges | generate.c:8-10, verify.c:10-12, edges.txt (4,096 lines) | Verified |
| AGPL v3.0 license | LICENSE | Verified |
| 36 tracked files, 854,342 bytes | git ls-files, HEAD 3737689 | Verified |
| C11 core is the connected core; satellites share no data | generate.c, verify.c, associative.c, and the absence of file I/O in the satellite files | Verified |
| Makefile targets and flags | Makefile:1-17 | Verified |
| `capacity` not in .PHONY; `clean` does not remove associative, capacity.csv, neurons_1024.cir | Makefile:4, Makefile:12-13 | Verified |

### Mathematics

| Claim | Source | Status |
|---|---|---|
| C dv/dt = -aV - bV^3 + gain V_stim, cap=1e-9, a=-1e-6, b=1e-6, gain=1e-6 | verilog-a/cognitive_poly_cell.va:16-26 | Verified |
| Nominal form dv/dt = 1000(v - v^3 + s) | cognitive_poly.jl:14 | Verified |
| Potential U(v,s) = 0.5av^2 + 0.25bv^4 - gain s v | cognitive_poly.jl:19-21, cognitive_poly.R:13-15 | Verified |
| Equilibria -1, 0, +1; Jacobians -2000, +1000, -2000 | cognitive_poly.jl:33, cognitive_poly.R:25 | Verified |
| s_crit = 2/(3 sqrt 3) = 0.38490018 | cognitive_poly.jl:34, cognitive_poly.R:26, verilog-a/tb_cognitive_poly_cell.va:41 | Verified |
| Network ODE gain = 0.2(1-r), diffusive coupling GC=0.04, 25-ohm resistors | verify.c:47-55, generate.c:42-43, netlist lines 4101-4102 | Verified |
| Reference REF=0.2, GREF=0.1, 10-ohm resistors | verify.c:60, netlist lines 12294-12296 | Verified |
| Lyapunov energy E, gradient flow, -0.05 per neuron at r=1 | verify.c:62-74 | Verified |
| GF(2^4) P(x)=x^4+x+1, reduction rules, node out=(in0^in1^in2)*coeff | verilog/gf_topological_dag.v:25-47 | Verified |
| Topology C_8(1,4), neighbors (i-1, i+1, i+4) mod 8 | verilog/gf_topological_dag.v:74-78, 90-94 | Verified |
| Hopfield field, no diagonal, energy E=-(sum overlap^2 - p*live)/(2N) | associative.c:42, associative.c:65-68 | Verified |

### C11 core and netlist

| Claim | Source | Status |
|---|---|---|
| generate.c validates (range, self-loop, duplicate, count, degree) and emits the netlist | generate.c:19-28, generate.c:34-51 | Verified |
| verify.c integrates RK4, DT=0.05, 2,400 steps, t=120 | verify.c:13-14, verify.c:76-85 | Verified |
| verify.c initial conditions (seed, x in [0.62,0.82], y in [-0.08,0.08], cue x1=1, y2=0.4, x3=-0.5) | verify.c:88-93 | Verified |
| verify.c mask_init removes N*pct/100, never neuron 0 | verify.c:95 | Verified |
| associative.c mask can remove neuron 0 | associative.c:59 | Verified |
| Netlist layout (header 1-4, neurons 5-4100, edges 4101-12292, footer 12293-12301) | neurons_1024.cir | Verified (structural) |
| Component counts (2,048 capacitors, 2,048 sources, 8,194 resistors) | neurons_1024.cir (counted) | Verified |
| Netlist .ic, .tran 0.1 40, .save neurons 0-3 | netlist lines 12298-12300 | Verified |

### Julia and R

| Claim | Source | Status |
|---|---|---|
| Julia stdlib only; R base only | cognitive_poly.jl:2-6, cognitive_poly.R:2 | Verified |
| Julia and R constants, rhs, potential, RK4 identical | cognitive_poly.jl:8-29, cognitive_poly.R:4-23 | Verified |
| potential is dead code in both (never called) | cognitive_poly.jl:19-21, cognitive_poly.R:13-15 (no call sites) | Verified |
| Julia MersenneTwister(42); R set.seed(42); different samples | cognitive_poly.jl:32, cognitive_poly.R:28 | Verified |
| Hysteresis parameters identical (h=1e-5, 2,000 steps, 25 values) | cognitive_poly.jl:36-47, cognitive_poly.R:29-35 | Verified |
| Ridge parameters identical (n=600, lambda=1e-4, same basis and target) | cognitive_poly.jl:49-64, cognitive_poly.R:37-51 | Verified |
| Julia is re-parameterizable (keyword args); R uses globals | cognitive_poly.jl:15,19,23; cognitive_poly.R:9,13,17 | Verified |
| No 450/150 split; no normalized RMSE; raw train RMSE on all 600 rows | cognitive_poly.jl:50-64, cognitive_poly.R:37-51 | Verified (contradicts sub-README) |
| No assertions in either file | cognitive_poly.jl, cognitive_poly.R (no @assert, no stopifnot) | Verified |
| Sub-README 450/150 split and normalized RMSE claims are false | cognitive_poly_julia_r/README.md:27 vs the code | Verified (discrepancy) |

### Q#, Quipper, Smalltalk

| Claim | Source | Status |
|---|---|---|
| Q# voltage-to-angle mapping (+1V to 0, 0V to pi/2, -1V to pi) | qsharp/AnalogWaveInterference.qs:18-21 | Verified |
| Q# interferometer H, Rz, H, MResetZ; P=cos^2(phi/2) | qsharp/AnalogWaveInterference.qs:33-53 | Verified |
| Q# parity is XOR only at basis points; (0,0) is probabilistic | qsharp/AnalogWaveInterference.qs:58-76 (derived) | Derived |
| Q# Main uses hard-coded voltages, no file I/O | qsharp/AnalogWaveInterference.qs:79-99 | Verified |
| QuipperODELiquid.hs imports only Data.Set, does not import Quipper, has no Hadamard | quipper/QuipperODELiquid.hs:23 | Verified |
| QuipperODELiquid.hs code is forward Euler, comment says Backward-Euler | quipper/QuipperODELiquid.hs:147, 158-168 | Verified (discrepancy) |
| QuipperODELiquid.hs constants (cap, a, b, gain, alpha) | quipper/QuipperODELiquid.hs:158-162 | Verified |
| Quipper StateMachine.hs nextState is partial (11 of 24 pairs undefined) | quipper/StateMachine.hs:44-55 | Verified |
| Smalltalk FSM matches spec, implements deferred buffer, handles undefined pairs by doing nothing | smalltalk/StateMachine.st:25-66, spec/transitions.md:29-43 | Verified |
| Spec transition table (7 transitions, ESCALATE terminal) | spec/transitions.md:29-43 | Verified |
| Spec grounding cues and deferred buffer (FIFO, drain on SYNC) | spec/transitions.md:59-90 | Verified |
| YACC production and Somatic Anchor are spec-only, no implementation | spec/transitions.md:92-104 (no code file) | Verified (absence) |

### Verilog and Verilog-A

| Claim | Source | Status |
|---|---|---|
| Verilog gf16_mul reduction and node equation | verilog/gf_topological_dag.v:25-47 | Verified |
| Verilog DAG ports, C_8(1,4), fault mask, syndrome, entropy | verilog/gf_topological_dag.v:50-137 | Verified |
| Verilog testbench checks only the syndrome, two scenarios | verilog/gf_topological_dag.v:144-190 | Verified |
| Verilog-A cell parameters and KCL | verilog-a/cognitive_poly_cell.va:10-26 | Verified |
| Verilog-A gf2_xor_analog defined but never instantiated | verilog-a/gf_topological_dag.va:26 (sole occurrence) | Verified |
| Verilog-A gf16_mac_node soft bits, tanh, laplace_nd tau=1ns | verilog-a/gf_topological_dag.va:47-99 | Verified |
| Verilog-A syndrome is LSB parity for unit coefficients only; header describes general c1*c2 parity | verilog-a/gf_topological_dag.va:158-160, 184, 204 | Verified (discrepancy) |
| Verilog-A testbench has no automated verdict, no stop time, no $finish; only a |V|>5 strobe | verilog-a/tb_cognitive_poly_cell.va:72-74 | Verified |
| Verilog-A testbench stimulus (hysteresis sweep, v_crit=0.384900179) | verilog-a/tb_cognitive_poly_cell.va:41, 51-58 | Verified |
| Unlambda witnesses print T and F (minimal pair) | retrieval_true.unl, retrieval_false.unl (10 bytes each) | Verified |

### Data and results

| Claim | Source | Status |
|---|---|---|
| results.csv: all 5 scenarios PASS, exact energies, gradient RMS, max amplitudes | results.csv:2-6 | Verified |
| capacity.csv: 105 rows, 1,680 trials, perfect recall to 32 patterns, breakdown at 64 | capacity.csv (all 105 rows) | Verified |
| clean_state.csv: 1,015 of 1,024 have x>=0.999; 1,023 have y<-0.0029; neuron 0 at (0.891758539329, -0.00251311502948) | clean_state.csv (direct count) | Verified |
| edges.txt: 4,096 lines, sorted, 8-regular, 0 vertices with degree != 8 | edges.txt (verified) | Verified |
| Graph stays in 1 component in all scenarios | results.csv, verify.c:104 | Verified |

## Derived claims

| Claim | Derivation | Status |
|---|---|---|
| Spectral bound: max nontrivial eigenvalue 5.26332069, second-largest 5.25756776, bound 2 sqrt(7)=5.29150262 | Computed from edges.txt by dense eigendecomposition (numpy eigvalsh) for this README. The eigenvalue numbers do not appear in any source file. | Derived (reproducible) |
| Energy scaling: -0.05 per alive neuron (clean -51.2, 973 -48.65, 922 -46.1, 820 -41.0) | From verify.c:66, 71, 73: per-neuron local term -0.1r + 0.05r^2 = 0.05((r-1)^2 - 1), minimized at r=1 with value -0.05. | Derived |
| Fixed points of the Verilog-A cell at V=0 and V=±1 | From verilog-a/cognitive_poly_cell.va:16-19: -a/b = 1, sqrt = 1. The file does not list them. | Derived |
| Q# (0,0) parity is probabilistic (~0.5) | From qsharp/AnalogWaveInterference.qs:18-21, 58-76: 0V encodes to |+>, CNOT entangles, measurement is probabilistic. | Derived |
| s_crit derivation (fold condition for a cubic) | Standard fold condition; not shown in any source file. Labeled as a derivation in the README. | Derived |

## Observed claims (no derivation in the source)

| Claim | Source | Status |
|---|---|---|
| Clean-state y values cluster near -0.003 rather than 0 | clean_state.csv | Observed (unexplained by any source file) |
| Noisy max amplitude 1.049698292 exceeds 1 during forcing | results.csv:3 | Observed |
| Hopfield mean sweeps 2.000 for nearly all low-load rows | capacity.csv | Observed |

## Author-tested claims

| Claim | Basis | Status |
|---|---|---|
| SPICE transient was run in ngspice; the netlist was verified by actual computation | Author's statement. The manifest line 17 ("NOT TESTED") is stale and should be corrected. No SPICE output file is in the repository. | Author-tested |
| Liquid Haskell verification was performed, including the badClone rejection | Author's statement. No Liquid Haskell output or certificate is in the repository. | Author-tested |
| The author hand-typed the majority of the Smalltalk FSM code with SnapKitty | Author's statement. | Author-tested |

## Unresolved claims

| Claim | Why it is unresolved | Status |
|---|---|---|
| Whether the Q# program was ever built or run | No execution output or log in the repository. | Unresolved |
| Whether the Liquid Haskell refinements were checked (beyond the author's statement) | No Liquid Haskell output or certificate in the repository. | Unresolved (author-tested) |
| Whether the Quipper FSM was ever built or run | No build log or circuit output in the repository. | Unresolved |
| Whether the Smalltalk file was ever filed into an image and executed | The usage example is a comment, not a transcript. | Unresolved |
| Whether the Verilog or Verilog-A testbenches were ever executed, and with what outcome | No simulation output in the repository. The PASS and strobe are code, not recorded results. | Unresolved |
| The reason the clean-state y values cluster near -0.003 | No source file explains it. | Unresolved |
| Cross-implementation agreement of the ODE constants with the C11 model | No C file is referenced by the Julia/R files, and the relation cannot be determined from those files alone. | Unresolved |
| The implementation status of the YACC production and the Somatic Anchor | No grammar, lexer, or Anchor code is in the repository. | Unresolved |
| The physical meaning and units of the grounding cues as real-system inputs | No code performs the cue-to-event mapping; no sensor or timing implementation is present. | Unresolved |
| How the Hopfield retrieval result reaches the Unlambda witnesses | No driver or invocation is in the .unl files. | Unresolved |
| Whether the current README's claims match the sources (for the prior README) | The prior README was reconciled against the sources in this audit; the new README replaces it. | Resolved by this audit |

## Discrepancies recorded (not hidden)

| Discrepancy | Location | README chapter |
|---|---|---|
| verify.c parity_check is vacuous (1u^1u is always 0) | verify.c:39-40 | 21, 25 |
| Julia/R sub-README 450/150 split and normalized RMSE are false | cognitive_poly_julia_r/README.md:27 vs the code | 10, 21, 25 |
| Sub-README claims "assertions" but there are none | cognitive_poly_julia_r/README.md:12 vs the code | 10, 25 |
| Manifest README.md entry is stale (6,663 bytes / 60 lines vs 63,398 / 4,543) | INTEGRITY_MANIFEST.txt:3 | 20, 25 |
| Manifest line 17 "NOT TESTED" is stale; the author ran the transient | INTEGRITY_MANIFEST.txt:17 | 3, 20, 21, 25 |
| Quipper ODE comment says Backward-Euler; the code is forward Euler | quipper/QuipperODELiquid.hs:147 vs 158-168 | 12, 25 |
| Quipper FSM nextState is partial (11 of 24 pairs undefined) | quipper/StateMachine.hs:44-55 | 12, 25 |
| Verilog-A DAG syndrome is LSB-only for unit coefficients; the header describes general c1*c2 parity | verilog-a/gf_topological_dag.va:158-160, 184, 204 | 14, 25 |
| gf2_xor_analog is defined but never instantiated | verilog-a/gf_topological_dag.va:26 | 14, 25 |
| C reference and SPICE netlist are not the same initial-value problem | verify.c:90-93 vs netlist line 12298 | 5, 25 |
| verify.c mask_init never removes neuron 0; associative.c mask can | verify.c:95 vs associative.c:59 | 5, 25 |
| Netlist cap is 1 (normalized); Verilog-A/Julia/R cap is 1e-9 | generate.c:34 vs verilog-a/cognitive_poly_cell.va:16 | 17, 25 |
| Makefile gaps (capacity not in .PHONY; clean leaves files behind) | Makefile:4, Makefile:12-13 | 19, 25 |
