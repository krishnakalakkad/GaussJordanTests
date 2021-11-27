#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "gaussJordan.h"

void divideRow(float *A, int processNum, int i){
    /*int k = 0;
    while (k < numCols){
        float divisor = *(A + i + 4*i);
        if (!divisor)
            return;
        for (int j = 0; j < 4; j++)
            B[4 * i + j] =  A[4 * i + j] / divisor;
        k++;
        i++;
    }*/
    if (processNum != 0)
        return;
    float divisor = *(A + i + 4*i);
    if(!divisor)
        return;
    for (int j = 0; j < 4; j++)
        A[j + 4*i] = A[j + 4*i] / divisor;
}

/*void sweep(float *A, float *B, int pivot, int numLines, int processNum){
    int annihilationMultiple;
    int start = numLines * processNum;
    //this for loop sweeps the current column
    for (int  k = start; k < numLines; k++){
        if (k == pivot)
            continue;
        annihilationMultiple = -1 * A[pivot + 4 * k];
        for(int m = pivot; m < 4; m++){
            B[m + 4 * k] += A[m + 4 * pivot] * annihilationMultiple;
        }
    }
}*/

/*void sweep(float *B, int numLines, int processNum){
    int annihilationMultiple;
    for(int i = 0; i < 3; i++){
        int start = (i + processNum + 1) % 3;
        annihilationMultiple = -1 * B[i + 4 * start];
        for (int j = start; j < start + numLines; j++){
            for(int k = 0; k < 4; k++)
                B[k + (4 * j)] += annihilationMultiple * B[k + (4 * i)];
        }
    }
}*/

void sweep(float *A, int numLines, int processNum, int i){
    int annihilationMultiple;
    int start = (i + processNum + 1) % 3;
    annihilationMultiple = -1 * A[i + 4 * start];
    for (int j = 0; j < numLines; j++){
        for(int k = 0; k < 4; k++)
            A[k + (4 * start)] = A[k + (4 * start)] + annihilationMultiple * A[k + (4 * i)];
        start = (start + 1) % 3;
    }
}

/*void gaussJordanSolve(float *A, float *B, int numLines, int processNum){
    int j = 0;
    for (int i = 0; i < 3; i++){
        //Here, we take our pivot (make sure it's not zero) and divide the whole row
        if (!processNum)
            divideRow(A, B, numLines, i);
        sweep(B, numLines, processNum);
    }
}*/

void synch(int par_id,int par_count,int *ready){
    int synchid = ready[par_count]+1; 
    ready[par_id]=synchid;
    int breakout = 0;
    while(1){
        breakout=1;
        for(int i=0;i<par_count;i++){ 
            if(ready[i]<synchid){
                breakout = 0;
                break;
            }
        }
        if(breakout==1) {
            ready[par_count] = synchid;
            break;
        } 
    }
}

void print_matrix(float* m)//for 3*4
{
    printf("_______________________________________________________\n");
    for (int r = 0; r < 3; r++)
        {
        for (int c = 0; c < 4; c++)
            printf("%.1f  ", m[c + r * 4]);            
        printf("\n");
        }
    printf("_______________________________________________________\n");
}

int main(int argc, char **argv){

    int numLines, processNum, n;
    float dotProduct;
    sscanf(argv[0], "%d", &processNum);
    sscanf(argv[1], "%d", &n);

    int fd1 = shm_open("matrixA", O_RDWR | O_CREAT, 0777);
    if(!processNum)
        ftruncate(fd1, 12*sizeof(float));
    float *A = mmap(NULL, 12*sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);

    int fd2 = shm_open("matrixA", O_RDWR | O_CREAT, 0777);
    if(!processNum)
        ftruncate(fd2, 12*sizeof(float));
    float *B = mmap(NULL, 12*sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);

    int fd4 = shm_open("ready", O_RDWR | O_CREAT, 0777);
    if (!processNum)
        ftruncate(fd4, n*sizeof(int));
    int *ready = (int *)mmap(NULL, n*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd4, 0);

    if (!processNum)
        for(int i = 0; i < 12; i++){A[i] = i+1;}
    
    if(!processNum){
        print_matrix(A);
    }

    clock_t a = clock();
    //gaussJordanSolve(A, B, 3/n, processNum)
    for(int i = 0; i < 3; i++){
        divideRow(A, processNum, i);
        synch(processNum, n, ready);
        sweep(A, 2/n, processNum, i);
        synch(processNum, n, ready); 
        if(!processNum)
            print_matrix(A);  
    }
    clock_t b =  clock();

    if(!processNum){
        print_matrix(A);
        printf("time taken: %lf sec\n", (double)(b-a)/CLOCKS_PER_SEC);
    }

    close(fd1);
    shm_unlink("matrixA");
    munmap(A, 12*sizeof(float));
    
    close(fd2);
    shm_unlink("matrixA");
    munmap(B, 12*sizeof(float));

    close(fd4);
    shm_unlink("ready");
    munmap(ready, n*sizeof(int));

    return 0;
}