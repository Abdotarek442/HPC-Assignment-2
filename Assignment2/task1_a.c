#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool isPrime(int x)
{
    if (x < 2)
        return false;
    for (int i = 2; i * i <= x; i++)
    {
        if (x % i == 0)
            return false;
    }
    return true;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int rank, size;
    int x, y, r;
    double timeStart, timeEnd;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0)
    {
        printf("Enter lower bound: \n");
        scanf("%d", &x);
        printf("Enter upper bound: \n");
        scanf("%d", &y);
        if (y < x)
        {
            printf("upper bound should be greater than or equal lower bound\n");
            MPI_Finalize();
            return 1;
        }
        timeStart = MPI_Wtime();
        r = (y - x) / (size - 1);
    }
    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&r, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int localCount = 0;
    int start, end;
    if (rank != 0)
    {
        start = x + ((rank - 1) * r);
        end = x + (rank * r);
    }
    else
    {
        start = x + ((size - 1) * r);
        end = y + 1;
    }
    for (int i = start; i < end; i++)
    {
        localCount += (isPrime(i)) ? 1 : 0;
    }
    if (localCount != 0)
        printf("p%d: calculate partial count of prime numbers from %d to %d  Count = %d\n", rank, start, end - 1, localCount);
    int globalCount = 0;
    MPI_Reduce(&localCount, &globalCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        timeEnd = MPI_Wtime();
        printf("After reduction, P0 will have Count = %d\n", globalCount);
        printf("\n");
        printf("Total time: %f\n", timeEnd - timeStart);
        printf("Total time in ms: %f\n", (timeEnd - timeStart) * 1000);
    }
    MPI_Finalize();
}
