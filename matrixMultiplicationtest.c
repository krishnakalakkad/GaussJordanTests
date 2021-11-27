#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void print_matrix(float* m){
    printf("_______________________________________________________\n");
    for (int r = 0; r < 3; r++)
        {
        for (int c = 0; c < 3; c++)
            printf("%.1f  ", m[c + r * 3]);            
        printf("\n");
        }
    printf("_______________________________________________________\n");
}

void multiplyMatrices(float *A, float *B, float *C){
    float dotProduct;
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++){
            dotProduct = 0;
            for (int k = 0; k < 3; k++)
                dotProduct += A[3*i + k] * B[j + 3*k];
            C[3 * i + j] = dotProduct; 
        }
}

int main(){

    float *A = (float *)malloc(9*sizeof(float));
    float *B = (float *)malloc(9*sizeof(float));
    float *C = (float *)malloc(9*sizeof(float));

    srand(time(0));

    float x;
    for(int i = 0; i<9; i++){
        x = (float)rand()/((float)RAND_MAX/10);
        A[i] = x;
    }

    for(int i = 0; i<9; i++)
        B[i] = 0.0;

    B[0] = B[4] = B[8] = 1.0;

    print_matrix(A);
    multiplyMatrices(A,B,C);
    print_matrix(C);

    free(A);
    free(B);
    free(C);

    return 0;

}
