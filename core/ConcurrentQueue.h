#pragma once

#include <queue>
#include <mutex>

template <class T>
class ConcurrentQueue {
private:
    std::queue<T> _queue;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic_bool terminate {false};

public:
    ConcurrentQueue() {}
    ConcurrentQueue(const ConcurrentQueue&) = delete;
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

    T pop() {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [this] { return !_queue.empty() || terminate.load(); });

        if(terminate.load()) {
            throw std::runtime_error("Queue has been terminated");
        }

        T elem(std::move(_queue.front()));
        _queue.pop();
        return elem;
    }

    void push(T const& val) {
        {
            std::unique_lock<std::mutex> lk(mtx);
            _queue.push(val);
        }
        cv.notify_one();
    }

    bool isEmpty() {
        std::unique_lock<std::mutex> lk(mtx);
        return _queue.empty();
    }

    void terminate_queue() {
        std::unique_lock<std::mutex> lk(mtx);
        terminate.store(true);
        cv.notify_all();
    }
};







