#include "TQueue.h"

using clg = const std::lock_guard<std::mutex>;

template<typename T>
TQueue<T>::TQueue(size_t s) {
    max_size = s;
    no_more_input = false;
    unfinished_tasks = 0;
    lock = std::unique_lock<std::mutex>(mt);
    mt.unlock();
}


template<typename T>
void TQueue<T>::task_done() {
    clg guard(mt);
    auto unfinished = unfinished_tasks - 1;
    if (unfinished <= 0) {
        if (unfinished < 0) {
            throw std::runtime_error("too many task_done called.");
        }
        all_task_done.notify_all();
    }
    unfinished_tasks = unfinished;
}

template<typename T>
void TQueue<T>::join() {
    clg guard(mt);
    all_task_done.wait(lock, [this]() {
        return !unfinished_tasks;
    });
}

template<typename T>
void TQueue<T>::put(T item) noexcept {
    clg guard(mt);
    not_full.wait(lock, [this]() {
        return queue_.size() < max_size;
    });
    queue_.push(item);
    unfinished_tasks++;
    not_empty.notify_one();
}

template<typename T>
T TQueue<T>::get() {
    clg guard(mt);
    if (queue_.empty() && no_more_input) throw execution_over_exception{};
    not_empty.wait(lock, [this]() {
        return queue_.size();
    });
    T item = std::move(queue_.front());
    queue_.pop();
    not_full.notify_one();
    return item;
}
