# Cognition Netlist — Analog Polynomial State Network + GF(2⁴) Topological DAG

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)

This repository contains complementary artifacts around a 1024-node complex analog polynomial state network, discrete associative memory, finite-field topological DAGs, and native Julia/R ports of the cubic cognitive cell.

**License:** GNU Affero General Public License v3.0 — see [LICENSE](LICENSE).

---

## Contents

| Path | Description |
|------|-------------|
| `generate.c` | C11 generator: validates 8-regular graph and emits SPICE netlist |
| `verify.c` | Deterministic C11 reference ODE integrator + robustness scenarios |
| `associative.c` | Binary Hopfield associative-memory capacity benchmark (Hebbian, no diagonal) |
| `Makefile` | Build targets for `generate`, `verify`, `associative` |
| `results.csv` | Summary of clean / noisy / node-loss scenarios (structural invariants PASS) |
| `retrieval_true.unl` / `retrieval_false.unl` | Minimal Unlambda retrieval stubs |
| `verilog/gf_topological_dag.v` | Synthesizable digital Verilog RTL + self-checking testbench (GF(2⁴) DAG) |
| `verilog-a/gf_topological_dag.va` | Verilog-A continuous-time behavioral model of the same DAG |
| `verilog-a/cognitive_poly_cell.va` | Verilog-A cubic polynomial cell (double-well memory / gradient flow) |
| `verilog-a/tb_cognitive_poly_cell.va` | Verification harness: KCL/Lyapunov energy, hysteresis sweep, F₂ basin readout |
| `cognitive_poly_julia_r/` | Native Julia and base-R ports of the cubic cell (no NumPy / third-party packages) |
| `LICENSE` | Full AGPLv3 text |

---

## 1. Analog + C reference

```sh
make          # builds generate, verify, associative
./verify      # deterministic ODE reference + loss/noise scenarios → results.csv
./associative # Hopfield capacity benchmark
```

### Robustness summary (`results.csv`)

| scenario   | removed % | alive | components | final energy | structural invariants |
|------------|-----------|-------|------------|--------------|-----------------------|
| clean      | 0         | 1024  | 1          | −51.17       | PASS                  |
| noisy      | 0         | 1024  | 1          | −51.17       | PASS                  |
| loss_5pct  | 5         | 973   | 1          | −48.62       | PASS                  |
| loss_10pct | 10        | 922   | 1          | −46.07       | PASS                  |
| loss_20pct | 20        | 820   | 1          | −40.97       | PASS                  |

---

## 2. Digital Verilog RTL — GF(2⁴) topological DAG

**File:** `verilog/gf_topological_dag.v`

```sh
cd verilog
iverilog -g2012 -o gf_dag_tb gf_topological_dag.v && vvp gf_dag_tb
```

---

## 3. Verilog-A continuous-time models

- `verilog-a/gf_topological_dag.va`
- `verilog-a/cognitive_poly_cell.va`
- `verilog-a/tb_cognitive_poly_cell.va` — hysteresis sweep past \(v_{\rm crit}\approx 0.3849\,\text{V}\), Lyapunov energy, soft F₂ readout, boundedness assert.

---

## 4. Native Julia & R ports

**Directory:** `cognitive_poly_julia_r/`

```sh
cd cognitive_poly_julia_r
julia --startup-file=no cognitive_poly.jl
Rscript --vanilla cognitive_poly.R
```

See the subdirectory README for equations, RK4 details, and reproducibility notes.

---

## License

GNU Affero General Public License v3.0. See [LICENSE](LICENSE).
