// Разработайте программу для задачи вычисления определенного
// интеграла с использованием метода прямоугольников

#include <iostream>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <chrono>

const int size_cases = 5;
const int threads_cases = 14;
size_t sizes[size_cases] = {10000, 50000, 100000, 500000, 1000000};
size_t num_threads_list[threads_cases] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 16, 32};

float f(float x)
{
    return sin(x); // integrand
}

void computeIntegral(int n)
{
    int i;
    float a, b, h, S, x;

    a = 0.0;  // lower limit
    b = M_PI; // upper limit
    h = (b - a) * 1.0 / n;
    S = 0; // result
    x = 0.0;

#pragma omp parallel for private(i, x) shared(n, h) reduction(+ : S)
    for (i = 0; i < n - 1; i++)
    {
        x = i * h;
        S += f(x);
    }
    S *= h;

    printf("S = %f \n", S);
}

double measureTime(void (*f)(int n), int n)
{
    using std::chrono::duration;
    using std::chrono::high_resolution_clock;

    const int iters = 5;
    double times[iters];

    for (int i = 0; i < iters; ++i)
    {
        auto t1 = high_resolution_clock::now();
        (*f)(n);
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
        for (int j = 0; j < threads_cases; j++)
        {
            omp_set_num_threads(num_threads_list[j]);
            average_time[i][j] = measureTime(computeIntegral, sizes[i]);
        }
    }
    printMatrix(average_time, size_cases, threads_cases);

    for (int i = 0; i < size_cases; i++)
    {
        delete[] average_time[i];
    }
    delete[] average_time;
    return 0;
}