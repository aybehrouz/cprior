library(MASS)

getPolynomialBasis <- function(k) {
  sapply(0:k, function(i) function(x) x ^ i)
}

makeVectorFunction <- function(func_list) {
  function(x) {
    sapply(func_list, function(f) f(x))
  }
}

Q <- function(k, theta, n) {
  choose(n, k) * theta ^ k * (1 - theta) ^ (n - k)
}

H <- function(theta, n) {
  -n * (theta * log(theta) + (1 - theta) * log(1 - theta))
}

errorBound <- function(n, m) {
  basis <- makeVectorFunction(getPolynomialBasis(m))
  
  Y = sapply(theta_samples, H, n = n)
  X = t(sapply(theta_samples, basis))
  A = ginv(X) %*% Y
  max(X %*% A - Y) - min(X %*% A - Y)
}

posterior <- function(k, n, m) {
  basis <- makeVectorFunction(getPolynomialBasis(m))
  
  Y = sapply(theta_samples, H, n = n)
  X = t(sapply(theta_samples, basis))
  A = ginv(X) %*% Y
  
  p0 = exp(-basis(k / n) %*% A)
  p1 = exp(-basis((k + 1) / n) %*% A)
  p1 / (p0 + p1)
}

theta_samples = seq(0.001, 0.999, length.out = 30)

m = 3:13
k = 2
n = 7

f_delta = sapply(m, errorBound, n = n)
plot(m , f_delta)

post_err = abs(sapply(m, posterior, n = n, k = k) - (k + 0.5) / n) 
plot(m, post_err)
           
(k + 1) / (n + 1) - (k + 0.5) / n

for (i in 1:length(m)) {
  cat(sprintf("(%d, %f) ", m[i] + 1, post_err[i]))
}

for (i in 1:length(m)) {
  cat(sprintf("(%d, %f) ", m[i] + 1, f_delta[i]))
}
