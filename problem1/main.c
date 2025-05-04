#include <stdio.h>
#include <mpi.h>

int isPrime(int n){
    if (n < 2) return 0;
    for (int i = 2; i <= n / 2; i++) {
        if (n % i == 0)
            return 0;
    }
    return 1;
}

int main(int argc, char** argv) {
    int x, y, rank, size, local_count = 0, total_count = 0;
    int start, end;
    double startTime, endTime;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Enter lower bound: ");
        scanf("%d", &x);
        printf("Enter upper bound: ");
        scanf("%d", &y);
    }

    startTime = MPI_Wtime();

    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&y, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int range = y - x + 1;
    int base = range / size;
    int remaining = range % size;

    int extra = (rank < remaining) ? 1 : 0;
    int offset = (rank < remaining) ? rank : remaining;
    start = x + rank * base + offset;
    end = start + base + extra - 1;

    if (end > y)
        end = y;

    for (int i = start; i <= end; i++) {
        if (isPrime(i)) local_count++;
    }

    MPI_Reduce(&local_count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    endTime = MPI_Wtime();

    if (rank == 0) {
        printf("Total prime numbers between %d and %d = %d\n", x, y, total_count);
        printf("Execution Time (MPI_Bcast + Reduce): %f seconds\n", endTime - startTime);
    }

    MPI_Finalize();
    return 0;
}

