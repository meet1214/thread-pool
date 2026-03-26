#include <iostream>
#include <chrono>
#include "../include/ThreadPool.h"
#include <future>
#include <vector>
#include <cmath>
#include <iomanip>

const int NUM_TASKS = 100;
const int TASK_LOAD = 100000;

auto cpu_task = [](int n) {
    double result = 0;
    for(int i = 0; i < n; i++)
        result += std::sqrt(i);
    return result;
};

long long run_single_threaded(){

    double total = 0.00;
    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < NUM_TASKS; i++){
        total += cpu_task(TASK_LOAD);
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    auto time = duration.count();

    return time;
}

long long run_async() {

    std::vector<std::future<double>> futures;
    double total = 0.00;

    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < NUM_TASKS ; i++){
        futures.emplace_back(std::async(std::launch::async, cpu_task, TASK_LOAD)) ;
    }

    for(auto&f : futures){
        total += f.get();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    auto time = duration.count();

    return time;
}

long long run_threadpool(int num_threads){
    auto pool = std::make_unique<ThreadPool>(num_threads);

    std::vector<std::future<double>> futures;
    double total = 0.00;

    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < NUM_TASKS; i++){
        futures.emplace_back(pool->submit(cpu_task,TASK_LOAD));
    }

    for(auto&f : futures){
        total += f.get();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    auto time = duration.count();
    pool->shutdown();

    return time;
}

int main(){
    
    long long result1 = run_single_threaded();
    long long result2 = run_async();
    long long result3 = run_threadpool(2); 
    long long result4 = run_threadpool(4);
    long long result5 = run_threadpool(8);

    std::cout << std::left
          << std::setw(20) << "Method"
          << std::setw(10) << "Threads"
          << std::setw(12) << "Time(ms)"
          << std::setw(10) << "Speedup" << "\n";
    std::cout << std::string(52, '-') << "\n";

    std::cout << std::left
          << std::setw(20) << "Single-threaded"
          << std::setw(10) << "1"
          << std::setw(12) << result1
          << std::fixed << std::setprecision(1)<< std::setw(10) << "1.0x" << "\n";

    std::cout << std::left
          << std::setw(20) << "std::async"
          << std::setw(10) << "-"
          << std::setw(12) << result2
          << std::fixed << std::setprecision(1)<< std::setw(10) << double(result1)/result2 << "\n";
          
    std::cout << std::left
          << std::setw(20) << "ThreadPool"
          << std::setw(10) << "2"
          << std::setw(12) << result3
          << std::fixed << std::setprecision(1)<< std::setw(10) << double(result1)/result3 << "\n";

    std::cout << std::left
          << std::setw(20) << "ThreadPool"
          << std::setw(10) << "4"
          << std::setw(12) << result4
          << std::fixed << std::setprecision(1)<< std::setw(10) << double(result1)/result4 << "\n";

    std::cout << std::left
          << std::setw(20) << "ThreadPool"
          << std::setw(10) << "8"
          << std::setw(12) << result5
          << std::fixed << std::setprecision(1)<< std::setw(10) << double(result1)/result5 << "\n";

    return 0;
}