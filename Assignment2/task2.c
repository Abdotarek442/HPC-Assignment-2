#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int rows, cols;
    MPI_Init(&argc, &argv);
    int rank, size, localSize;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0)
    {
        printf("Please enter number of rows: \n");
        scanf("%d", &rows);
        printf("Please enter number of cols: \n");
        scanf("%d", &cols);
        if (rows % size != 0)
        {
            printf("Error: number of rows should be divisible by number of processes.\n");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        localSize = rows * cols / size;
    }
    MPI_Bcast(&localSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int **A = NULL, **B = NULL;
    int *flattedA = NULL, *flattedB = NULL, *flattedC = NULL;
    int *localA = malloc(localSize * sizeof(int));
    int *localB = malloc(localSize * sizeof(int));
    int *localC = malloc(localSize * sizeof(int));
    if (rank == 0)
    {
        A = malloc(rows * sizeof(int *));
        B = malloc(rows * sizeof(int *));
        for (int i = 0; i < rows; i++)
        {
            A[i] = malloc(cols * sizeof(int));
            B[i] = malloc(cols * sizeof(int));
        }
        printf("Enter matrix A elements (%d x %d):\n", rows, cols);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                scanf("%d", &A[i][j]);
        printf("Enter matrix B elements (%d x %d):\n", rows, cols);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                scanf("%d", &B[i][j]);
        flattedA = malloc(rows * cols * sizeof(int));
        flattedB = malloc(rows * cols * sizeof(int));
        flattedC = malloc(rows * cols * sizeof(int));
        int index = 0;
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                flattedA[index++] = A[i][j];
        index = 0;
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                flattedB[index++] = B[i][j];
    }
    MPI_Scatter(flattedA, localSize, MPI_INT, localA, localSize, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(flattedB, localSize, MPI_INT, localB, localSize, MPI_INT, 0, MPI_COMM_WORLD);
    for (int i = 0; i < localSize; i++)
    {
        localC[i] = localA[i] + localB[i];
    }
    MPI_Gather(localC, localSize, MPI_INT, flattedC, localSize, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        printf("Result matrix C (A + B):\n");
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                printf("%d ", flattedC[i * cols + j]);
            }
            printf("\n");
        }
        for (int i = 0; i < rows; i++)
        {
            free(A[i]);
            free(B[i]);
        }
        free(A);
        free(B);
        free(flattedA);
        free(flattedB);
        free(flattedC);
    }
    free(localA);
    free(localB);
    free(localC);
    MPI_Finalize();
    return 0;
}