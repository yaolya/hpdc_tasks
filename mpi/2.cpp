// Разработайте программу для вычисления
// скалярного произведения двух векторов

#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <chrono>

const int size_cases = 3;
size_t sizes[size_cases] = {10240, 102400, 1024000};

void printArray(int *ar, int n)
{
    for (int i = 0; i < n; i++)
    {
        std::cout << ar[i] << " ";
    }
    std::cout << std::endl;
}

void printTimeArray(double *ar, int n)
{
    for (int i = 0; i < n; i++)
    {
        std::cout << ar[i] << " ";
    }
    std::cout << std::endl;
}

int *getArray(int n)
{
    int *ar = new int[n];
    for (int i = 0; i < n; i++)
    {
        ar[i] = rand() % 10;
    }

    return ar;
}

double dotProduct(int *x, int *y, int n)
{
    int i;
    int prod = 0;
    for (i = 0; i < n; i++)
    {
        prod += x[i] * y[i];
    }
    // printf("partial dot product = %d\n", prod);
    return prod;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int my_rank;
    int num_procs;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    double *average_time = new double[size_cases];

    for (int i = 0; i < size_cases; i++)
    {
        int *a = getArray(sizes[i]);
        int *b = getArray(sizes[i]);

        using std::chrono::duration;
        using std::chrono::high_resolution_clock;
        const int iters = 5;
        double times[iters];
        for (int j = 0; j < iters; j++)
        {
            int prod;
            MPI_Barrier(MPI_COMM_WORLD);
            auto t1 = high_resolution_clock::now();
            // if (my_rank == 0)
            // {
            //     printf("a: ");
            //     printArray(a, sizes[i]);
            //     printf("b: ");
            //     printArray(b, sizes[i]);
            //     printf("\n");
            // }

            int local_N = sizes[i] / num_procs;
            int local_x[local_N];
            int local_y[local_N];
            for (int k = 0; k < local_N; k++)
            {
                local_x[k] = a[k + my_rank * local_N];
                local_y[k] = b[k + my_rank * local_N];
            }

            int local_prod;
            // printArray(local_x, local_N);
            // printArray(local_y, local_N);
            local_prod = dotProduct(local_x, local_y, local_N);
            MPI_Reduce(&local_prod, &prod, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
            // if (my_rank == 0)
            // {
            //     printf("dotProduct = %d\n", prod);
            // }
            MPI_Barrier(MPI_COMM_WORLD);
            auto t2 = high_resolution_clock::now();
            duration<double, std::milli> ms_double = t2 - t1;
            times[j] = ms_double.count();
        }

        double average = 0.0;
        for (int k = 0; k < iters; k++)
        {
            average += times[k];
        }
        average_time[i] = average / iters;
        delete[] a;
        delete[] b;
    }

    MPI_Finalize();

    if (my_rank == 0)
    {
        printTimeArray(average_time, size_cases);
    }

    delete[] average_time;
    return 0;
}