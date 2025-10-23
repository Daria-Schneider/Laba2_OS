#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <windows.h>
#include "gauss.h"

double now_seconds(){
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart / (double)f.QuadPart;
}

static void usage(const char *p){
    printf("Usage: %s [-n size] [-t max_threads] [-r seed] [-f filename]\n", p);
    printf("Example: %s -n 500 -t 8 -r 123\n", p);
    printf("(Project: Laba2_OS — Parallel Gaussian Elimination)\n");
}

int main(int argc, char **argv){
    int n = 200;
    int max_threads = 4;
    int seed = -1;
    const char *fname = NULL;

    for(int i=1;i<argc;i++){
        if(strcmp(argv[i],"-n")==0 && i+1<argc) n = atoi(argv[++i]);
        else if(strcmp(argv[i],"-t")==0 && i+1<argc) max_threads = atoi(argv[++i]);
        else if(strcmp(argv[i],"-r")==0 && i+1<argc) seed = atoi(argv[++i]);
        else if(strcmp(argv[i],"-f")==0 && i+1<argc) fname = argv[++i];
        else { usage(argv[0]); return 1; }
    }

    double *A = (double*)malloc(sizeof(double)*n*n);
    double *b = (double*)malloc(sizeof(double)*n);
    double *x = (double*)calloc(n, sizeof(double));
    if(!A || !b || !x){ fprintf(stderr,"alloc fail\n"); return 2; }

    if(fname){
        FILE *f = fopen(fname,"r");
        if(!f){ perror("fopen"); return 3; }
        int rn;
        if(fscanf(f,"%d",&rn)!=1){ fprintf(stderr,"bad file\n"); return 4; }
        if(rn != n){ fprintf(stderr,"file n != specified n; using file n\n"); n = rn; }
        for(int i=0;i<n*n;i++) fscanf(f,"%lf",&A[i]);
        for(int i=0;i<n;i++) fscanf(f,"%lf",&b[i]);
        fclose(f);
    } else {
        if(seed>=0) srand(seed); else srand((unsigned)time(NULL));
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                double v = ((double)rand()/RAND_MAX)*2.0 - 1.0;
                A[i*n + j] = v;
            }
        }
        for(int i=0;i<n;i++){
            double s = 0;
            for(int j=0;j<n;j++) s += fabs(A[i*n + j]);
            A[i*n + i] += s + 1.0;
            b[i] = ((double)rand()/RAND_MAX)*10.0;
        }
    }

    printf("Matrix size: %d x %d, max_threads=%d\n", n, n, max_threads);
    printf("Program will print active thread counts during elimination.\n");

    double t0 = now_seconds();
    int rc = gauss_solve(A,b,x,n,max_threads,1);
    double t1 = now_seconds();

    if(rc!=0){
        fprintf(stderr,"gauss failed (maybe singular)\n");
        return 5;
    }

    printf("Solved in %.6f seconds\n", t1-t0);
    int toprint = (n<10)?n:10;
    printf("x[0..%d]= ", toprint-1);
    for(int i=0;i<toprint;i++) printf("%.6g ", x[i]);
    printf("\n");

    free(A); free(b); free(x);
    return 0;
}