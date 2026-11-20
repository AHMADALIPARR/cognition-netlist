# Cognition Netlist — Analog Polynomial State Network + GF(2⁴) Topological DAG

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)

This repository contains two complementary artifacts:

1. **Analog SPICE netlist** (1024-neuron complex polynomial state network on an 8-regular expander graph).
2. **Digital Verilog RTL + Verilog-A models** for a finite-field GF(2⁴) arithmetic DAG over a sparse Ramanujan-expander topology, with holographic parity-check syndrome and boundary entropy readout, plus a cubic polynomial analog cell.

**License:** GNU Affero General Public License v3.0 — see [LICENSE](LICENSE).

---

## Contents

| Path | Description |
|------|-------------|
| `generate.c` | C11 generator: validates 8-regular graph and emits SPICE netlist |
| `edges.txt` | 4096 undirected edges of the 8-regular graph on 1024 vertices |
| `neurons_1024.cir` | Pre-generated SPICE netlist (1024 complex neurons) |
| `verilog/gf_topological_dag.v` | Synthesizable digital Verilog RTL + self-checking testbench (GF(2⁴) DAG) |
| `verilog-a/gf_topological_dag.va` | Verilog-A continuous-time behavioral model of the same DAG |
| `verilog-a/cognitive_poly_cell.va` | Verilog-A cubic polynomial cell (double-well memory / gradient flow) |
| `LICENSE` | Full AGPLv3 text |

---

## 1. Analog SPICE artifact (1024-neuron network)

This is a **simulation artifact**, not a biological consciousness model. Every complex state has two circuit nodes, `x{i}` and `y{i}`, with a polynomial nonlinear current source and capacitor at each node. The graph has 1024 vertices, degree 8, and 4096 undirected edges. The graph was generated as a seeded random regular graph (numerical spectral Ramanujan check: max nontrivial eigenvalue ≈ 5.26332 vs 2√7 ≈ 5.29150).

### Build & run

```sh
cc -std=c11 -O2 -Wall -Wextra -pedantic generate.c -o generate
./generate edges.txt neurons_1024.cir
ngspice -b neurons_1024.cir -r neurons.raw
```

Requires `ngspice`. A full transient was not verified in the original package.

### Dynamics

Each \( z_i = x_i + i y_i \) obeys the relaxation

\[
C \frac{dz_i}{dt} = -(\alpha + \beta |z_i|^2) z_i - g_c (Lz)_i + \text{external}_i.
\]

The resistor network realizes the graph Laplacian over real conductances.

---

## 2. Digital Verilog RTL — GF(2⁴) topological DAG

**File:** `verilog/gf_topological_dag.v`

- Finite field GF(2⁴) with irreducible \( P(x) = x^4 + x + 1 \) (0x13).
- 3-layer DAG, \( N = 8 \) nodes per layer (Bulk → Interior → Boundary).
- Inter-layer connectivity: 3-regular Ramanujan circulant \( C_8(1,4) \) (Wagner / Möbius ladder); \( |\lambda_2| = 1+\sqrt{2} < 2\sqrt{2} \).
- Holographic parity: global boundary sum equals \( c_1\cdot c_2\cdot\sum(\text{bulk}) \); mismatch produces a non-zero GF(2⁴) syndrome.
- `boundary_entropy` counts active boundary bits (area-law proxy).

### Simulate with Icarus Verilog (recommended)

```sh
# Install if needed: apt install iverilog   or   brew install icarus-verilog
cd verilog
iverilog -g2012 -o gf_dag_tb gf_topological_dag.v
vvp gf_dag_tb
```

Expected output:

```
No-fault: boundary=... syndrome=0 entropy=...
Faulty:   boundary=... syndrome=... entropy=...
PASS: All GF(2^4) topological DAG checks passed.
```

The self-checking testbench injects a bulk fault and asserts that the syndrome becomes non-zero.

### Other simulators

- **Verilator** (lint / cycle-accurate C++ model):
  ```sh
  verilator --lint-only -Wall gf_topological_dag.v
  ```
- **Synopsys VCS / Cadence Xcelium / Mentor Questa** — standard SystemVerilog flow; the design is pure synthesizable RTL + a simple testbench.

---

## 3. Verilog-A continuous-time models

**Files:**
- `verilog-a/gf_topological_dag.va` — continuous analog version of the GF(2⁴) DAG using smooth tanh soft-bits and first-order RC relaxation.
- `verilog-a/cognitive_poly_cell.va` — single cubic polynomial cell.

### Physical & ODE verification (cognitive_poly_cell)

By KCL the cell obeys

\[
C\frac{dV}{dt} = -aV - bV^3 + g V_{\text{stim}} = 10^3(V - V^3 + V_{\text{stim}}),
\]

which is a gradient flow on the double-well potential

\[
U(V) = \tfrac12 a V^2 + \tfrac14 b V^4 - g V_{\text{stim}} V.
\]

At \( V_{\text{stim}} = 0 \), RK4 integration and Jacobian analysis confirm two stable \( \mathbb{F}_2 \) memory wells at \( V^* = \pm 1.0\,\text{V} \) (Jacobian \( = -2000\,\text{s}^{-1} \)) separated by an unstable saddle at \( 0\,\text{V} \) (\( +1000\,\text{s}^{-1} \)). Hysteresis switching occurs past the theoretical saddle-node threshold

\[
|V_{\text{stim}}| = \frac{2}{3\sqrt{3}} \approx 0.3849\,\text{V}
\]

(observed numerically at \( \pm 0.4000\,\text{V} \)).

### Why ML is used here

When many `cognitive_poly_cell` nodes are coupled in a Ramanujan-expander topological DAG, closed-form inversion of the \( N \)-coupled nonlinear cubic ODEs and compensation for analog process mismatch are intractable analytically. Machine learning:

- learns fast surrogate models of the multi-node dynamics,
- calibrates device variation (example: Ridge regression under \( \pm 15\% \) variation in \( a,b,C \) recovers nominal coefficients \( [0, 1000, 0, -1000, 1000] \) as approximately \( [-8.34, 976.69, 8.43, -968.77, 1014.07] \) with 18 % relative RMSE),
- learns boundary-to-bulk decoders for the holographic parity map.

### Why formal verification remains essential

ML surrogates can extrapolate poorly outside their training basins. Pairing ML with classical KCL / Lyapunov checks — for example verifying \( b > 0 \) coercivity (\( w_3 = -968.77 < 0 \)) and boundedness in a testbench for `cognitive_poly_cell.va` — guarantees that trajectories stay finite and remain inside the intended \( \mathbb{F}_2 \) attractors.

### Running the Verilog-A models

These require an analog / mixed-signal simulator that supports Verilog-A (or Verilog-AMS):

| Simulator | Notes |
|-----------|--------|
| **Cadence Spectre / ADE** | Native Verilog-A support; include `disciplines.vams` / `constants.vams` from the Cadence installation. |
| **Synopsys HSPICE / CustomSim** | Verilog-A support via `*.va` inclusion. |
| **Siemens (Mentor) Eldo / AFS** | Verilog-A supported. |
| **ngspice** (experimental) | Limited Verilog-A; prefer pure SPICE netlists for the 1024-neuron design. |
| **Xyce** | Open-source; partial Verilog-A support. |

Typical flow (Cadence example):

```tcl
// In ADE or ocean script
ahdl_include "verilog-a/cognitive_poly_cell.va"
ahdl_include "verilog-a/gf_topological_dag.va"
// Instantiate, set DC/transient analysis, probe syndrome_err and boundary_entropy
```

---

## Mathematical notes (shared)

- **GF(2) / GF(2⁴)** — binary adjacency, incidence / parity checks, optional code-based retrieval. Not the same arithmetic as the continuous analog state.
- **Ramanujan / expander spectral analysis** — certifies connectivity and robustness; the random-regular graph meets a numerical eigenvalue test but is not a proven constructive Ramanujan family.
- **Holographic / boundary entropy** — the digital and Verilog-A models expose an explicit syndrome and an area-law style bit-count on the boundary cut. These are information-theoretic analogies, not a physical black-hole or AdS/CFT implementation.

---

## License

This project is free software licensed under the **GNU Affero General Public License v3.0**. See the [LICENSE](LICENSE) file for the complete terms. Network use of a modified version requires offering the corresponding source.
