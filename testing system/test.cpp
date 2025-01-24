#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "test.hpp"

std::vector<TestCase> testQueue; // Global queue for storing test cases
std::mutex outputMutex; // Mutex to synchronize output
std::atomic<int> passedTests(0); // Atomic counter to track completed tests
std::mutex queueMutex; // Mutex to protect access to testQueue
uint64_t totalTests = 0;

// Function to execute each test
void runSingleTest(const TestCase& test, int index, bool exitOnFail) {
    bool passed = test.run();

    if (!passed) {
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "[" << index << "]: " << test.failMesg << std::endl;
        if (exitOnFail) {
            std::exit(1);  // Exit if fail on test failure
        }
        return;
    }
    
    int currentPassed = passedTests.fetch_add(1, std::memory_order_relaxed);
    std::lock_guard<std::mutex> lock(outputMutex);
    std::cout << "[" << currentPassed + 1 << "/" << totalTests << "]\r";
    std::cout.flush();
}

// Worker thread function that pulls from the queue
void workerThread(int totalTests, bool exitOnFail) {
    while (true) {
        TestCase test;
        int index = -1;

        // Lock the queue and get a test case if available
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (!testQueue.empty()) {
                test = testQueue.back();
                index = testQueue.size() - 1;
                testQueue.pop_back();
            } else {
                // If the queue is empty, break the loop
                return;
            }
        }

        runSingleTest(test, index, exitOnFail);
    }
}

bool runTests(bool exitOnFail) {

    totalTests = testQueue.size();
    if (totalTests == 0) {
        std::cout << "No tests to run." << std::endl;
        return true;
    }


    // Determine the number of threads to use
    int numThreads = std::max((unsigned)10, std::thread::hardware_concurrency());

    // Spawn worker threads
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread(workerThread, totalTests, exitOnFail));
    }

    // Wait for all threads to finish
    for (std::thread& thread : threads) {
        thread.join();
    }

    bool allPassed = passedTests == totalTests;

    if (allPassed) {
        std::cout << "All tests PASSED!\r" << std::endl;
        std::cout.flush();
    } else {
        std::cout << "[" << passedTests << "/" << totalTests << "] tests PASSED\r" << std::endl;
        std::cout.flush();
    }

    return allPassed;
}
