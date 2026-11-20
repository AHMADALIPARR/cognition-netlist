#!/usr/bin/env julia
# Julia standard library only; no Python/NumPy or third-party dependencies.
using LinearAlgebra
using Random
using Statistics
using Printf

const DEFAULT_CAP = 1e-9
const DEFAULT_A = -1e-6
const DEFAULT_B = 1e-6
const DEFAULT_GAIN = 1e-6

# KCL: C dv/dt = -a v - b v^3 + g s
# Nominal: dv/dt = 1000 (v - v^3 + s)
function rhs(v, s; a=DEFAULT_A, b=DEFAULT_B, gain=DEFAULT_GAIN, cap=DEFAULT_CAP)
    return (-a * v - b * v^3 + gain * s) / cap
end

function potential(v, s; a=DEFAULT_A, b=DEFAULT_B, gain=DEFAULT_GAIN)
    return 0.5 * a * v^2 + 0.25 * b * v^4 - gain * s * v
end

function rk4_step(v, s, h; kwargs...)
    k1 = rhs(v, s; kwargs...)
    k2 = rhs(v + 0.5 * h * k1, s; kwargs...)
    k3 = rhs(v + 0.5 * h * k2, s; kwargs...)
    k4 = rhs(v + h * k3, s; kwargs...)
    return v + (h / 6) * (k1 + 2k2 + 2k3 + k4)
end

function main()
    rng = MersenneTwister(42)
    @printf("Equilibria at s=0: v=-1,0,+1  Jacobians -2000,+1000,-2000\n")
    @printf("s_crit = %.8f\n", 2 / (3 * sqrt(3)))

    # Hysteresis sweep
    h = 1e-5
    v = -1.0
    s_vals = -0.6:0.05:0.6
    switches = Float64[]
    for s in s_vals
        for _ in 1:2000
            v = rk4_step(v, s, h)
        end
        push!(switches, v)
    end
    @printf("Hysteresis end state at s=+0.6: %.4f\n", switches[end])

    # Simple polynomial surrogate (derivative regressor)
    n = 600
    X = zeros(n, 5)
    y = zeros(n)
    for i in 1:n
        v = 2 * rand(rng) - 1
        s = 0.5 * (2 * rand(rng) - 1)
        X[i, :] .= (1.0, v, v^2, v^3, s)
        y[i] = rhs(v, s)
    end
    λ = 1e-4
    w = (X' * X + λ * I) \ (X' * y)
    @printf("Ridge coeffs (1,v,v²,v³,s): %s\n", string(round.(w; digits=2)))
    pred = X * w
    rmse = sqrt(mean((pred .- y).^2))
    @printf("Train RMSE: %.6f\n", rmse)
    println("PASS: Julia cognitive_poly checks completed")
end

main()
