#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAP_ANON 0x1000

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

int main(int argc, char *argv[]){

    int n;
    
    sscanf(argv[2], "%d", &n); //num processes

    //established shared memory

    //original matrix
    int fd1 = shm_open("matrixA", O_RDWR | O_CREAT, 0777);
    ftruncate(fd1, 12*sizeof(float));
    float *A = (float *)mmap(NULL, 12*sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);

    //result matrix
    int fd2 = shm_open("matrixA", O_RDWR | O_CREAT, 0777);
    ftruncate(fd2, 12*sizeof(float));
    float *B = (float *)mmap(NULL, 12*sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);

    //piece of allocated memory to keep track of when processes are done so that they can synchronize
    int fd4 = shm_open("ready", O_RDWR | O_CREAT, 0777);
    ftruncate(fd4, n*sizeof(int));
    int *ready = (int*)mmap(NULL, n*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd4, 0);

    char processNum[3];

    char *programName = (char *)malloc(100);

    char *args[3];

    args[0] = (char *)malloc(100);
    args[1] = (char *)malloc(100);
    args[2] = NULL;

    strcpy(args[1], argv[2]);
    
    strcpy(programName, argv[1]);

    for(int i = 0; i < n; i++){
        if(!fork()){
            sprintf(processNum, "%d", i);
            strcpy(args[0], processNum);
            execv(programName, args);
            return 0;
        }
        sleep(1);
    }
    wait(0);

    close(fd1);
    shm_unlink("matrixA");
    munmap(A, 12*sizeof(float));

    close(fd2);
    shm_unlink("matrixA");
    munmap(B, 12*sizeof(float));

    close(fd4);
    shm_unlink("ready");
    munmap(ready, n*sizeof(int));

    free(programName);
    free(args[0]);
    free(args[1]);
    
    return 0;
}