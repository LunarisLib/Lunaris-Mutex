#pragma once

#include <shared_mutex>
#include <atomic>
#include <thread>

namespace Lunaris {
namespace Mutex {
    /**
     * @brief SharedRecursiveMutex is a shared_mutex with the recursive feature when locked non-shared mode.
     */
    class SharedRecursiveMutex : protected std::shared_mutex {
        std::atomic<std::thread::id> _owner;
        std::atomic<size_t> _count = 0;
    public:
        /**
         * @brief Locks for write (Blocks reads and writes from other threads).
         */
        void lock();

        /**
         * @brief Tries to lock for write (if successful, blocks read and writes from other threads).
         * 
         * @return `bool` Successfully locked?
         */
        bool try_lock();

        /**
         * @brief Unlocks write (allows other thread to lock or any read (shared) thread to read).
         */
        void unlock();

        using std::shared_mutex::lock_shared;
        using std::shared_mutex::try_lock_shared;
        using std::shared_mutex::unlock_shared;
        using std::shared_mutex::native_handle;
    };

    /**
     * @brief The reason this exists is because sometimes you have a speedy thread that rarely needs to stop so another thread can change its data.
     * 
     * Default mutex is obviously slower than a literal copy to a bool and return.
     * 
     * Using two bools one can be the last "fast thread" ack and the other the expected wait/continue.
     * 
     * You are supposed to check run() return. If true, you can go, else hold somehow (do not read or write whatever data you have).
     * 
     * The external thread should call lock() and unlock() like a mutex. The difference is that if run() is not being called, lock() will hold FOREVER.
     * 
     * As generally external threads are not the performance ones, the waiting is just a while() with sleep for 10 ms.
     */
    class FastOneWayMutex {
        volatile bool slave_ack = false;
        volatile bool request_stop = false;
    public:
        FastOneWayMutex() = default;
        FastOneWayMutex(FastOneWayMutex&&) = delete;
        FastOneWayMutex(const FastOneWayMutex&) = delete;
        void operator=(FastOneWayMutex&&) = delete;
        void operator=(const FastOneWayMutex&) = delete;

        /**
         * @brief Call this on the performance thread. If returns TRUE, you can do whatever you'd like (safe).
         * 
         * If false, wait somehow (testing the value in a reasonable time).
         * 
         * @return `bool` TRUE if you're good to go this time.
         */
        bool run();

        /**
         * @brief This is called by an external non-performance thread. Lock the resources and wait for the performance thread to "ack" automatically with this.
         * 
         * There's not much going on. It should feel like std::mutex's lock().
         */
        void lock();

        /**
         * @brief As for every lock() there's an unlock. This does NOT wait for ack, because it doesn't matter.
         * 
         * You're good to go after this.
         */
        void unlock();
    };

    /**
     * @brief Very useful lock() and unlock() done automatically for you.
     * 
     * This MUST NOT be called on the performance thread. This is for the external thread changing something in a random time once only!
     * 
     * The main performance thread (loop) should keep an eye on run()
     */
    class FastLockGuard {
        FastOneWayMutex& ref;
    public:
        FastLockGuard(FastLockGuard&&) = delete;
        FastLockGuard(const FastLockGuard&) = delete;
        void operator=(FastLockGuard&&) = delete;
        void operator=(const FastLockGuard&) = delete;

        FastLockGuard(FastOneWayMutex&);
        ~FastLockGuard();
    };
    
} // namespace Mutex
} // namespace Lunaris