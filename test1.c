#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){


    int fd1 = shm_open("matrixA", O_RDWR | O_CREAT, 0777);
    ftruncate(fd1, 100*sizeof(int));
    int *p = (int *)mmap(NULL, 100*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
    
    printf("starting test1\n");

    p[0] = 0;

    int check = 0;

    check = atoi(argv[0]);
    printf("i have gotten this far\ncheck = %d\n", check);

    if(check){
        p[0] = 50; 
    }
    printf("%d\n", p[0]);
    

    return 0;

}
