#ifndef GAUSSJORDANH
#define GAUSSJORDANH

void print_matrix(float* m, int numRows);

void synch(int par_id,int par_count,int *ready);

void sweep(float *A, int numLines, int processNum, int i, int numRows);

void divideRow(float *A, int processNum, int i, int numRows);

#endif