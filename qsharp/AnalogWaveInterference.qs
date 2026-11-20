// =============================================================================
// AnalogWaveInterference.qs
// Q# Wave & Quantum Interference Bridge: Connecting Analog ODE Voltage States
// (cognitive_poly_cell) to Qubits via Phase/Amplitude Encoding & Interferometry
// =============================================================================

namespace AnalogWaveInterference {
    open Microsoft.Quantum.Intrinsic;
    open Microsoft.Quantum.Measurement;
    open Microsoft.Quantum.Math;
    open Microsoft.Quantum.Convert;

    /// Maps a normalized analog voltage vState in [-1.0, +1.0] V to a Bloch
    /// polar angle theta in [0, pi]:
    ///   +1.0 V -> theta = 0    (|0> basin)
    ///    0.0 V -> theta = pi/2 (|+> saddle superposition)
    ///   -1.0 V -> theta = pi   (|1> basin)
    function AnalogVoltageToTheta(vState : Double) : Double {
        let clamped = vState > 1.0 ? 1.0 | (vState < -1.0 ? -1.0 | vState);
        return 0.5 * PI() * (1.0 - clamped);
    }

    /// Encodes an analog voltage state onto a single qubit using Ry(theta).
    operation EncodeAnalogState(vState : Double, q : Qubit) : Unit is Adj + Ctl {
        let theta = AnalogVoltageToTheta(vState);
        Ry(theta, q);
    }

    /// Single-qubit Mach-Zehnder / Ramsey wave interferometer driven by an
    /// analog phase accumulation `phi` (e.g., phi = integral of V(t) dt).
    /// Measurement probabilities follow:
    ///   P(|0>) = cos^2(phi / 2),  P(|1>) = sin^2(phi / 2)
    operation RunWaveInterferometer(phi : Double, shots : Int) : (Int, Int) {
        mutable countZero = 0;
        mutable countOne  = 0;

        for _ in 1..shots {
            use q = Qubit();
            // 1. Split wave into equal superposition (50/50 beam splitter)
            H(q);
            // 2. Imprint analog-driven relative phase between |0> and |1> paths
            Rz(phi, q);
            // 3. Recombine paths to produce constructive/destructive interference
            H(q);

            if MResetZ(q) == Zero {
                set countZero += 1;
            } else {
                set countOne += 1;
            }
        }
        return (countZero, countOne);
    }

    /// Two-node analog-to-qubit interference & GF(2) parity check:
    /// Encodes two analog cell voltages (vA, vB), entangles via CNOT, and
    /// measures the F_2 parity (vA XOR vB) in the interference basis.
    operation AnalogParityInterference(vA : Double, vB : Double, shots : Int) : Double {
        mutable oddParityCount = 0;

        for _ in 1..shots {
            use (qA, qB) = (Qubit(), Qubit());
            EncodeAnalogState(vA, qA);
            EncodeAnalogState(vB, qB);

            // Entangle to compute F_2 parity onto qB
            CNOT(qA, qB);

            let resB = MResetZ(qB);
            Reset(qA);
            if resB == One {
                set oddParityCount += 1;
            }
        }
        return IntAsDouble(oddParityCount) / IntAsDouble(shots);
    }

    @EntryPoint()
    operation Main() : Unit {
        let shots = 200;

        // 1. Sweep analog-driven phase phi in {0, pi/2, pi} to show interference
        let (z0, o0)     = RunWaveInterferometer(0.0, shots);
        let (zHalf, oHalf) = RunWaveInterferometer(PI() / 2.0, shots);
        let (zPi, oPi)   = RunWaveInterferometer(PI(), shots);

        Message($"Interference phi=0:    |0>={z0}, |1>={o0} (constructive |0>)");
        Message($"Interference phi=pi/2: |0>={zHalf}, |1>={oHalf} (50/50 quadrature)");
        Message($"Interference phi=pi:   |0>={zPi}, |1>={oPi} (destructive |0> -> |1>)");

        // 2. Connect two cognitive_poly_cell attractor voltages (+1 V, -1 V)
        let pOddOpposite = AnalogParityInterference(1.0, -1.0, shots);
        let pOddSame     = AnalogParityInterference(1.0,  1.0, shots);
        let pOddSaddle   = AnalogParityInterference(0.0,  0.0, shots);

        Message($"F_2 parity P(odd) for (+1V, -1V): {pOddOpposite}");
        Message($"F_2 parity P(odd) for (+1V, +1V): {pOddSame}");
        Message($"F_2 parity P(odd) for ( 0V,  0V): {pOddSaddle}");
    }
}
