#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <chrono>

const int size_cases = 1;
const int threads_cases = 1;
size_t sizes[size_cases] = {1};
size_t num_threads_list[threads_cases] = {4};

int *getArray(int n)
{
    int *ar = new int[n];
    for (int i = 0; i < n; i++)
    {
        ar[i] = rand() % 100;
    }
    return ar;
}

void printArray(int *ar, int n)
{
    for (int i = 0; i < n; i++)
    {
        std::cout << ar[i] << ", ";
    }
    std::cout << std::endl;
}

double measureTime(int (*f)(int *ar, int n), int *ar, int n)
{
    using std::chrono::duration;
    using std::chrono::high_resolution_clock;

    const int iters = 5;
    double times[iters];

    for (int i = 0; i < iters; ++i)
    {
        auto t1 = high_resolution_clock::now();
        (*f)(ar, n);
        auto t2 = high_resolution_clock::now();
        duration<double, std::milli> ms_double = t2 - t1;
        times[i] = ms_double.count();
    }

    double average = 0.0;
    for (int i = 0; i < iters; ++i)
    {
        average += times[i];
    }
    average = average / iters;

    return average;
}

void printMatrix(double **ar, int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            std::cout << ar[i][j] << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void exampleProblem(int *ar, int n)
{
#pragma omp parallel
    {
        printf("level: %d, number of threads: %d\n", 1, omp_get_num_threads());
#pragma omp parallel
        {
#pragma omp parallel
            {
#pragma omp parallel
                {
                    printf("level: %d, number of threads: %d\n", 4, omp_get_num_threads());
                }
            }
        }
    }
}

double measureTime(void (*f)(int *ar, int n), int *ar, int n)
{
    using std::chrono::duration;
    using std::chrono::high_resolution_clock;

    const int iters = 1;
    double times[iters];

    for (int i = 0; i < iters; ++i)
    {
        auto t1 = high_resolution_clock::now();
        (*f)(ar, n);
        auto t2 = high_resolution_clock::now();
        duration<double, std::milli> ms_double = t2 - t1;
        times[i] = ms_double.count();
    }

    double average = 0.0;
    for (int i = 0; i < iters; ++i)
    {
        average += times[i];
    }
    average = average / iters;

    return average;
}

int main()
{
    srand(time(NULL));
    double **average_time = new double *[size_cases];
    for (int i = 0; i < size_cases; i++)
    {
        average_time[i] = new double[threads_cases];
    }

    for (int i = 0; i < size_cases; i++)
    {
        int *ar = getArray(sizes[i]);
        for (int j = 0; j < threads_cases; j++)
        {
            omp_set_num_threads(num_threads_list[j]);
            omp_set_nested(1);
            average_time[i][j] = measureTime(exampleProblem, ar, sizes[i]);
        }
        delete[] ar;
    }
    printf("\n");
    printMatrix(average_time, size_cases, threads_cases);

    for (int i = 0; i < size_cases; i++)
    {
        delete[] average_time[i];
    }
    delete[] average_time;
    return 0;
}
