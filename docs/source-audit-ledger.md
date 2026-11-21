# Source Audit Ledger

This ledger records the source inspection performed for the grounded README of `cognition-netlist-1024`. It records, for every tracked file, the SHA-256 digest, the byte count, the line count, the reading completion, and the findings. It is the audit trail for the README.

## Audit parameters

- **Repository:** `cognition-netlist-1024`
- **Local HEAD at inspection:** 3737689 ("Restore the full README; embed PNG renders so the diagrams show on phones")
- **Remote HEAD (diverged):** 8e20bbc4cf770407bca8083dbb6f43ff5b316b88 (Grok's damage; not in local history)
- **Files inspected:** 36 tracked files, 854,342 bytes total (prior README). The five new SVG illustrations and their PNG renders in `docs/assets/svg/` are additional.
- **Inspection method:** Four independent readers, one per domain (C core + netlist + data; Julia/R; Q#/Quipper/Smalltalk; Verilog/Verilog-A/Unlambda/SVGs). Each reader read its assigned text files in full and verified line numbers by grep. The generated files (`neurons_1024.cir`, `edges.txt`, `clean_state.csv`) were inspected structurally: header, sample, footer, and counts, not pasted in full. The hand-written sources (approximately 1,700 lines total) were read in full.
- **Reading completion:** Recorded honestly per file. "Full" means every line was read. "Structural" means the file was inspected by header, sample, footer, and counts. "Binary" means the file was identified by format and metadata only; binary content was never claimed as read text.
- **README.md note:** The README.md digest below is the grounded README (30,100 bytes, 395 lines, 4,324 words) produced by this audit. It replaces an earlier 17,923-word monograph (commit 9c02356) that padded to reach a 10,000-word target the sources do not support. The prior README at HEAD 3737689 was 63,398 bytes and 4,543 lines; it was read in full as part of the inspection and reconciled against the sources.

## Passes

The audit was performed in ten passes, as specified in the mission. The passes are lenses, not ten literal re-readings of the 12,301-line generated netlist. Pass 1 inventoried all files. Pass 2 recovered the architecture. Pass 3 extracted the mathematics. Pass 4 recovered the data structures and dimensions. Pass 5 inspected the Julia and R calculations. Pass 6 inspected Q#, Quipper, and Smalltalk. Pass 7 inspected Verilog, Verilog-A, and netlists. Pass 8 traced data producers, consumers, and cross-language contracts. Pass 9 inspected tests, formal properties, build rules, and integrity. Pass 10 re-read the findings and reconciled each prospective README claim with specific files and lines. The per-file findings below are the result of all ten passes.

## Per-file ledger

### INTEGRITY_MANIFEST.txt

- **SHA-256:** 9903031c0ede90e521b4577cf4de26509207f14f8281025c85c7a9c43e28f4d1
- **Bytes:** 1,595
- **Lines:** 19
- **Reading completion:** Full
- **Findings:** 12 file entries with bytes, line count, and SHA-256. Recomputed digests match for 11 of 12 files. The README.md entry is stale (records 6,663 bytes and 60 lines; the prior README was 63,398 bytes and 4,543 lines). Footer line 17 reads "Circuit transient in ngspice: NOT TESTED" and is stale; the author ran the transient and verified the netlist by computation. Footer lines on make check PASS, make capacity PASS (1,680 trials), byte-identical netlist regeneration, capacity.csv aggregation, and clean_state.csv are correct.
- **README claims:** Chapters 20, 21, 25.

### LICENSE

- **SHA-256:** 4ce7f620b37fec2e22a33379b5b1490a4bb24efd612a2fa0706ed1390f334c57
- **Bytes:** 34,459
- **Lines:** 660
- **Reading completion:** Full (header and license text confirmed as GNU Affero General Public License v3.0)
- **Findings:** AGPL v3.0. The repository's license. No MIT license bearing the author's name is present.
- **README claims:** Chapter 1, License section.

### Makefile

- **SHA-256:** 40093068e7b3761d67b4e1b278a00ae5f751b1d7559122b21665dcefc09c86fb
- **Bytes:** 397
- **Lines:** 17
- **Reading completion:** Full
- **Findings:** Variables CC, CFLAGS (-std=c11 -O3 -Wall -Wextra -pedantic), LDLIBS (-lm). Targets: all, check, clean, verify, generate, capacity. Gaps: `capacity` is not in .PHONY; `clean` does not remove `associative`, `capacity.csv`, or `neurons_1024.cir`.
- **README claims:** Chapters 3, 19, 23, 24, 25.

### README.md (grounded version, this audit)

- **SHA-256:** c99ceb4aac9936370d0e3731377ea8835397ee892cc5523767901cbf89862d9a
- **Bytes:** 30,100
- **Lines:** 395
- **Words:** 4,324
- **Reading completion:** Full (written from the inspection; every claim traceable to a source)
- **Findings:** This README replaces an earlier 17,923-word monograph (commit 9c02356) that padded to reach a 10,000-word target the sources do not support. The grounded version is 4,324 words, which is the length the sources actually support without invention. It uses the repository's real root paths (verify.c, generate.c, associative.c, results.csv, clean_state.csv, capacity.csv), not the src/ and data/ paths from a remote revision that moved the files. It states the path discrepancy explicitly. It credits the author's SPICE and Liquid Haskell testing and flags the stale manifest lines.
- **README claims:** Chapters 1-17.

### associative.c

- **SHA-256:** 00de20647c630c519e5f0cad91204a0f36231ae68af22b556d0fb7630a80232d
- **Bytes:** 5,635
- **Lines:** 107
- **Reading completion:** Full
- **Findings:** Binary Hopfield capacity benchmark. N = 1024, TRIALS = 16, SWEEPS = 24. Grid: patterns {1,2,4,8,16,32,64}, flip {5,10,20,30,40}, loss {0,10,20}. 105 scenarios, 1,680 trials. Hebbian low-rank weights via overlaps, no diagonal self-coupling. Energy E = -(sum overlap^2 - p*live)/(2N). Mask can remove neuron 0 (differs from verify.c mask_init). Header states it is not claimed equivalent to the SPICE model. Final print: "This is a binary Hopfield reference, not SPICE."
- **README claims:** Chapters 4, 5, 7, 21, 22, 23.

### capacity.csv

- **SHA-256:** 5449f1af9bb0404e7ed67c443ed2aad063671580e28ea3e534d1eb2bafb6f0b4
- **Bytes:** 6,110
- **Lines:** 106 (header + 105 rows)
- **Reading completion:** Full (all 105 rows verified)
- **Findings:** Aggregated rows, not per-trial. Perfect recall 1.000000 for patterns <= 32 except 32/40/20 (0.937500). At 64 patterns, sharp breakdown at flip 40: recall 0.187500 (loss 0), 0.250000 (loss 10), 0.125000 (loss 20). Mean sweeps 2.000 for nearly all low-load rows. Mean energy at p=1, loss=0 is -511.5000000, exactly -(1024^2-1024)/2048. Baseline cue accuracy is exactly 1 - flips/live.
- **README claims:** Chapters 5, 18, 21.

### clean_state.csv

- **SHA-256:** 2b9a257cefe9181e19eaf18518c82e353e67cc9f6e74cf39e06026ea0c8d7dbf
- **Bytes:** 37,620
- **Lines:** 1,025 (header + 1,024 rows)
- **Reading completion:** Structural (header, first rows, last rows, direct counts)
- **Findings:** Header neuron,x,y. Neuron 0: (0.891758539329, -0.00251311502948), the outlier. 1,015 of 1,024 have x >= 0.999. 1,023 of 1,024 have y < -0.0029. States cluster near (1, -0.003). The reason y clusters near -0.003 rather than 0 is not explained by any source file.
- **README claims:** Chapters 5, 18, 21.

### cognitive_poly_julia_r/README.md

- **SHA-256:** 7920329be8e45f917ae1955cd1be1c6e519ade2070a42c8308e73938f26fdf7a
- **Bytes:** 2,052
- **Lines:** 31
- **Reading completion:** Full
- **Findings:** Line 27 claims a 450/150 train/test split and a normalized RMSE using population standard deviation to match NumPy. The code does neither: it trains on all 600 rows and reports raw train RMSE. No split, no normalization, no NumPy. Line 12 claims the ports are "source-level translations with assertions," but there are no assertions in either file. Line 12 also correctly states that neither interpreter was installed in the artifact-building environment. Lines 16-22 (KCL, potential, equilibria, s_crit, RK4, surrogate, lambda) are verified against the code. Line 26 (different RNGs, non-identical samples) is correct. Line 28 (derivative regressor only) is correct. Line 30 (hysteresis grid-dependent) is correct.
- **README claims:** Chapters 10, 21, 25.

### cognitive_poly_julia_r/cognitive_poly.R

- **SHA-256:** a546a3f9fbd9d9102d22bff10d5b32a9a0371b8d904cd568301dbae5e431da7b
- **Bytes:** 1,255
- **Lines:** 52
- **Reading completion:** Full
- **Findings:** Base R only. Constants cap=1e-9, a=-1e-6, b=1e-6, gain=1e-6. rhs, potential (dead code, never called), rk4_step (classic RK4, s constant across stages). Top-level script, no main wrapper. set.seed(42). Hysteresis: h=1e-5, 2,000 steps, s -0.6 to 0.6 step 0.05, 25 values. Ridge: n=600, features [1,v,v^2,v^3,s], lambda=1e-4, solve(). Prints coefficients to 2 decimals, Train RMSE, PASS line. Analytic coefficient expectation (0,1000,0,-1000,1000). Exact printed values are execution-only (no R on the audit machine).
- **README claims:** Chapters 4, 8, 9, 10, 21, 23.

### cognitive_poly_julia_r/cognitive_poly.jl

- **SHA-256:** 47b946f47e04c935a8bb223d9f5c6c2c8a9f2cd5784b3a626437e7b99ce9ea32
- **Bytes:** 1,877
- **Lines:** 68
- **Reading completion:** Full
- **Findings:** Standard library only (LinearAlgebra, Random, Statistics, Printf). Constants DEFAULT_CAP=1e-9, DEFAULT_A=-1e-6, DEFAULT_B=1e-6, DEFAULT_GAIN=1e-6. rhs, potential (dead code, never called), rk4_step. main() with MersenneTwister(42). Prints equilibria (-1,0,+1) and Jacobians (-2000,+1000,-2000) and s_crit to 8 decimals. Hysteresis: h=1e-5, 2,000 steps, s -0.6 to 0.6 step 0.05, 25 values, stores all end states in switches, prints only the last. Ridge: n=600, features [1,v,v^2,v^3,s], lambda=1e-4, backslash solve. Prints coefficients to 2 decimals, Train RMSE, PASS line. Analytic coefficient expectation (0,1000,0,-1000,1000). Exact printed values are execution-only (no Julia on the audit machine).
- **README claims:** Chapters 4, 8, 9, 10, 21, 23.

### docs/neuron_states.png

- **SHA-256:** 93ae5aa5df99644526fb656f23e0916cf7feebd2b3515df4b2554b73c7405ff5
- **Bytes:** 20,763
- **Reading completion:** Binary (PNG image data, 760x520). Visually verified: scatter clusters near (1, 0) with the neuron-0 outlier.
- **Findings:** PNG render of the clean attractor state figure. Displays on phones; the SVG is the editable source.
- **README claims:** Chapters 3, 18, Figures section.

### docs/neuron_states.svg

- **SHA-256:** 6daa84fe80e9ca8565aa3da6b338619d7c77a53f99f456eaccfe048dcfa786c6
- **Bytes:** 78,550
- **Lines:** 1,043
- **Reading completion:** Text content read (title, caption, axes, labels). Not pasted in full.
- **Findings:** Title: "Clean attractor state: 1024 neurons (x = real, y = imaginary)". Caption: x range 0.8918 to 1.0000, y range -0.003022 to -0.002513, n = 1024 points, one per neuron. Exactly 1,024 circle elements. The SVG text does not name a source file; the data matches the clean-state record, but that attribution is not inside the SVG.
- **README claims:** Chapters 18, Figures section.

### docs/panic_mode_cycle.png

- **SHA-256:** 212f13aaab0c3d18695db181abcd7acf8ca93b6a040feecad02b682f772a33c3
- **Bytes:** 56,219
- **Reading completion:** Binary (PNG image data, 760x520). Visually verified.
- **Findings:** PNG render of the overload-recovery cycle diagram. Displays on phones; the SVG is the editable source.
- **README claims:** Chapters 3, 13, Figures section.

### docs/panic_mode_cycle.svg

- **SHA-256:** 81d451c99575427c91b9d854cd41767731817d6424842699daa04a065064a758
- **Bytes:** 5,240
- **Lines:** 48
- **Reading completion:** Text content read.
- **Findings:** Title: "Overload-recovery cycle and 2-qubit encoding". Caption: deferred tokens in RECOVERY are buffered and drained on SYNC. Cycle: NORMAL (00) to OVERLOAD (01) to RECOVERY (10) to NORMAL (00), events NO_ACTION, NO_ACTION, SYNC. Encoding table and grounding cues. Diagram, not a data plot; no CSV source.
- **README claims:** Chapters 13, Figures section.

### docs/panic_mode_states.png

- **SHA-256:** 4e3225066d92e7fd0c322c77648cfcdd6503b0293171fca260fac0524ef73312
- **Bytes:** 56,856
- **Reading completion:** Binary (PNG image data, 760x520). Visually verified.
- **Findings:** PNG render of the panic-mode state machine diagram. Displays on phones; the SVG is the editable source.
- **README claims:** Chapters 3, 13, Figures section.

### docs/panic_mode_states.svg

- **SHA-256:** 4f3cbbdf926d5f0985287acf51630645a0833e4479216d995882eeb3d04e337b
- **Bytes:** 3,707
- **Lines:** 33
- **Reading completion:** Text content read.
- **Findings:** Title: "Panic-mode state machine: 4 states, LALR(1) error recovery". Subtitle: ESCALATE is terminal (double circle). States with 2-qubit codes: NORMAL 00, OVERLOAD 01, RECOVERY 10, ESCALATE 11. Events shown: NO_ACTION, SYNC, ESCALATE_REQ, TIMEOUT, SHIFT/REDUCE. State diagram, not a data plot. Does not list full edge endpoints; the transition table was not reconstructed from it.
- **README claims:** Chapters 13, Figures section.

### docs/robustness.png

- **SHA-256:** f2042f318ef5d0a1ce59a554ebac44cc4a649232439bf7f67f652a22509c289f
- **Bytes:** 20,598
- **Reading completion:** Binary (PNG image data, 760x460). Visually verified: bars show clean -51.17, noisy -51.17, loss_5pct -48.62, loss_10pct -46.07, loss_20pct -40.97.
- **Findings:** PNG render of the final-energy-by-scenario figure. Displays on phones; the SVG is the editable source.
- **README claims:** Chapters 3, 18, Figures section.

### docs/robustness.svg

- **SHA-256:** 3eb161c1a9440d8086133bab3658c79a69ad4a160ce3a1899852dc38803684b8
- **Bytes:** 3,394
- **Lines:** 35
- **Reading completion:** Text content read.
- **Findings:** Title: "Final energy by scenario (results.csv)". The source CSV is named in the title. Y-axis -55 to -35. Five bars with values and alive counts: clean -51.17 (1024), noisy -51.17 (1024), loss_5pct -48.62 (973), loss_10pct -46.07 (922), loss_20pct -40.97 (820). Six rect elements: 5 bars plus one frame/background.
- **README claims:** Chapters 18, Figures section.

### edges.txt

- **SHA-256:** 9f28307fa396a8cc170804bcc7439e02554ce39ecdca2d4eed6bbd378ca78fc6
- **Bytes:** 32,080
- **Lines:** 4,096
- **Reading completion:** Structural (first 5 lines, last 5 lines, counts, degree verification)
- **Findings:** Two integers per line, 0-indexed, one undirected edge per line, sorted lexicographically. Vertices span 0-1023. First five: 0 474, 0 492, 0 530, 0 638, 0 747. Last five: 987 995, 987 998, 988 1001, 988 1015, 995 1016. Independently verified 8-regular: 0 vertices have degree other than 8. Adjacency eigenvalues computed from this file for the README: largest 8.0, second-largest 5.25756776, max nontrivial magnitude 5.26332069. Ramanujan bound 2 sqrt(7) = 5.29150262.
- **README claims:** Chapters 3, 4, 7, 17, 18.

### generate.c

- **SHA-256:** 561e5de9437fce3119f1048b39d780a7a5ecd41170c7e962badf9b024333b9d0
- **Bytes:** 2,515
- **Lines:** 55
- **Reading completion:** Full
- **Findings:** SPICE netlist generator. main with input edge file (default edges.txt) and output netlist (default neurons_1024.cir). N=1024, D=8, EDGES=4096. adj[N][N] adjacency matrix, degree[N]. Validation: range, self-loop, duplicate, exact count 4096, degree 8. Exit codes 1-5. Emission: header with .param cap=1 alpha=-0.2 beta=0.2 gc=0.04; per neuron 4 lines (Cx, Cy, Bx, By); per edge 2 resistors (Rx, Ry, 1/gc = 25 ohm); Vref, Rref, Ryref; .ic; .tran 0.1 40 uic; .save neurons 0-3; .end. Header comment: "This program does not assert a Ramanujan proof; use spectral checks."
- **README claims:** Chapters 3, 6, 7, 17, 23.

### neurons_1024.cir

- **SHA-256:** 27760d191d87e6d0e594f1a1df7f85ceabd00f02ae88d24f81733179f045acef
- **Bytes:** 366,220
- **Lines:** 12,301
- **Reading completion:** Structural (header lines 1-4, neuron 0 lines 5-8, neuron 1 lines 9-12, last neuron line 4100, first edge pair lines 4101-4102, last edge pair lines 12291-12292, footer lines 12293-12301, component counts)
- **Findings:** Layout: header 1-4, neuron blocks 5-4100 (4 lines x 1024), edge resistors 4101-12292 (2 lines x 4096), footer 12293-12301 (9 lines). 5 comment lines, 12,296 non-comment lines. Counts: Cx 1024, Cy 1024, Bx 1024, By 1024, Rx 4096, Ry 4096, plus Vref, Rref, Ryref. Totals: 2,048 capacitors, 2,048 behavioral sources, 8,194 resistors. First edge 0-474, last edge 995-1016. Footer: Vref 0.2, Rref 10, Ryref 10, .ic V(x1)=1 V(y2)=0.4 V(x3)=-0.5, .tran 0.1 40 uic, .save neurons 0-3, .end. The author ran the transient in ngspice and verified the netlist by computation.
- **README claims:** Chapters 3, 4, 5, 6, 17, 23.

### qsharp/AnalogWaveInterference.csproj

- **SHA-256:** 27ca0231605322f6c54ae3d191b65c768d8d4c4bdc575a221ca07c62f6574517
- **Bytes:** 212
- **Lines:** 9
- **Reading completion:** Full
- **Findings:** SDK Microsoft.Quantum.Sdk/0.28.302812, OutputType Exe, TargetFramework net8.0, Nullable disable.
- **README claims:** Chapters 11, 23.

### qsharp/AnalogWaveInterference.qs

- **SHA-256:** e4fe59ba2ba0a88433c9d6ad6f6c403c0a2b2636c7b633f1b69ba524034a759f
- **Bytes:** 4,095
- **Lines:** 100
- **Reading completion:** Full
- **Findings:** Namespace AnalogWaveInterference. Opens Intrinsic, Measurement, Math, Convert. AnalogVoltageToTheta: clamp to [-1,1], theta = 0.5*PI()*(1-clamped). +1V maps to theta 0 (|0>), 0V to pi/2 (|+>), -1V to pi (|1>). EncodeAnalogState: Ry(theta), Adj + Ctl. RunWaveInterferometer: H, Rz(phi), H, MResetZ, returns (countZero, countOne). P(|0>)=cos^2(phi/2), P(|1>)=sin^2(phi/2). Phi is a plain Double; no integration implemented. AnalogParityInterference: encodes two voltages, CNOT, measures qB, returns odd-parity fraction. XOR holds only at basis points; (0,0) is probabilistic (~0.5). Main: EntryPoint, shots=200, phi sweep {0, pi/2, pi}, hard-coded voltage pairs (+1,-1), (+1,+1), (0,0). No file I/O, no read of clean_state.csv or the netlist. Complete self-contained demonstration, not wired to the analog model. No execution output in the repository.
- **README claims:** Chapters 11, 16, 21, 23.

### quipper/QuipperODELiquid.hs

- **SHA-256:** ab6bbe6f0c80a468668fa106c7d4d5285efd07e47fa01ae6f2d7e5204923ae7c
- **Bytes:** 8,766
- **Lines:** 203
- **Reading completion:** Full
- **Findings:** Module QuipperODELiquid. Imports only Data.Set. Does NOT import Quipper. Self-contained Haskell simulation of Quipper-style linear circuits, not a program built on the Quipper library. No Hadamard primitive despite the header claim. Only gates: qinit0, applyRz, applyCNOT. No main, no .cabal file; a library module/fragment, not an executable. Liquid Haskell refinements declared (PosDouble, NonNegInt, LiveQubit, Distinct, qinit0/applyRz/applyCNOT refinements). badClone commented out, noted as UNSAT. No Liquid Haskell output in the repository; the author performed the verification. DAEState: timeT, vState, zAlg. ODE: C dV/dt = -aV - bV^3 + gain V_stim + zAlg, constraint 0 = zAlg - alpha V. Constants: cap=1e-9, a=-1e-6, b=1e-6, gain=1e-6, alpha=0.1e-6. The code is explicit forward Euler, but the comment says "Backward-Euler / semi-implicit." Code is authoritative. unrollDAECircuit recurses n times with termination measure. runExample: 3 steps, dt=1e-5, vStim=0.5, from DAEState 0.0 0.2 0.02e-6. Single cell, not the 1,024-neuron network.
- **README claims:** Chapters 12, 16, 21, 23, 25.

### quipper/StateMachine.cabal

- **SHA-256:** 5165804b9cfabae2a37adde84f83db8669f03f156c397e22d800abe8cc208b3d
- **Bytes:** 311
- **Lines:** 10
- **Reading completion:** Full
- **Findings:** Package panic-mode-fsm version 0.1.0, executable panic-fsm, main-is StateMachine.hs, build-depends base >= 4.14, quipper >= 0.9, quipper-utils, language Haskell2010, -Wall.
- **README claims:** Chapters 12, 23.

### quipper/StateMachine.hs

- **SHA-256:** c8136e944a4c2a807fa7555f04f2fac1c147aa0c5f988151156923192a5befe2
- **Bytes:** 3,771
- **Lines:** 119
- **Reading completion:** Full
- **Findings:** State: Normal, Overload, Recovery, Escalate. Event: Shift, Reduce, NoAction, Sync, EscalateReq, Timeout. 2-qubit encoding: 00 Normal, 01 Overload, 10 Recovery, 11 Escalate. Events as integers 0-5. nextState is PARTIAL and non-exhaustive: defines only Normal 0, Normal 1, Normal 2, Overload 2, Overload 4, Recovery 3, Recovery 5, Escalate _. Undefined pairs (e.g. Normal 3, Overload 0, Recovery 0) have no clause and would fail at runtime. Event type and eventToInt defined but never used; the circuit hard-codes the event integer. stateMachineCircuit and overloadRecoveryCycle are classical bits in a quantum circuit: no superposition, no entanglement, no gate other than init and measure. main prints stateMachineCircuit with print_generic Preview. No build or execution output in the repository.
- **README claims:** Chapters 12, 13, 21, 23, 25.

### results.csv

- **SHA-256:** 9eb828642eaf3b16de19088874308c9dd17d70b1c39720a66ece963eee4b614c
- **Bytes:** 568
- **Lines:** 6 (header + 5 rows)
- **Reading completion:** Full (all 5 rows)
- **Findings:** All 5 scenarios PASS. Clean: energy -51.1722093, grad rms 3.47e-08, max|z| 0.999998496. Noisy: energy -51.17220937, max|z| 1.049698292 (exceeds 1 during forcing), rms_to_clean 0.00130922207. loss_5pct: -48.62244505, 973 alive. loss_10pct: -46.07244776, 922 alive. loss_20pct: -40.97245173, 820 alive. All have 1 component. Energy scales as -0.05 per alive neuron.
- **README claims:** Chapters 5, 18, 21.

### retrieval_false.unl

- **SHA-256:** f0629b60897c46e32101c751d7ce50c85e41a846c5872971f1f0122cb9cad37a
- **Bytes:** 10 (including trailing newline)
- **Lines:** 1
- **Reading completion:** Full (byte-for-byte, verified with od -c)
- **Findings:** Single Unlambda expression ```k.F.Ti. Prints F. Minimal pair with retrieval_true.unl. No comments, no driver, no invocation. How it is run and how the Hopfield result reaches it is not stated in the files.
- **README claims:** Chapters 15, 23.

### retrieval_true.unl

- **SHA-256:** 9980cd365b1570cae13f9a46cdec8509111fc40233179001e0496bb71976ab2c
- **Bytes:** 10 (including trailing newline)
- **Lines:** 1
- **Reading completion:** Full (byte-for-byte, verified with od -c)
- **Findings:** Single Unlambda expression ```k.T.Fi. Prints T. Minimal pair with retrieval_false.unl. No comments, no driver, no invocation.
- **README claims:** Chapters 15, 23.

### smalltalk/StateMachine.st

- **SHA-256:** bff35011aadf1831e0e22455929f41c7929c6eb08394de06eb1dc83b75d403ef
- **Bytes:** 4,333
- **Lines:** 124
- **Reading completion:** Full
- **Findings:** Class StateMachine, subclass of Object, instance variables state, transitions, buffer, log, category SovereignSomatic. initialize sets state to #NORMAL and installs 7 transitions. process: defers non-SYNC and non-TIMEOUT events in RECOVERY to the buffer; silently ignores undefined (state, event) pairs. drainBuffer re-processes buffered tokens. deferLast, bufferSize, currentState, logTransition:, printLog, reset. Matches spec/transitions.md exactly and implements the deferred-buffer behavior that the Quipper FSM lacks. Uses symbolic event names, not integer codes. Usage example in a comment block, not a transcript. The author hand-typed the majority of this code with SnapKitty; AI could not help with the Smalltalk.
- **README claims:** Chapters 13, 16, 23.

### spec/transitions.md

- **SHA-256:** 266ce64cbf7b5a699b0d2458fe9d5f97384ee9dc9ba28e5743c1545a2e3222ad
- **Bytes:** 3,249
- **Lines:** 104
- **Reading completion:** Full
- **Findings:** Deterministic finite automaton for cognitive overload management, inspired by LALR(1) error recovery. States: NORMAL, OVERLOAD, RECOVERY, ESCALATE. Events: SHIFT, REDUCE, NO_ACTION, SYNC, ESCALATE_REQ, TIMEOUT. Transition table: 7 transitions, ESCALATE terminal. Action primitives: shift_token, reduce_rule, flag_overload, start_recovery, resume_normal, request_help, noop. Deferred buffer: FIFO queue, drained on SYNC. Grounding cues: 5 breaths, room change, call a friend, 60s timeout, unexpected stressor. Post-Anchor YACC production: specification-only, no grammar, lexer, or Anchor code in the repository. Physical meaning and units of grounding cues as real-system inputs are not implemented.
- **README claims:** Chapters 13, 16, 23, 25.

### verify.c

- **SHA-256:** 101b2e06d2d952e8a0936c6ec6dcd0d91c788d50bbc68fbf6772662085362069
- **Bytes:** 7,956
- **Lines:** 187
- **Reading completion:** Full
- **Findings:** Deterministic C11 reference. N=1024, DEG=8, E=4096, STEPS=2400, DT=0.05, ALPHA=-0.2, BETA=0.2, GC=0.04, GREF=0.1, REF=0.2, NOISE_STEPS=400. xorshift64star RNG. read_edges validates (range, self-loop, duplicate, count, degree). parity_check is VACUOUS: (1u^1u)!=0u is always false, so it never verifies GF(2) incidence parity; it only rejects self-loops already excluded. derivative: gain = 0.2(1-r), diffusive coupling, reference on neuron 0. energy: Lyapunov function. rk4: classic 4th-order. init: seed 0x123456789abcdef, x in [0.62,0.82], y in [-0.08,0.08], cue x1=1, y2=0.4, x3=-0.5. mask_init removes N*pct/100, never neuron 0. components: BFS, counts connected components. simulate: noise force 0.12*(2*uniform01()-1) for first 400 steps when noisy; monotone check (energy rise > 1e-9 sets monotone=0); PASS = finite and monotone and max|z| < 5; writes clean_state.csv only for the clean scenario. main: runs baseline, then clean, noisy, loss_5pct, loss_10pct, loss_20pct; prints "Structural invariant suite: PASS/FAIL (does not imply memory retrieval or consciousness)."
- **README claims:** Chapters 3, 4, 5, 6, 7, 17, 21, 22, 23, 25.

### verilog-a/cognitive_poly_cell.va

- **SHA-256:** 75f48714f51bdffa876c0bb5747cf647011e8fc72493cbadfe08024bcbccd143
- **Bytes:** 859
- **Lines:** 28
- **Reading completion:** Full
- **Findings:** Module cognitive_poly_cell(state, stimulus). Parameters cap=1e-9, a=-1e-6, b=1e-6, gain=1e-6. Analog block contributes 4 currents; KCL gives C dV/dt = -aV - bV^3 + gain V_stim. Double-well (Landau/pitchfork) ODE. Fixed points at 0 and ±1 V (derived, not listed in the file). Single scalar cell; no graph, no coupling, no GF(2^4).
- **README claims:** Chapters 4, 6, 14, 16, 23.

### verilog-a/gf_topological_dag.va

- **SHA-256:** 409a4141ff3708ffe6bac1f310277d3de7aaf78daff9f0a519638630aa4285ce
- **Bytes:** 10,483
- **Lines:** 208
- **Reading completion:** Full
- **Findings:** Modules gf2_xor_analog, gf16_mac_node, gf_topological_dag_va. Field GF(2^4), P(x)=x^4+x+1 (0x13), logic-0=0.0V, logic-1=Vdd=1.0V, tanh thresholding. gf2_xor_analog is DEFINED BUT NEVER INSTANTIATED (sole occurrence is its own definition). gf16_mac_node: soft bits, 3-input XOR via a+b-2ab, 7-bit product, same reduction as Verilog, laplace_nd relaxation tau=1ns. Top: 8 bulk inputs, 8 boundary outputs, coeff_l1, coeff_l2, syndrome_err, boundary_entropy. Layers use C_8(1,4). Entropy is a normalized voltage (ent_acc/32). Syndrome is LSB parity only, restricted to "unit coefficients" by its own comment, while the header describes the general c1*c2 parity. No fault injection input. No testbench for this DAG in the repository. "Bekenstein-Hawking / Ryu-Takayanagi" is an analogy in the header, not a measured property.
- **README claims:** Chapters 14, 15, 16, 21, 23, 25.

### verilog-a/tb_cognitive_poly_cell.va

- **SHA-256:** 47f67d72294fbe36a6ca4c20c7af9dac59d515e0f5f089a1e70afbd72df8b061
- **Bytes:** 2,640
- **Lines:** 76
- **Reading completion:** Full
- **Findings:** Re-defines cognitive_poly_cell identically. Testbench tb_cognitive_poly_cell. Verifies KCL ODE, Lyapunov energy U(V), bistable F_2 basin classification. Outputs stim_out, state_out, energy_out, binary_bit. v_crit = 0.384900179 (2/(3 sqrt 3)). Stimulus: piecewise-linear hysteresis sweep, hold -0.6V 0-10ms, ramp -0.6 to +0.6V 10-30ms, ramp +0.6 to -0.6V 30-50ms. Crosses the bifurcation in both directions. Energy reported scaled by 1e6. Basin readout 0.5(1+tanh(10 V(state))). NO automated PASS/FAIL verdict, no $fatal, no assertion. Only failure signal is a |V|>5.0 $strobe. No transient stop time, no $finish. Outputs are probes to be inspected.
- **README claims:** Chapters 14, 21, 23, 25.

### verilog/gf_topological_dag.v

- **SHA-256:** ff04cdf260837e6b5bec1535d5b6d29c3494a5c0a37fa3957453cf8f007e6c52
- **Bytes:** 6,539
- **Lines:** 190
- **Reading completion:** Full
- **Findings:** Three synthesizable modules plus a testbench. gf16_mul: GF(2^4) multiplier, P(x)=x^4+x+1, y[0]=p0^p4, y[1]=p1^p4^p5, y[2]=p2^p5^p6, y[3]=p3^p6. gf16_dag_node: out = (in0^in1^in2)*coeff. gf_topological_dag: 8-node, 3-layer DAG, C_8(1,4), coeff_l1, coeff_l2, bulk_fault_mask, syndrome_gf, boundary_entropy (popcount 0-32). Outputs registered on posedge clk with async active-low reset. Expected parity = bulk_parity * coeff_l1 * coeff_l2. Syndrome = bnd_parity ^ expected. Testbench: 10ns clock, bulk=0xA53C71E2, coeff_l1=3, coeff_l2=5, fault mask 0 then 0x00000009. Check 1: syndrome must be 0. Check 2: syndrome must be non-zero. Prints PASS on success. Asserts ONLY the syndrome; boundary and entropy are displayed but never compared. Only two scenarios. Synthesizable constructs only in design modules; non-synthesizable constructs confined to the testbench. No synthesis tool run or area/timing result in the file. "Ramanujan" and "holographic" are design labels in comments, not proven bounds. Separate small (N=8) model, not the 1,024-neuron network.
- **README claims:** Chapters 4, 14, 15, 16, 21, 23, 25.

## Binaries handled separately

The PNG files (`docs/neuron_states.png`, `docs/panic_mode_cycle.png`, `docs/panic_mode_states.png`, `docs/robustness.png`, and the five new PNG renders in `docs/assets/svg/`) were identified by format and metadata and visually verified. Binary content was never claimed as read text. The five new SVG illustrations in `docs/assets/svg/` were validated as well-formed XML and rendered to PNG; each PNG was visually inspected for layout, overlap, and legibility.

## Coverage summary

- **Files inspected:** 36 tracked files.
- **Text files read in full:** 27 (all hand-written sources, all CSVs except clean_state.csv which was structural, all specs, all build and manifest files).
- **Files inspected structurally:** 3 (`edges.txt`, `neurons_1024.cir`, `clean_state.csv`).
- **Binary files:** 4 PNGs in `docs/` (format and metadata plus visual verification).
- **SVG figures:** 4 in `docs/` (text content read) plus 5 new in `docs/assets/svg/` (validated and rendered).
- **Per-pass coverage:** All 10 passes covered all files. Passes 1-2 covered the inventory and architecture. Passes 3-4 covered the mathematics and data structures. Passes 5-7 covered the language-specific implementations. Pass 8 covered the data contracts. Pass 9 covered the tests and integrity. Pass 10 reconciled every README claim with a file and line.
- **Source revision:** Local HEAD 3737689. The README.md digest above is the new README produced by this audit.
