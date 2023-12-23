// Разработайте программу для задачи 4 с использованием
// матриц специального типа (ленточных, треугольных и т.п.)

#include <iostream>
#include <time.h>
#include <omp.h>
#include <chrono>

const int size_cases = 5;
const int threads_cases = 14;
size_t sizes[size_cases] = {100, 500, 1000, 5000, 10000};
size_t num_threads_list[threads_cases] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 16, 32};

void findMaxOfMinBand(int **matrix, int size, int bandwidth, int threads)
{
    int max = -INT32_MAX, i = 0, j = 0, chunk = size / threads;
#pragma omp parallel for private(i, j) shared(matrix) schedule(dynamic, chunk) reduction(max : max)
    for (i = 0; i < size; ++i)
    {
        int lower_limit, upper_limit, minValue;
        if (i - bandwidth > 0)
        {
            lower_limit = i - bandwidth;
        }
        else
        {
            lower_limit = 0;
        }

        if (i + bandwidth < size)
        {
            upper_limit = i + bandwidth;
        }
        else
        {
            upper_limit = size - 1;
        }

        minValue = matrix[i][lower_limit];
        // printf("i: %d, l: %d, u: %d\n", i, lower_limit, upper_limit);

        for (j = lower_limit; j < upper_limit; ++j)
        {
            if (matrix[i][j] < minValue)
            {
                minValue = matrix[i][j];
            }
        }
        // printf("Min value for row %d = %d \n", i, minValue);
        max = minValue;
    }

    // printf("Max value =  %d \n", max);
}

int **getBandMatrix(int size, int bandwidth)
{
    int **ar = new int *[size];
    for (int i = 0; i < size; i++)
    {
        ar[i] = new int[size];
    }

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (abs(i - j) <= bandwidth)
            {
                ar[i][j] = rand() % 10 + 1;
            }
            else
            {
                ar[i][j] = 0;
            }
        }
    }
    return ar;
}

void printMatrix(int **ar, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            std::cout << ar[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

double measureTime(void (*f)(int **ar, int n, int bandwidth, int threads), int **ar, int n, int bandwidth, int threads)
{
    using std::chrono::duration;
    using std::chrono::high_resolution_clock;

    const int iters = 5;
    double times[iters];

    for (int i = 0; i < iters; ++i)
    {
        auto t1 = high_resolution_clock::now();
        (*f)(ar, n, bandwidth, threads);
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

void printTimeMatrix(double **ar, int m, int n)
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
        int **matrix = getBandMatrix(sizes[i], 1);
        for (int j = 0; j < threads_cases; j++)
        {
            omp_set_num_threads(num_threads_list[j]);
            average_time[i][j] = measureTime(findMaxOfMinBand, matrix, sizes[i], 1, num_threads_list[j]);
        }
        for (int i = 0; i < sizes[i]; i++)
        {
            delete[] matrix[i];
        }
        delete[] matrix;
    }
    printTimeMatrix(average_time, size_cases, threads_cases);

    for (int i = 0; i < size_cases; i++)
    {
        delete[] average_time[i];
    }
    delete[] average_time;
    return 0;
}