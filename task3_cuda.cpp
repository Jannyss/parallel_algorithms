#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#define BLOCK_SIZE 16
#define M 8192
#define N 4096


__global__ void objSimilarityGlobalMem (float * a, float * d, int n) {

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    float sum = 0.0f;
    int ia = n * BLOCK_SIZE * by + n * ty;
    int ib = n * BLOCK_SIZE * bx + n * tx;
    int ic = n * BLOCK_SIZE * by + n * ty + BLOCK_SIZE * bx + tx;
    for (int k = 0; k < n; k++)
        sum += pow((a[ia + k] - a[ib + k]), 2);
    d[ic] = sum;
}

__global__ void objSimilaritySharedMem (float * a, float * d, int n) {
    int bx = blockIdx.x, by = blockIdx.y;
    int tx = threadIdx.x, ty = threadIdx.y;
    int aBegin = n * BLOCK_SIZE * by;
    int aEnd = aBegin + n - 1;
    int bBegin = BLOCK_SIZE * bx;
    int aStep = BLOCK_SIZE, bStep = BLOCK_SIZE * n;
    float sum = 0.0f;

    __shared__ float as [BLOCK_SIZE][BLOCK_SIZE+1];
    __shared__ float bs [BLOCK_SIZE][BLOCK_SIZE+1];

    for ( int ia = aBegin, ib = bBegin; ia <= aEnd; ia += aStep, ib += bStep ){
        as [ty][tx] = a [ia + n * ty + tx];
        bs [ty][tx] = a [ib + n * ty + tx];
        __syncthreads ();
        for ( int k = 0; k < BLOCK_SIZE; k++ )
            sum += pow((as[ty][k] - bs[k][tx]),2);
        __syncthreads ();
    }
    d [aBegin + bBegin + n * ty + tx] = sum;
}


int main() {

    int numBytes_MN = M * N * sizeof(float);
    int numBytes_MM = M * M * sizeof(float);

    float * h_A = (float*)malloc(M * N * sizeof(float));
    float * h_D = (float*)malloc(M * M * sizeof(float));

    float * d_A;
    float * d_D;

    cudaMalloc((void**)&d_A, numBytes_MN);
    cudaMalloc((void**)&d_D, numBytes_MM);

    dim3 threads(BLOCK_SIZE, BLOCK_SIZE);
    dim3 blocks(M / BLOCK_SIZE, N / BLOCK_SIZE);

    printf("1");
    int i, j;
    for(i=0; i<M; i++) {
        for(j=0; j<N; j++) {
            h_A[i*M+j] = rand(); }}

    for(i=0; i<M; i++) {
        for(j=0; j<M; j++) {
            h_D[i*M+j] = 0.0f;}}

    cudaEvent_t startGpu, stopGpu;
    cudaEventCreate(&startGpu);
    cudaEventCreate(&stopGpu);
    float gpuTime;
    cudaEventRecord(startGpu, 0);

    cudaMemcpy(d_A, &h_A, numBytes_MN, cudaMemcpyHostToDevice);
    cudaMemcpy(d_D, &h_D, numBytes_MM, cudaMemcpyHostToDevice);

    objSimilarityGlobalMem<<<blocks,threads>>> (d_A, d_D, M);
    cudaMemcpy(h_D, d_D, numBytes_MM, cudaMemcpyDeviceToHost);

    cudaEventRecord(stopGpu, 0);
    cudaEventSynchronize(stopGpu);
    cudaEventElapsedTime(&gpuTime, startGpu, stopGpu);
    printf("GPU Time, ms: %6.6f\n", gpuTime);
    cudaEventDestroy(startGpu);
    cudaEventDestroy(stopGpu);

    cudaFree(d_A);
    cudaFree(d_D);

    free(h_A);
    free(h_D);

    return 0;
}
