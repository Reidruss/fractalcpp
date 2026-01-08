#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

using namespace std;


// Need to add documentation comments to this.
class ThreadPool
{
    public:
        ThreadPool(size_t threads = thread::hardware_concurrency());
        ~ThreadPool();

        void enqueue(function<void()> task);

    private:
        vector<thread> threads_;
        queue<function<void()>> tasks_;

        mutex q_lock_;
        condition_variable condvar_;
        bool stop_;
};

#endif // THREADPOOL_HPP