// Copyright © 2017 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://vinecopulib.github.io/vinecopulib/.

#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <future>
#include <condition_variable>
#include <memory>

#ifdef INTERFACED_FROM_R
    // import interruptible ThreadPool from RcppThread
    #include <vinecopulib/misc/tools_interface.hpp>
#endif

namespace tools_parallel {

#ifndef INTERFACED_FROM_R
    //! Implemenation of the thread pool pattern similar to RcppThread.
    class ThreadPool
    {
    public:
        
        ThreadPool() = default;
        
        ThreadPool(ThreadPool &&) = delete;
        
        ThreadPool(const ThreadPool &) = delete;
        
        //! constructs a thread pool with `nThreads` threads.
        //! @param nThreads number of threads to create.
        ThreadPool(size_t nThreads) : stopped_(false)
        {
            for (size_t t = 0; t < nThreads; t++) {
                pool_.emplace_back([this] {
                    // observe thread pool as long there are jobs or pool has 
                    // been stopped
                    while (!stopped_ | !jobs_.empty()) {
                        std::function<void()> job;
                        {
                            // thread must hold the lock while modifying shared
                            // variables
                            std::unique_lock <std::mutex> lk(m_);
                            
                            // wait for new job or stop signal
                            cv_.wait(lk, [this] {
                                return stopped_ || !jobs_.empty();
                            });
                            
                            // check if there are any jobs left in the queue
                            if (jobs_.empty())
                            continue;
                            
                            // take job from the queue
                            job = std::move(jobs_.front());
                            jobs_.pop();
                        }
                        
                        // execute job
                        job();
                    }
                });
            }
        }
        
        ~ThreadPool()
        {
            join();
        }
        
        // assignment operators
        ThreadPool &operator=(const ThreadPool &) = delete;
        
        ThreadPool &operator=(ThreadPool &&other) = default;
        
        //! pushes new jobs to the thread pool.
        //! @param f a function taking an arbitrary number of arguments.
        //! @param args a comma-seperated list of the other arguments that shall
        //!   be passed to `f`.
        //! @return an `std::shared_future`, where the user can get the result 
        //!   and rethrow the catched exceptions.
        template<class F, class... Args>
        auto push(F &&f, Args &&... args) -> std::future<decltype(f(args...))>
        {
            // create packaged task on the heap to avoid stack overlows.
            auto job =
                std::make_shared<std::packaged_task<decltype(f(args...))()>> (
                    [&f, args...] { return f(args...); }
                );
                
            // add job to the queue
            {
                std::unique_lock <std::mutex> lk(m_);
                if (stopped_)
                throw std::runtime_error("cannot push to stopped thread pool");
                jobs_.emplace([job]() { (*job)(); });
            }
            
            // signal a waiting worker that there's a new job
            cv_.notify_one();
            
            // return future result of the job
            return job->get_future();
        }
            
        //! waits for all jobs to finish and joins all threads.
        void join()
        {
            // signal all threads to stop
            {
                std::unique_lock <std::mutex> lk(m_);
                stopped_ = true;
            }
            cv_.notify_all();
            
            // join threads if not done already
            if (pool_[0].joinable()) {
                for (auto &worker : pool_) {
                    worker.join();
                }
            }
        }
            
    private:
        std::vector <std::thread> pool_;            // worker threads
        std::queue <std::function<void()>> jobs_;  // the task queue
        
        // variables for synchronization between workers
        std::mutex m_;
        std::condition_variable cv_;
        bool stopped_;
    };
#endif

//! maps a function on a list of items, possibly running tasks in parallel.
//! @param f function to be mapped.
//! @param items an objects containing the items on which `f` shall be mapped;
//!     must allow for `auto` loops (i.e., `std::begin(I)`/`std::end(I)` must be 
//!     defined).
//! @param num_threads the number of concurrent threads to use in the pool.
template<class F, class I>
void map_on_pool(F &&f, I &&items, size_t num_threads)
{
    if (num_threads <= 1) {
        for (const auto &item : items) {
            f(item);
        }
    } else {
        ThreadPool pool(num_threads);
        for (const auto &item : items) {
            pool.push(f, item);
        }
        pool.join();
    }
}

}
