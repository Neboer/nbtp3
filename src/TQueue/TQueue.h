#pragma once

#include <queue>
#include <thread>
#include <condition_variable>
#include <cassert>
#include <stdexcept>
// a queue for threads sharing data.

using namespace std;

class execution_over_exception : exception {};

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
    queue<T> queue_;
    mutex mt;
    unique_lock<mutex> lock;
    condition_variable not_empty;
    condition_variable not_full;
    condition_variable all_task_done;
    size_t unfinished_tasks;
};

