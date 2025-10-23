#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include "gauss.h"
#include "threads_lim.h"

typedef struct {
    double *A, *b;
    int n, k, i;
} RowTask;

static DWORD WINAPI row_eliminate(void *arg) {
    RowTask *t = (RowTask*)arg;
    int n = t->n, k = t->k, i = t->i;
    double *A = t->A, *b = t->b;

    double factor = A[i*n + k] / A[k*n + k];
    for (int j = k; j < n; j++)
        A[i*n + j] -= factor * A[k*n + j];
    b[i] -= factor * b[k];

    free(t);
    threads_lim_release();
    return 0;
}

int gauss_solve(double *A, double *b, double *x, int n, int max_threads, int verbose) {
    threads_lim_init(max_threads);

    for (int k = 0; k < n-1; k++) {
        int pivot = k;
        double maxv = fabs(A[k*n + k]);
        for (int i = k+1; i < n; i++) {
            if (fabs(A[i*n + k]) > maxv) {
                maxv = fabs(A[i*n + k]);
                pivot = i;
            }
        }
        if (pivot != k) {
            for (int j = 0; j < n; j++) {
                double tmp = A[k*n + j];
                A[k*n + j] = A[pivot*n + j];
                A[pivot*n + j] = tmp;
            }
            double tmpb = b[k]; b[k] = b[pivot]; b[pivot] = tmpb;
        }

        if (fabs(A[k*n + k]) < 1e-12) {
            threads_lim_destroy();
            return 1;
        }

        int rows = n - (k+1);
        HANDLE *th = malloc(sizeof(HANDLE) * rows);
        if (!th) {
            threads_lim_destroy();
            return 1;
        }

        for (int i = k+1; i < n; i++) {
            threads_lim_acquire();
            RowTask *task = malloc(sizeof(RowTask));
            if (!task) {
                threads_lim_release();
                free(th);
                threads_lim_destroy();
                return 1;
            }
            task->A = A; task->b = b;
            task->n = n; task->k = k; task->i = i;
            
            th[i - (k+1)] = CreateThread(NULL, 0, row_eliminate, task, 0, NULL);
            if (!th[i - (k+1)]) {
                free(task);
                threads_lim_release();
                for (int j = k+1; j < i; j++) {
                    WaitForSingleObject(th[j - (k+1)], INFINITE);
                    CloseHandle(th[j - (k+1)]);
                }
                free(th);
                threads_lim_destroy();
                return 1;
            }
        }

        WaitForMultipleObjects(rows, th, TRUE, INFINITE);
        
        for (int i = 0; i < rows; i++) {
            CloseHandle(th[i]);
        }
        free(th);

        if (verbose && (k % 10 == 0 || k == n-2))
            printf("[step %d/%d] active threads limited to %d\n", k, n, max_threads);
    }

    for (int i = n-1; i >= 0; i--) {
        double sum = b[i];
        for (int j = i+1; j < n; j++)
            sum -= A[i*n + j] * x[j];
        x[i] = sum / A[i*n + i];
    }

    threads_lim_destroy();
    return 0;
}