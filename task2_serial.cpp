#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <ctime>

using namespace std;

#define Size 500


double a[Size][Size],b[Size][Size],c[Size][Size];


void multiplicateMatrices() {
    int i,j,k;

    for (i=0; i<Size; i++) {
        for (j=0; j<Size; j++){
            c[i][j] = 0;
            for (k=0; k<Size; k++){
                c[i][j] += a[i][k]*b[k][j];
            }
        }
    }
}

void generateMatrices() {
  int i,j;

    for (i=0; i<Size; i++) {
        for (j=0; j<Size; j++) {
            a[i][j] = rand();
        }
    }

    for (i=0; i<Size; i++) {
        for (j=0; j<Size; j++) {
            b[i][j] = rand();
        }
    }

}

void printMatrix(double C[Size][Size]) {
    int i,j;

    for (i=0; i<Size; i++) {
        for (j=0; j<Size; j++) {
            cout << c[i][j] << " ";
        }
    cout << endl;
    }
}


int main() {

    generateMatrices();

    std::clock_t start = std::clock();
    multiplicateMatrices();
    std::clock_t end = std::clock();
    printf("Time, ms: ");
    printf("%6.6f", 1000.0 * (end - start) / CLOCKS_PER_SEC);
    printf("\n");
    
    // printMatrix(C);
}