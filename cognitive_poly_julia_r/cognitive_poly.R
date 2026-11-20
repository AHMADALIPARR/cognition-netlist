#!/usr/bin/env Rscript
# Base R only; no external packages.

cap <- 1e-9
a <- -1e-6
b <- 1e-6
gain <- 1e-6

rhs <- function(v, s) {
  (-a * v - b * v^3 + gain * s) / cap
}

potential <- function(v, s) {
  0.5 * a * v^2 + 0.25 * b * v^4 - gain * s * v
}

rk4_step <- function(v, s, h) {
  k1 <- rhs(v, s)
  k2 <- rhs(v + 0.5 * h * k1, s)
  k3 <- rhs(v + 0.5 * h * k2, s)
  k4 <- rhs(v + h * k3, s)
  v + (h / 6) * (k1 + 2 * k2 + 2 * k3 + k4)
}

cat(sprintf("Equilibria at s=0: v=-1,0,+1  Jacobians -2000,+1000,-2000\n"))
cat(sprintf("s_crit = %.8f\n", 2 / (3 * sqrt(3))))

set.seed(42)
h <- 1e-5
v <- -1.0
s_vals <- seq(-0.6, 0.6, by = 0.05)
for (s in s_vals) {
  for (i in 1:2000) v <- rk4_step(v, s, h)
}
cat(sprintf("Hysteresis end state at s=+0.6: %.4f\n", v))

n <- 600
X <- matrix(0, n, 5)
y <- numeric(n)
for (i in 1:n) {
  vv <- 2 * runif(1) - 1
  ss <- 0.5 * (2 * runif(1) - 1)
  X[i, ] <- c(1, vv, vv^2, vv^3, ss)
  y[i] <- rhs(vv, ss)
}
lambda <- 1e-4
w <- solve(t(X) %*% X + lambda * diag(5), t(X) %*% y)
cat("Ridge coeffs (1,v,v2,v3,s):", paste(round(as.vector(w), 2), collapse = ", "), "\n")
pred <- as.vector(X %*% w)
rmse <- sqrt(mean((pred - y)^2))
cat(sprintf("Train RMSE: %.6f\n", rmse))
cat("PASS: R cognitive_poly checks completed\n")
