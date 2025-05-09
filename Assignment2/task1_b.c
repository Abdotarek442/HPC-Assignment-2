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
    int totalCount = 0;
    double timeStart, timeEnd;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size < 2)
    {
        printf("The number of processes should be greater than 1.\n");
        MPI_Finalize();
        return 1;
    }
    if (rank == 0)
    {
        int x = 0, y = 0;
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
        int r = (y - x) / (size - 1);
        for (int i = 1; i < size; i++)
        {
            MPI_Send(&x, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&r, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        }
        int primeNumber = 0;
        for (int i = x + (r * (size - 1)); i <= y; i++)
        {
            if (isPrime(i))
            {
                totalCount++;
                primeNumber++;
            }
        }
        if (primeNumber != 0)
            printf("p%d: calculate partial count of prime numbers from %d to %d  Count = %d\n", rank, x + (r * (size - 1)), y, primeNumber);
    }
    else
    {
        int x = 0, r = 0;
        MPI_Recv(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&r, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int start = x + ((rank - 1) * r);
        int end = x + (rank * r);
        int localCount = 0;
        for (int i = start; i < end; i++)
        {
            localCount += (isPrime(i)) ? 1 : 0;
        }
        if (localCount != 0)
            printf("p%d: calculate partial count of prime numbers from %d to %d  Count = %d\n", rank, start, end - 1, localCount);
        MPI_Send(&localCount, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }
    if (rank == 0)
    {
        for (int i = 1; i < size; i++)
        {
            int localCount;
            MPI_Recv(&localCount, 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            totalCount += localCount;
        }
        timeEnd = MPI_Wtime();
        printf("After reduction, P0 will have Count = %d\n", totalCount);
        printf("\n");
        printf("Total time: %f\n", timeEnd - timeStart);
        printf("Total time in ms: %f\n", (timeEnd - timeStart) * 1000);
    }
    MPI_Finalize();
}
