#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

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

    if (rank == 0) {
        printf("Enter lower bound (x): ");
        scanf("%d", &x);
        printf("Enter upper bound (y): ");
        scanf("%d", &y);

        int total_numbers = y - x + 1;
        int workers = size - 1;
        int numbers_per_worker = total_numbers / workers;
        int remainder = total_numbers % workers;

        for (int i = 1; i < size; i++) {
            int start = x + (i - 1) * numbers_per_worker;
            int end = start + numbers_per_worker - 1;

            if (i <= remainder) {
                end++;
            }

            if (i == size - 1) {
                end = y;
            }

            MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        int total_count = 0;
        int* all_primes = (int*)malloc(total_numbers * sizeof(int));
        int index = 0;

        for (int i = 1; i < size; i++) {
            int count;
            MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_count += count;

            int* primes = (int*)malloc(count * sizeof(int));
            MPI_Recv(primes, count, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int j = 0; j < count; j++) {
                all_primes[index++] = primes[j];
            }
            free(primes);
        }

        printf("\nTotal primes in [%d, %d]: %d\n", x, y, total_count);
        printf("Primes: ");
        for (int i = 0; i < index; i++) {
            printf("%d ", all_primes[i]);
        }
        printf("\n");
        free(all_primes);

    } else {
        int start, end;
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int count = 0;
        int* primes = (int*)malloc((end - start + 1) * sizeof(int));

        for (int i = start; i <= end; i++) {
            if (is_prime(i)) {
                primes[count++] = i;
            }
        }

        printf("Process %d: primes in [%d, %d] = %d\n", rank, start, end, count);
        for (int i = 0; i < count; i++) {
            printf("%d ", primes[i]);
        }
        printf("\n");

        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(primes, count, MPI_INT, 0, 0, MPI_COMM_WORLD);
        free(primes);
    }

    MPI_Finalize();
    return 0;
}
جربي ده كدا
