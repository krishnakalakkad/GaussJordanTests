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

void divideRow(float *A, int processNum, int i, int numRows){
    if (processNum != 0)
        return;
    float divisor = *(A + i + (numRows + 1)*i);
    if(!divisor)
        return;
    for (int j = 0; j < (numRows + 1); j++)
        A[j + (numRows + 1)*i] = A[j + (numRows + 1)*i] / divisor;
}

void sweep(float *A, int numLines, int processNum, int i, int numRows){
    float annihilationMultiple;
    int start = (i + processNum + 1) % numRows;
    for (int j = 0; j < numLines; j++){
        annihilationMultiple = -1 * A[i + (numRows + 1)* start];
        for(int k = 0; k < (numRows + 1); k++)
        A[k + ((numRows + 1) * start)] = A[k + ((numRows + 1) * start)] + annihilationMultiple * A[k + ((numRows + 1) * i)];
        start = (start + 1) % numRows;
    }
}

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

void print_matrix(float* m, int numRows){
    printf("_______________________________________________________\n");
    for (int r = 0; r < numRows; r++)
        {
        for (int c = 0; c < (numRows + 1); c++)
            printf("%.1f  ", m[c + r * (numRows + 1)]);            
        printf("\n");
        }
    printf("_______________________________________________________\n");
}

int main(int argc, char **argv){

    int numLines, processNum, multiSwitch, numRows, n = 1;
    float dotProduct;
    sscanf(argv[0], "%d", &processNum);
    sscanf(argv[1], "%d", &multiSwitch);
    sscanf(argv[2], "%d", &numRows);

    int matrixSize = numRows*(numRows+1);

    if(multiSwitch)
        n = numRows - 1;

    int fd1 = shm_open("matrixA", O_RDWR | O_CREAT, 0777);
    if(!processNum)
        ftruncate(fd1, matrixSize*sizeof(float));
    float *A = mmap(NULL, matrixSize*sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);

    int fd4 = shm_open("ready", O_RDWR | O_CREAT, 0777);
    if (!processNum)
        ftruncate(fd4, n*sizeof(int));
    int *ready = (int *)mmap(NULL, n*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd4, 0);

    srand(time(0)); 

    if (!processNum)
        for(int i = 0; i < matrixSize; i++){
            float x = (float)rand()/(float)(RAND_MAX/10.0);
            A[i] = x;
        }

    double avgTime;

    if (!processNum){
        print_matrix(A, numRows);
    }

    for (int j = 0; j < 20; j++){
        clock_t a = clock();
        for(int i = 0; i < numRows; i++){
            divideRow(A, processNum, i, numRows);
            synch(processNum, n, ready);
            sweep(A, (numRows-1)/n, processNum, i, numRows);
            synch(processNum, n, ready); 
        }
        clock_t b =  clock();
        if(!processNum){
            avgTime += (double)(b-a)/CLOCKS_PER_SEC;
            for(int i = 0; i < matrixSize; i++){
                float x = (float)rand()/(float)(RAND_MAX/10.0);
                A[i] = x;
            }
        }
    }

    if(!processNum){
        avgTime = avgTime / 20;
        printf("avg time taken: %lf sec\n", avgTime);
        
    }

    close(fd1);
    shm_unlink("matrixA");
    munmap(A, matrixSize*sizeof(float));

    close(fd4);
    shm_unlink("ready");
    munmap(ready, n*sizeof(int));

    return 0;
}