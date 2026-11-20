# 1,024-neuron analog polynomial state network

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)

This is a **simulation artifact**, not a biological consciousness model. Every complex state has two circuit nodes, `x{i}` and `y{i}`, with a polynomial nonlinear current source and capacitor at each node. The graph has 1024 vertices, degree 8, and 4096 undirected edges (8192 real-channel resistors). The provided graph was generated as a seeded random regular graph, NOT by a provably Ramanujan construction. Its sampled sparse-matrix eigenvalues satisfy the numerical spectral Ramanujan criterion in double precision: maximal nontrivial magnitude approximately **5.26332**, compared with **2√7 ≈ 5.29150**. This is an empirical spectral check, not an exact mathematical proof or validated biological property.

## License

This project is licensed under the **GNU Affero General Public License v3.0** (AGPL-3.0). See the [LICENSE](LICENSE) file for the full text.

## Contents

| File | Description |
|------|-------------|
| `generate.c` | C11 generator that reads `edges.txt` and emits a SPICE netlist |
| `edges.txt` | 4096 undirected edges of an 8-regular graph on 1024 vertices |
| `neurons_1024.cir` | Pre-generated SPICE netlist (1024 complex neurons) |
| `LICENSE` | GNU Affero General Public License v3.0 |

## Build and inspect

```sh
cc -std=c11 -O2 -Wall -Wextra -pedantic generate.c -o generate
./generate edges.txt neurons_1024.cir
ngspice -b neurons_1024.cir -r neurons.raw
```

The raw simulation command needs an installed ngspice. The package was generated and syntax inspected, but a SPICE transient run was **not** verified here.

## Dynamics

Each `z_i = x_i + i*y_i` follows the SPICE transient-time relaxation

```
C dz_i/dt = -(alpha + beta |z_i|²) z_i - gc * (Lz)_i + external_i.
```

The resistor network realizes the graph Laplacian over real-valued conductances (NOT arithmetic over GF(2)). Coefficients are illustrative dimensionless-scaled simulation parameters. Initial conditions and a weak reference phase deliberately break symmetry. This basic model is **gradient relaxation only**: oscillatory/Hamiltonian evolution, biological feedback, and synaptic plasticity are not implemented. Real hardware would require unit calibration and device models.

## Where the proposed mathematical layers belong

- **GF(2)**: binary adjacency, incidence/parity checks, optional code-based retrieval constraints. It is NOT the analog state arithmetic.
- **Ramanujan/expander spectral analysis**: certify or benchmark connectivity and robustness; a random-regular graph meeting a numerical eigenvalue test is not a constructed/proven Ramanujan family.
- **Black-hole information theory**: entropy and quantum-code-inspired analogies can motivate information preservation experiments; this netlist has no horizon, gravity, or holographic realization.
- **M-theory**: speculative research analogy; this simulation does not implement an M-theory model.

The GF(2) incidence matrix associated with `edges.txt` has exactly two 1s per column, hence every column has even parity. A meaningful next experiment is applying explicit parity-check codes to discretized attractor labels and testing noisy retrieval.
