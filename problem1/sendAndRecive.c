#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

// Function to check if a number is prime
int is_prime(int num) {
    if (num <= 1) return 0;
    if (num <= 3) return 1;
    if (num % 2 == 0 || num % 3 == 0) return 0;
    for (int i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) return 0;
    }
    return 1;
}

int main(int argc, char** argv) {
    int rank, size;
    int x, y;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Master process (rank 0)
    if (rank == 0) {
        printf("Enter lower bound (x): ");
        scanf("%d", &x);
        printf("Enter upper bound (y): ");
        scanf("%d", &y);

        int total_range = y - x + 1;
        int subrange_size = total_range / (size - 1);
        int remaining = total_range % (size - 1); 

        for (int i = 1; i < size; ++i) {
            int start = x + (i - 1) * subrange_size + (i <= remaining ? i - 1 : remaining);
            int end = start + subrange_size - 1;
            if (i <= remaining) {
                end++;
            }
            MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        int total_count = 0;
        int* all_primes = (int*) malloc(total_range * sizeof(int));
        int total_primes_index = 0;

        for (int i = 1; i < size; ++i) {
            int received_count;
            MPI_Recv(&received_count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_count += received_count;

            int* received_primes = (int*) malloc(received_count * sizeof(int));
            MPI_Recv(received_primes, received_count, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j < received_count; ++j) {
                all_primes[total_primes_index++] = received_primes[j];
            }
            free(received_primes);
        }

        printf("Total number of prime numbers between %d and %d: %d\n", x, y, total_count);
        printf("List of prime numbers:\n");
        for (int i = 0; i < total_primes_index; ++i) {
            printf("%d ", all_primes[i]);
        }
        printf("\n");
        free(all_primes);
    } else {
        int lower_bound, upper_bound;
        MPI_Recv(&lower_bound, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&upper_bound, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int local_count = 0;
        int* local_primes = (int*) malloc((upper_bound - lower_bound + 1) * sizeof(int));

        for (int i = lower_bound; i <= upper_bound; ++i) {
            if (is_prime(i)) {
                local_primes[local_count++] = i;
            }
        }

        printf("Process %d found %d prime numbers: ", rank, local_count);
        for (int i = 0; i < local_count; ++i) {
            printf("%d ", local_primes[i]);
        }
        printf("\n");

        MPI_Send(&local_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(local_primes, local_count, MPI_INT, 0, 0, MPI_COMM_WORLD);

        free(local_primes);
    }

    MPI_Finalize();
    return 0;
}