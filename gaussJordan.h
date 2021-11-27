#ifndef GAUSSJORDANH
#define GAUSSJORDANH


void print_matrix(float* m);

void synch(int par_id,int par_count,int *ready);

void gaussJordanSolve(float *A, float *B, int numLines, int processNum);

void sweep(float *A, int numLines, int processNum, int i);

void divideRow(float *A, int processNum, int i);

#endif