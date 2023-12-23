// Реализуйте операцию редукции с использованием разных
// способов организации взаимоисключения (атомарные операции,
// критические секции, синхронизация при помощи замков)

#include <iostream>
#include <time.h>
#include <omp.h>
#include <chrono>

const int size_cases = 1;
const int threads_cases = 14;
size_t sizes[size_cases] = {500000};
size_t num_threads_list[threads_cases] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 16, 32};

int useReduction(int *ar, int n)
{
    int sum = 0;

#pragma omp parallel for reduction(+ : sum)
    for (int i = 0; i < n; i++)
        sum += ar[i];

    return sum;
}

int useConcurrent(int *ar, int n)
{
    int sum = 0;
    for (int i = 0; i < n; i++)
        sum += ar[i];

    return sum;
}

int useAtomicOperation(int *ar, int n)
{
    int sum = 0;

#pragma omp parallel for
    for (int i = 0; i < n; i++)
#pragma omp atomic
        sum += ar[i];

    return sum;
}

int useCriticalSection(int *ar, int n)
{
    int sum = 0;

#pragma omp parallel for
    for (int i = 0; i < n; i++)
#pragma omp critical
    {
        sum += ar[i];
    }

    return sum;
}

omp_lock_t lock;
int useLock(int *ar, int n)
{
    int sum = 0;
#pragma omp parallel for
    for (int k = 0; k < n; k++)
    {
        omp_set_lock(&lock);
        sum += ar[k];
        omp_unset_lock(&lock);
    }
    omp_destroy_lock(&lock);
    return sum;
}

int *getArray(int n)
{
    int *ar = new int[n];
    for (int i = 0; i < n; i++)
    {
        ar[i] = rand() % 100;
    }
    return ar;
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
            average_time[i][j] = measureTime(useConcurrent, ar, sizes[i]);
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