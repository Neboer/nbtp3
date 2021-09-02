#pragma once

#include <queue>
#include <thread>
#include <condition_variable>
#include <cassert>
#include <stdexcept>
// a queue for threads sharing data.

class execution_over_exception : std::exception {};
using clg = const std::lock_guard<std::mutex>;

template<typename T>
class TQueue {
public:
    size_t max_size;

    bool no_more_input;

    explicit TQueue(size_t s) {
        max_size = s;
        no_more_input = false;
        unfinished_tasks = 0;
        lock = std::unique_lock<std::mutex>(mt);
        mt.unlock();
    };

    TQueue(const TQueue &) = delete; // delete the copy constructor of TQueue

    void task_done() {
        clg guard(mt);
        auto unfinished = unfinished_tasks - 1;
        if (unfinished <= 0) {
            if (unfinished < 0) {
                throw std::runtime_error("too many task_done called.");
            }
            all_task_done.notify_all();
        }
        unfinished_tasks = unfinished;
    };

    void join() {
        clg guard(mt);
        all_task_done.wait(lock, [this]() {
            return !unfinished_tasks;
        });
    };

    void put(T &&item) noexcept {
        clg guard(mt);
        not_full.wait(lock, [this]() {
            return queue_.size() < max_size;
        });
        queue_.push(std::move(item));
        unfinished_tasks++;
        not_empty.notify_one();
    };

    T get() {
        clg guard(mt);
        if (queue_.empty() && no_more_input) throw execution_over_exception{};
        not_empty.wait(lock, [this]() {
            return queue_.size();
        });
        T item = std::move(queue_.front());
        queue_.pop();
        not_full.notify_one();
        return item;
    };

private:
    std::queue<T> queue_;
    std::mutex mt;
    std::unique_lock<std::mutex> lock;
    std::condition_variable not_empty;
    std::condition_variable not_full;
    std::condition_variable all_task_done;
    size_t unfinished_tasks;
};
