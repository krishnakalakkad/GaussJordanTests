#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h>



int main(){

    
    //create shared named mem
    int fd1 = shm_open("matrixA", O_RDWR | O_CREAT, 0777);
    ftruncate(fd1, 100*sizeof(int));
    int *p = (int *)mmap(NULL, 100*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);

    
    char arg1[3];

    sprintf(arg1, "%d", 1);

    char *args[2];
    args[0] = (char *)malloc(100);
    args[1] = NULL;

    strcpy(args[0], "1");


    if (!fork())
        execv("./test1", args);
    wait(0);
    printf("hello!\n");

    close(fd1);
    shm_unlink("matrixA");
    munmap(p, 1024*sizeof(float));
    

    return 0;
}




