# Cognitive polynomial cell — native Julia and base R

Faithful translation of the supplied scalar Verilog-A KCL model and numerical experiments, **without NumPy, Python runtime, or third-party Julia/R packages**.

Run:

```sh
julia --startup-file=no cognitive_poly.jl
Rscript --vanilla cognitive_poly.R
```

Neither interpreter was installed in the artifact-building environment, so interpreter results cannot be certified here. The implementations are source-level translations with assertions; users should run both commands in their respective environments.

## Raw equations

- KCL: `C dv/dt = -a v - b v^3 + g s`
- Nominal derivative: `dv/dt = 1000 (v - v^3 + s)` (units: V/s)
- Potential: `U(v;s) = a v^2/2 + b v^4/4 - g s v`, and `C dv/dt = -dU/dv`
- Equilibria for `s = 0`: `v = -1, 0, +1`, Jacobians `-2000, +1000, -2000` s^-1 respectively
- Saddle-node stimulus: `|s_crit| = 2/(3 sqrt(3)) ≈ 0.38490018` V
- Standard RK4: `v(t+h) = v(t) + h(k1+2k2+2k3+k4)/6`
- Polynomial surrogate: `X = [1, v, v², v³, s]`, `w = (XᵀX + λI)⁻¹ Xᵀ y`, `λ = 10^-4`.

## Reproducibility / interpretation

- Julia uses `MersenneTwister(42)`; R uses `set.seed(42)`. They do NOT produce identical samples because their RNG/distribution implementations differ.
- Both train on the first 450 samples and evaluate on the last 150. Normalizing RMSE uses **population** standard deviation to match NumPy.
- Original description claims to predict a steady-state branch, but the original features/targets train a derivative regressor only. These ports do not claim branch prediction.
- Negative cubic coefficient is a **necessary sign check for dominant cubic restoring force in a 1D polynomial surrogate**, not a full proof of global boundedness for an entire coupled network or under arbitrary forcing.
- The hysteresis switch estimates are grid-dependent (`Δs = 0.05`) and depend on 20 ms settling; threshold values are not exact measurement of saddle-node location.
- Julia and R are both native mathematical reimplementations, not interfaces to NumPy.
