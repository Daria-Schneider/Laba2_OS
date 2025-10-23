#ifndef GAUSS_H
#define GAUSS_H

#ifdef __cplusplus
extern "C" {
#endif

int gauss_solve(double *A, double *b, double *x, int n, int max_threads, int verbose);

#ifdef __cplusplus
}
#endif

#endif
