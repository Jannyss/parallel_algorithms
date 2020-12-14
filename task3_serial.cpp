#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

#define M 5
#define N 3

float A[M][N],D[M][M];

void findDistances() {
    int i,j,k;

    for (i=0; i<M; i++) {
        for (j=0; j<M; j++){
            for (k=0; k<N; k++){
                D[i][j] += (A[i][k]-A[j][k])*(A[i][k]-A[j][k]);
            }
        }
    }
}

void generateMatrices() {
    int i,j;

    for (i=0; i<M; i++) {
        for (j=0; j<N; j++) {
            A[i][j] = rand();
        }
    }

    for (i=0; i<M; i++) {
        for (j=0; j<M; j++) {
            D[i][j] = 0;
        }
    }

}

void printDistanceMatrix() {
    int i,j;

    for (i=0; i<M; i++) {
        for (j=0; j<M; j++) {
            cout << D[i][j] << " ";
        }
        cout << endl;
    }
}


int main() {

    generateMatrices();

    std::clock_t start = std::clock();
    findDistances();
    std::clock_t end = std::clock();

    printf("Time, ms: ");
    printf("%6.6f", 1000.0 * (end - start) / CLOCKS_PER_SEC);
    printf("\n");

    printDistanceMatrix();
}