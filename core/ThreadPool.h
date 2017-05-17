#pragma once

#include <thread>
#include <vector>
#include <deque>

class ThreadPool {

    class Worker  {
    public:
        explicit Worker(ThreadPool* _master) : master(_master) { }
        void operator()() {
            std::function<void()> task;

            while(true) {
                {
                    std::unique_lock<std::mutex> lk(master->mtx);
                    master->cv.wait(lk, [&] () { return !master->tasks.empty() || master->terminate.load(); });

                    if(master->terminate.load()) {
                        return;
                    }

                    task = master->tasks.front();
                    master->tasks.pop_front();
                }

                task();
            }
        }

    private:
        ThreadPool* master;
    };

public:
    ThreadPool(size_t);
    template<typename F> void enqueue(F f);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    friend class Worker;

    std::vector<std::thread> workers;
    std::deque<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic_bool terminate {false};
};


ThreadPool::ThreadPool(size_t number_of_threads) {
    for (size_t i = 0; i < number_of_threads; ++i) {
        workers.push_back(std::thread(Worker(this)));
    }
}

ThreadPool::~ThreadPool() {
    terminate.store(true);
    cv.notify_all();
    for(auto& t : workers) t.join();
}

template<typename F>
void ThreadPool::enqueue(F f) {
    {
        std::unique_lock<std::mutex> lk(mtx);
        tasks.push_back(std::function<void()>(f));
    }

    cv.notify_one();
}