// Разработайте программу для нахождения
// минимального (максимального) значения среди элементов вектора

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

int main(int argc, char *argv[])
{
    srand(time(NULL));

    int pid, np;

    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    double *average_time = new double[size_cases];

    for (int s = 0; s < size_cases; s++)
    {
        int *a = getArray(sizes[s]);
        int a2[sizes[s]];
        int size, rank;
        int elements_per_process,
            n_elements_recieved;

        using std::chrono::duration;
        using std::chrono::high_resolution_clock;
        const int iters = 5;
        double times[iters];
        for (int j = 0; j < iters; j++)
        {
            MPI_Barrier(MPI_COMM_WORLD);
            auto t1 = high_resolution_clock::now();

            if (pid == 0)
            {
                // printArray(a, sizes[s]);
                int index, i;
                elements_per_process = sizes[s] / np;

                if (np > 1)
                {
                    for (i = 1; i < np - 1; i++)
                    {
                        index = i * elements_per_process;

                        MPI_Send(&elements_per_process,
                                 1, MPI_INT, i, 0,
                                 MPI_COMM_WORLD);
                        MPI_Send(&a[index],
                                 elements_per_process,
                                 MPI_INT, i, 0,
                                 MPI_COMM_WORLD);
                    }

                    index = i * elements_per_process;
                    int elements_left = sizes[s] - index;

                    MPI_Send(&elements_left,
                             1, MPI_INT,
                             i, 0,
                             MPI_COMM_WORLD);
                    MPI_Send(&a[index],
                             elements_left,
                             MPI_INT, i, 0,
                             MPI_COMM_WORLD);
                }

                int minimum = INT32_MAX;
                for (i = 0; i < elements_per_process; i++)
                    if (a[i] < minimum)
                        minimum = a[i];

                int tmp;
                for (i = 1; i < np; i++)
                {
                    MPI_Recv(&tmp, 1, MPI_INT,
                             MPI_ANY_SOURCE, 0,
                             MPI_COMM_WORLD,
                             &status);
                    int sender = status.MPI_SOURCE;
                    if (tmp < minimum)
                        minimum = tmp;
                }

                // printf("min of array is : %d\n", minimum);
            }
            else
            {
                MPI_Recv(&n_elements_recieved,
                         1, MPI_INT, 0, 0,
                         MPI_COMM_WORLD,
                         &status);

                MPI_Recv(&a2, n_elements_recieved,
                         MPI_INT, 0, 0,
                         MPI_COMM_WORLD,
                         &status);

                int local_min = INT32_MAX;
                for (int i = 0; i < n_elements_recieved; i++)
                {
                    if (a2[i] < local_min)
                        local_min = a2[i];
                }

                MPI_Send(&local_min, 1, MPI_INT,
                         0, 0, MPI_COMM_WORLD);
            }

            MPI_Barrier(MPI_COMM_WORLD);
            auto t2 = high_resolution_clock::now();
            duration<double, std::milli> ms_double = t2 - t1;
            times[j] = ms_double.count();
            // printf("time %f\n", ms_double.count());
            // printTimeArray(times, iters);
        }

        double average = 0.0;
        for (int k = 0; k < iters; k++)
        {
            average += times[k];
        }
        average_time[s] = average / iters;
        delete[] a;
    }

    MPI_Finalize();

    if (pid == 0)
    {
        printTimeArray(average_time, size_cases);
    }

    delete[] average_time;
    return 0;
}