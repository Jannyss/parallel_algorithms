#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include "mpi.h"
#define Size 500

MPI_Status status;

double a[Size][Size],b[Size][Size],c[Size][Size];

MPI_Datatype coltype;

void printResultMatrix() {
    int i,j;

    for (i=0; i<Size; i++) {
        for (j=0; j<Size; j++) {
            printf("%6.2f   ", c[i][j]);
            printf(" ");
        }
        printf("\n");
    }
}


void multiplicateMatrices(int rows) {
  int i,j,k;
  for (k=0; k<Size; k++)
      for (i=0; i<rows; i++) {
        c[i][k] = 0.0;
        for (j=0; j<Size; j++)
          c[i][k] = c[i][k] + a[i][j] * b[j][k];
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

void multiplicateMatricesSerial() {
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


int main(int argc, char **argv)
{
  int numtasks,taskid,numworkers,source,dest,rows,offset,i,j,k;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

  numworkers = numtasks-1;

  if (taskid == 0) {

    generateMatrices();

    std::clock_t start = std::clock();

    rows = Size/numworkers;
    offset = 0;

    for (dest=1; dest<=numworkers; dest++)
    {
      MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);

      MPI_Type_vector(rows,1,Size,MPI_DOUBLE,&coltype);
      MPI_Type_commit(&coltype);

      MPI_Send(&a[offset][0], 1, coltype, dest,1, MPI_COMM_WORLD);

      MPI_Type_vector(Size,1,Size,MPI_DOUBLE,&coltype);
      MPI_Type_commit(&coltype);

      MPI_Send(&b, 1, coltype, dest, 1, MPI_COMM_WORLD);
      offset = offset + rows;
    }

    for (i=1; i<=numworkers; i++)
    {
      source = i;
      MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);

      MPI_Type_vector(rows,1,Size,MPI_DOUBLE,&coltype);
      MPI_Type_commit(&coltype);

      MPI_Recv(&c[offset][0], 1, coltype, source, 2, MPI_COMM_WORLD, &status);
    }

    std::clock_t end = std::clock();
    printf("Time, ms: ");
    printf("%6.6f", 1000.0 * (end - start) / CLOCKS_PER_SEC);
    printf("\n");

    // printResultMatrix();

  }

  if (taskid > 0) {
    source = 0;
    MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);

    MPI_Type_vector(rows,1,Size,MPI_DOUBLE,&coltype);
    MPI_Type_commit(&coltype);

    MPI_Recv(&a, 1, coltype, source, 1, MPI_COMM_WORLD, &status);

    MPI_Type_vector(Size,1,Size,MPI_DOUBLE,&coltype);
    MPI_Type_commit(&coltype);

    MPI_Recv(&b, 1, coltype, source, 1, MPI_COMM_WORLD, &status);

    multiplicateMatrices(rows);

    MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    
    MPI_Type_vector(rows,1,Size,MPI_DOUBLE,&coltype);
    MPI_Type_commit(&coltype);

    MPI_Send(&c, 1, coltype, 0, 2, MPI_COMM_WORLD);
  }

  MPI_Finalize();

}