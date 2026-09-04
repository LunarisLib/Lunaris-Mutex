#include <Lunaris/Mutex/mutex.h>
#include <Lunaris/Mutex/exception.h>

namespace Lunaris {
namespace Mutex {
    
    void SharedRecursiveMutex::lock() {
        const auto this_id = std::this_thread::get_id();
        if (_owner == this_id) {
            ++_count;
        }
        else {
            this->std::shared_mutex::lock();
            _owner = this_id;
            _count = 1;
        }
    }

    bool SharedRecursiveMutex::try_lock() {
        const auto this_id = std::this_thread::get_id();
        const bool got = this->std::shared_mutex::try_lock();
        if (got) {
            _owner = this_id;
            _count = 1;
        }
        return got;
    }

    void SharedRecursiveMutex::unlock() {
        const auto this_id = std::this_thread::get_id();
        if (_count > 0 && _owner == this_id) {
            if (--_count == 0) {
                _owner = std::thread::id();
                this->std::shared_mutex::unlock();
            }
        }
        else { // count 0 or not owner
            throw MutexException("SharedRecursiveMutex unlock on non-owned or already unlocked mutex!");
        }
    }

    bool FastOneWayMutex::run() {
        const bool inv = !request_stop;
        slave_ack = inv;
        return inv;
    }

    void FastOneWayMutex::lock() {
        request_stop = true;
        while (slave_ack) {
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void FastOneWayMutex::unlock() {
        request_stop = false;
    }

    FastLockGuard::FastLockGuard(FastOneWayMutex& r)
        : ref(r)
    {
        ref.lock();
    }

    FastLockGuard::~FastLockGuard() {
        ref.unlock();
    }

} // namespace Mutex
} // namespace Lunaris