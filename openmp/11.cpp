// Исследование режимов распараллеливания цикла for (static,
// dynamic, guided): придумать цикл с неравномерной нагрузкой
// итераций (например, на некоторых итерациях генерировать случайные числа)

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <chrono>

const int size_cases = 5;
const int threads_cases = 14;
size_t sizes[size_cases] = {10000, 50000, 100000, 500000, 1000000};
size_t num_threads_list[threads_cases] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 16, 32};

int *getArray(int n)
{
    int *ar = new int[n];
    for (int i = 0; i < n; i++)
    {
        ar[i] = rand() % 10;
    }
    return ar;
}

void printArray(int *ar, int n)
{
    for (int i = 0; i < n; i++)
    {
        std::cout << ar[i] << " ";
    }
    std::cout << std::endl;
}

void testStatic(int *ar, int n, int threads)
{
    int maxValue = 0;
    int chunk = n / threads;
#pragma omp parallel shared(ar, n)
    {
#pragma omp for schedule(static, chunk) reduction(max : maxValue)
        for (int i = 0; i < n; ++i)
        {
            if (i % 3 == 1)
            {
                int temp = rand() % 10;
            }
            if (ar[i] > maxValue)
            {
                maxValue = ar[i];
            }
        }
    }
    // printf("max(static) : %d \n", maxValue);
}

void testDynamic(int *ar, int n, int threads)
{
    int maxValue = 0;
    int chunk = n / threads;
#pragma omp parallel shared(ar, n)
    {
#pragma omp for schedule(dynamic, chunk) reduction(max : maxValue)
        for (int i = 0; i < n; ++i)
        {
            if (i % 3 == 1)
            {
                int temp = rand() % 10;
            }
            if (ar[i] > maxValue)
            {
                maxValue = ar[i];
            }
        }
    }
    // printf("max(dynamic) : %d \n", maxValue);
}

void testGuided(int *ar, int n, int threads)
{
    int maxValue = 0;
    int chunk = n / threads;
#pragma omp parallel shared(ar, n)
    {
#pragma omp for schedule(guided, chunk) reduction(max : maxValue)
        for (int i = 0; i < n; ++i)
        {
            if (i % 3 == 1)
            {
                int temp = rand() % 10;
            }
            if (ar[i] > maxValue)
            {
                maxValue = ar[i];
            }
        }
    }
    // printf("max(guided) : %d \n", maxValue);
}

double measureTime(void (*f)(int *ar, int n, int threads), int *ar, int n, int threads)
{
    using std::chrono::duration;
    using std::chrono::high_resolution_clock;

    const int iters = 5;
    double times[iters];

    for (int i = 0; i < iters; ++i)
    {
        auto t1 = high_resolution_clock::now();
        (*f)(ar, n, threads);
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
            average_time[i][j] = measureTime(testStatic, ar, sizes[i], num_threads_list[j]); //testDynamic testGuided
        }
        delete[] ar;
    }

    printMatrix(average_time, size_cases, threads_cases);
    for (int i = 0; i < size_cases; i++)
    {
        delete[] average_time[i];
    }
    delete[] average_time;
    return 0;
}
