#include <iostream>
#include <atomic>
#include <thread>

#include <Lunaris/mutex.h>

using namespace Lunaris::Mutex;

constexpr size_t c_starts_at = 1000;

void async_fast_thread(FastOneWayMutex& fowm, std::atomic<size_t>& counter);

int main() {
    FastOneWayMutex fowm;
    std::atomic<size_t> counter{c_starts_at};

    std::printf("# Starting thread with fast run...\n");

    std::thread fast(async_fast_thread, std::ref(fowm), std::ref(counter));

    std::printf("Wait thread a bit to see if it increases...\n");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (counter == c_starts_at) {
        std::printf("Thread is not running! ABORT!\n");
        std::terminate();
    }

    std::printf("Attempting to lock...\n");

    size_t prev = counter;
    {
        fowm.lock();

        std::printf("Lock took %zu ticks to happen. Checking if it is locked...\n", counter - prev);
        prev = counter;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (prev != counter) {
            std::printf("Lock did not work for fast lock! It kept running after lock!\n");
            counter = 0;
            fowm.unlock();
            fast.join();
            return 1;
        }

        std::printf("Waiting counter to go up again...\n");
        counter = c_starts_at;
        fowm.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (counter == c_starts_at) {
            std::printf("Thread is not running! ABORT!\n");
            std::terminate();
        }
    }


    std::printf("All good! Testing again with FastLockGuard...\n");

    {
        prev = counter;
        FastLockGuard l(fowm);

        std::printf("Lock took %zu ticks to happen. Checking if it is locked...\n", counter - prev);
        prev = counter;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (prev != counter) {
            std::printf("Lock did not work for fast lock! It kept running after lock!\n");
            counter = 0;
            fowm.unlock();
            fast.join();
            return 1;
        }

        std::printf("Waiting counter to go up again after unlock again...\n");
        counter = c_starts_at;
    }


    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (counter == c_starts_at) {
        std::printf("Thread is not running! ABORT!\n");
        std::terminate();
    }
    
    counter = 0;
    fast.join();

    std::printf("PASSED!\n");

    return 0;
}

void async_fast_thread(FastOneWayMutex& fowm, std::atomic<size_t>& counter) {
    while(counter >= 1000) {
        if (fowm.run()) {
            ++counter;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}