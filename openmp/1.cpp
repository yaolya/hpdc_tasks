// Разработайте программу для нахождения
// минимального (максимального) значения среди элементов вектора

#include <iostream>
#include <time.h>
#include <chrono>
#include <omp.h>

const int size_cases = 5;
const int threads_cases = 14;
size_t sizes[size_cases] = {10000, 50000, 100000, 500000, 1000000};
size_t num_threads_list[threads_cases] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 16, 32};

void findMinMax(int *ar, int n)
{
    int min = ar[0];
    int max = ar[0];
    int i = 0;

#pragma omp parallel for private(i) shared(ar) \
    reduction(min : min) reduction(max : max)
    for (i = 0; i < n; i++)
    {
        if (ar[i] < min)
            min = ar[i];
        if (ar[i] > max)
            max = ar[i];
    }

    printf("Min value: %d \n", min);
    printf("Max value: %d \n", max);
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

void printArray(int *ar, int n)
{
    for (int i = 0; i < n; i++)
    {
        std::cout << ar[i] << ", ";
    }
    std::cout << std::endl;
}

double measureTime(void (*f)(int *ar, int n), int *ar, int n)
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
            average_time[i][j] = measureTime(findMinMax, ar, sizes[i]);
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
