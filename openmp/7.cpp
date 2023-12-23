// Разработайте программу для вычисления скалярного произведения
// для последовательного набора векторов (исходные данные в файле)

#include <iostream>
#include <fstream>
#include <time.h>
#include <omp.h>
#include <chrono>

const int size_cases = 5;
const int threads_cases = 4;
size_t sizes[size_cases] = {10, 100, 500, 1000, 10000};
size_t num_threads_list[threads_cases] = {1, 2, 3, 4};

void printArray(int *ar, int n)
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

int *findDotProduct(int *a, int *b, int n)
{
    int *result = new int[n];
    for (int i = 0; i < n; i++)
    {
        result[i] = a[i] * b[i];
    }
    return result;
}

void calculateSum(int *a, int n)
{
    int result = 0;
    for (int i = 0; i < n; i++)
        result += a[i];
    // printf("Dot product: %d \n", result);
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
    std::fstream file("input.txt", std::ios_base::in | std::ios_base::out);

    for (int i = 0; i < size_cases; i++)
    {
        file << sizes[i];
        file << " ";
        file << 10;
        file << "\n";

        for (int j = 0; j < 10; j++)
        {
            int *ar = getArray(sizes[i]);
            for (int k = 0; k < sizes[i]; k++)
            {
                file << ar[k] << " ";
            }
            file << "\n";
            delete[] ar;
        }
        file.clear();
        file.seekg(0);

        for (int j = 0; j < threads_cases; j++)
        {
            omp_set_num_threads(num_threads_list[j]);
            using std::chrono::duration;
            using std::chrono::high_resolution_clock;

            const int iters = 5;
            double times[iters];

            for (int k = 0; k < iters; k++)
            {
                auto t1 = high_resolution_clock::now();
                int n, m;
                file >> n;
                file >> m;
                int **a = new int *[m];
                int *res = new int[n];
                for (int l = 0; l < m; l++)
                {
                    a[l] = new int[n];
                }

                for (int l = 0; l < n; l++)
                {
                    file >> a[0][l];
                    res[l] = 1;
                }

                for (int l = 1; l < m + 1; l++)
                {
#pragma omp sections
                    {
#pragma omp section
                        if (!file.eof())
                        {
                            for (int h = 0; h < n; h++)
                            {
                                file >> a[l][h];
                            }
                        }
#pragma omp section
                        res = findDotProduct(res, a[l - 1], n);
                    }
                }
                calculateSum(res, n);
                auto t2 = high_resolution_clock::now();
                duration<double, std::milli> ms_double = t2 - t1;
                times[k] = ms_double.count();
            }
            double average = 0.0;
            for (int u = 0; u < iters; ++u)
            {
                average += times[u];
            }
            average_time[i][j] = average / iters;
        }

        file.clear();
        file.seekg(0);
    }

    printMatrix(average_time, size_cases, threads_cases);
    for (int i = 0; i < size_cases; i++)
    {
        delete[] average_time[i];
    }
    delete[] average_time;
    return 0;
}
