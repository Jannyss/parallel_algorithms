#include <iostream>
#include <cmath>
#include <chrono>
#include <fstream>
#include <omp.h>

#define NUM_THREADS 16

double f(double x) {
    return (1/(x * x)) * (sin(1/x) * sin(1/x));
}

double calculateIntegralSerial(double a, double b, int n) {
    double step = (b-a) / n;
    double sum = 0;
    double x;

    for(int i=1; i<n-1; i++) {
        x = a + step * i;
        sum += f(x);
    }

    return sum * step/2;
}

double calculateIntegralAtomic(double a, double b, int n) {
    double step = (b-a) / n;
    double sum = 0;
    double x;

    #pragma omp parallel for num_threads(NUM_THREADS) private(x)
    for(int i=1; i<n-1; i++) {
        x = a + step * i;
        #pragma omp atomic
        sum += f(x);
    }

    return sum * step/2;
}

double calculateIntegralCritical(double a, double b, int n) {
    double step = (b-a) / n;
    double sum = 0;
    double x;

    #pragma omp parallel for num_threads(NUM_THREADS) private(x)
    for(int i=1; i<n-1; i++) {
        x = a + step * i;
        #pragma omp critical
        sum += f(x);
    }

    return sum * step/2;
}

double calculateIntegralLocks(double a, double b, int n) {
    double step = (b-a) / n;
    double sum = 0;
    double x;

    omp_lock_t lock;
    omp_init_lock(&lock);

    #pragma omp parallel for private(x)
    for(int i=1; i<n-1; i++) {
        x = a + step * i;
        omp_set_lock(&lock);
        sum += f(x);
        omp_unset_lock(&lock);
    }
    omp_destroy_lock(&lock);
    return sum * step/2;
}

double calculateIntegralReduction(double a, double b, int n) {
    double step = (b - a) / n;
    double sum = 0;
    double x;
#pragma omp parallel num_threads(NUM_THREADS) private(x)
#pragma omp for reduction(+:sum)
    for (int i = 1; i < n - 1; i++) {
        x = a + step * i;
        sum += f(x);
    }

    return sum * step / 2;
}

int main() {
    std::ofstream file;
    file.open("task1_16threads.txt");

    double a_s[7] = {0.00001, 0.0001, 0.001, 0.01, 0.1, 1.0, 10};
    double b_s[7] = {0.0001, 0.001, 0.01, 0.1, 1.0, 10, 100};

    int n = 10000000;

    file << "Serial calculations" << std::endl;

    for(int i=0; i<7; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        calculateIntegralSerial(a_s[i], b_s[i], n);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end-start;
        file << "A=" << a_s[i] << "  B=" << b_s[i] << "  Time=" <<diff.count() << std::endl;
    }

    file << std::endl << "Atomic" << std::endl;

    for(int i=0; i<7; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        calculateIntegralAtomic(a_s[i], b_s[i], n);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end-start;
        file << "A=" << a_s[i] << "  B=" << b_s[i] << "  Time=" <<diff.count() << std::endl;
    }

    file << std::endl << "Critical sections" << std::endl;

    for(int i=0; i<7; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        calculateIntegralCritical(a_s[i], b_s[i], n);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end-start;
        file << "A=" << a_s[i] << "  B=" << b_s[i] << "  Time=" <<diff.count() << std::endl;
    }
    file << std::endl << "Locks" << std::endl;

    for(int i=0; i<6; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        calculateIntegralLocks(a_s[i], b_s[i], n);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end-start;
        file << "A=" << a_s[i] << "  B=" << b_s[i] << "  Time=" <<diff.count() << std::endl;
    }
    file << std::endl << "Reduction" << std::endl;

    for(int i=0; i<7; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        calculateIntegralReduction(a_s[i], b_s[i], n);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end-start;
        file << "A=" << a_s[i] << "  B=" << b_s[i] << "  Time=" <<diff.count() << std::endl;
    }

    file.close();

    return 0;
}
