#include <iostream>

#include <Lunaris/mutex.h>

using namespace Lunaris::Mutex;


void recursivelly_lock_this(SharedRecursiveMutex& share, int steps);
void recursivelly_share_lock_this(SharedRecursiveMutex& share, int steps);

int main() {
    std::printf("# Testing locking normally a SharedRecursiveMutex...\n");

    SharedRecursiveMutex share;
    recursivelly_lock_this(share, 10);

    std::printf("Testing locking shared mode a SharedRecursiveMutex...\n");

    recursivelly_share_lock_this(share, 10);


    std::printf("Testing lock() and try_lock_shared()...\n");

    share.lock();
    if (share.try_lock_shared()) {
        share.unlock();
        std::printf("Failed: shared lock shouldn't be possible with lock() already locked!\n");
        return 1;
    }
    share.unlock();

    std::printf("Testing try_lock_shared() and lock()...\n");

    share.lock_shared();
    if (share.try_lock()) {
        share.unlock_shared();
        std::printf("Failed: lock shouldn't be possible with lock_shared() already locked!\n");
        return 1;
    }
    share.unlock_shared();

    std::printf("PASSED!\n");

    return 0;
}


void recursivelly_lock_this(SharedRecursiveMutex& share, int steps) {
    std::lock_guard<SharedRecursiveMutex> l(share);

    if (steps <= 0) {
        std::printf("All good, unlocking back...\n");
    }
    else {
        std::printf("Lock step %d\n", steps);
    }

    if (steps > 0) recursivelly_lock_this(share, steps - 1);

    std::printf("Unlock step %d\n", steps);
}

void recursivelly_share_lock_this(SharedRecursiveMutex& share, int steps) {
    std::shared_lock<SharedRecursiveMutex> l(share);

    if (steps <= 0) {
        std::printf("All good, unlocking back...\n");
    }
    else {
        std::printf("Lock step %d\n", steps);
    }

    if (steps > 0) recursivelly_share_lock_this(share, steps - 1);

    std::printf("Unlock step %d\n", steps);
}
