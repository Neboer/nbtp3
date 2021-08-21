#include "TQueue.h"

using clg = const lock_guard<mutex>;

template<typename T>
TQueue<T>::TQueue(size_t s) {
    max_size = s;
    no_more_input = false;
    unfinished_tasks = 0;
    lock = unique_lock<mutex>(mt);
    mt.unlock();
}


template<typename T>
void TQueue<T>::task_done() {
    clg guard(mt);
    auto unfinished = unfinished_tasks - 1;
    if (unfinished <= 0) {
        if (unfinished < 0) {
            throw runtime_error("too many task_done called.");
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
    if (queue_.size() == 0 && no_more_input) throw execution_over_exception{};
    not_empty.wait(lock, [this]() {
        return queue_.size();
    });
    T item = move(queue_.front());
    queue_.pop();
    not_full.notify_one();
    return item;
}
