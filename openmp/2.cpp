// Разработайте программу для вычисления
// скалярного произведения двух векторов

#include <iostream>
#include <time.h>
#include <omp.h>
#include <chrono>

const int size_cases = 5;
const int threads_cases = 14;
size_t sizes[size_cases] = {10000, 50000, 100000, 500000, 1000000};
size_t num_threads_list[threads_cases] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 16, 32};

void findDotProduct(int *a, int *b, int n)
{
    int result = 0;
#pragma omp parallel for default(shared) reduction(+ : result)
    for (int i = 0; i < n; i++)
        result += (a[i] * b[i]);

    printf("Dot product: %d \n", result);
}

double measureTime(void (*f)(int *a, int *b, int n), int *a, int *b, int n)
{
    using std::chrono::duration;
    using std::chrono::high_resolution_clock;

    const int iters = 5;
    double times[iters];

    for (int i = 0; i < iters; ++i)
    {
        auto t1 = high_resolution_clock::now();
        (*f)(a, b, n);
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
        int *a = new int[sizes[i]];
        int *b = new int[sizes[i]];
        for (int j = 0; j < sizes[i]; j++)
        {
            a[j] = rand() % 10;
            b[j] = rand() % 10;
        }
        for (int k = 0; k < threads_cases; k++)
        {
            omp_set_num_threads(num_threads_list[k]);
            average_time[i][k] = measureTime(findDotProduct, a, b, sizes[i]);
        }
        delete[] a;
        delete[] b;
    }
    printMatrix(average_time, size_cases, threads_cases);

    for (int i = 0; i < size_cases; i++)
    {
        delete[] average_time[i];
    }
    delete[] average_time;
    return 0;
}