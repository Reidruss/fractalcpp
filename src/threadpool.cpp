#include <thread>

#include "threadpool.hpp"

ThreadPool::ThreadPool(size_t threads)
    : stop_(false)
{
    for (size_t i = 0; i < threads; ++i)
    {
        threads_.emplace_back([this]
        {
            while (1)
            {
                function<void()> task;

                {
                    unique_lock<mutex> lock (q_lock_);

                    condvar_.wait(lock, [this]
                    {
                        return !tasks_.empty() || stop_;
                    });

                    if (stop_ && tasks_.empty())
                    {
                        return;
                    }

                    task = move(tasks_.front());
                    tasks_.pop();
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    {
        unique_lock<mutex> lock(q_lock_);
        stop_ = true;
    }
    condvar_.notify_all();
    for (auto& thread : threads_)
    {
        thread.join();
    }
}

void ThreadPool::enqueue(function<void()> task)
{
    {
        unique_lock<mutex> lock(q_lock_);
        tasks_.emplace(move(task));
    }
    condvar_.notify_one();
}