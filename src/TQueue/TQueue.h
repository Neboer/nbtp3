#pragma once

#include <queue>
#include <thread>
#include <condition_variable>
#include <cassert>
#include <stdexcept>
// a queue for threads sharing data.

class execution_over_exception : std::exception {};

template<typename T>
class TQueue {
public:
    size_t max_size;

    bool no_more_input;

    explicit TQueue(size_t s);

    TQueue(const TQueue &) = delete; // delete the copy constructor of TQueue

    void task_done();

    void join();

    void put(T item) noexcept;

    T get();

private:
    std::queue<T> queue_;
    std::mutex mt;
    std::unique_lock<std::mutex> lock;
    std::condition_variable not_empty;
    std::condition_variable not_full;
    std::condition_variable all_task_done;
    size_t unfinished_tasks;
};
