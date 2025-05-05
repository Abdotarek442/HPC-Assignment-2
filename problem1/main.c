#include <stdio.h>
#include <mpi.h>

int isPrime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char** argv) {
    int x, y, rank, size;
    int start, end, local_count = 0, total_count = 0;
    int local_primes[100], idx = 0;
    double startTime, endTime;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("enter lower bound: ");
        scanf("%d", &x);
        printf("enter upper bound: ");
        scanf("%d", &y);
    }

    startTime = MPI_Wtime();

    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&y, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int range = y - x ;
    int base = range / (size - 1);
    int remaining = range % (size - 1);

    int extra = (rank - 1 < remaining) ? 1 : 0;  
    int offset = (rank - 1 < remaining) ? rank - 1 : remaining; 
    start = x + (rank - 1) * base + offset;
    end = start + base + extra - 1;
    if (end > y) end = y;
    

    for (int i = start; i <= end; i++) {
        if (isPrime(i)) {
            local_primes[idx++] = i;
            local_count++;
        }
    }

    if (rank != 0) {
        printf("p%d: calculate partial count of prime numbers from %d to %d  Count = %d (", rank, start, end, local_count);
        for (int i = 0; i < idx; i++) {
            printf("%d", local_primes[i]);
            if (i < idx - 1) printf(", ");
        }
        printf(")\n");
    }

    MPI_Reduce(&local_count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    
    if (rank != 0) {
        MPI_Send(&idx, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(local_primes, idx, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        int all_primes[1000], total_idx = 0;
        for (int i = 0; i < idx; i++) {
            all_primes[total_idx++] = local_primes[i];
    }

        
        for (int i = 1; i < size; i++) {
            int count;
            MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (count > 0) {
                int buffer[100];
                MPI_Recv(buffer, count, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int j = 0; j < count; j++) {
                    all_primes[total_idx++] = buffer[j];
                }
            }
        }

        endTime = MPI_Wtime();

        printf("P0 will have Count = %d (", total_count);
        for (int i = 0; i < total_idx; i++) {
            printf("%d", all_primes[i]);
            if (i < total_idx - 1) printf(", ");
        }
        printf(")\n");

        printf("Execution Time (MPI_Bcast + Reduce): %f seconds\n", endTime - startTime);
    }

    MPI_Finalize();
    return 0;
}

