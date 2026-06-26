iterate <- function(func, n, d) {
  x = rep(NA, d)
  index = 0
  recursiveIt <- function(rest, i) {
    if (i == d) {
      x[i] <<- rest
      index <<- index + 1
      func(x, index)
      return()
    }
    for (a in 0:rest) {
      x[i] <<- a
      recursiveIt(rest - a, i + 1)
    }
  }
  
  recursiveIt(n, 1)
}

klDivergence <- function(Q, P) {
  l = log(Q / P)
  l[Q == 0] <- 0
  sum(Q * l)
}

findCentralDist <- function(setQ) {
  samples_count = 100
  min_divergence = Inf
  center = c()
  iterate(function(P, i) {
    P = P / samples_count
    if (max(sapply(setQ, klDivergence, P = P)) < min_divergence) {
      center <<- P
      min_divergence <<- max(sapply(setQ, klDivergence, P = P)) 
    }
  }, d = 3, n = samples_count)
  return(center)
}

C_S1 = c(0 , 0.5, 0.5)
C_S2 = c(0.5 , 0, 0.5)
C_S3 = c(0.5 , 0.5, 0)

C_S1S2S3 = findCentralDist(list(C_S1, C_S2, C_S3))
C_S1S2 = findCentralDist(list(C_S1, C_S2)) 
C_S1S2_S3 = findCentralDist(list(C_S1S2, C_S3))

C_S1S2S3
C_S1S2
C_S1S2_S3

# divergence of center from members
sapply(list(C_S1S2, C_S3), klDivergence, C_S1S2_S3)