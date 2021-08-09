#include "Dispatcher.h"


void Dispatcher::add_task() {
    auto c = data_queue->get();
    auto r = network_uploader(move(c));
    upload_queue.push(move(r));
}

upload_result Dispatcher::wait_and_return() {
    auto &&t = upload_queue.front();
    t.wait();
    auto m = move(t.get());
    upload_queue.pop();
    return m;
}

void Dispatcher::start_loop() {
    while (true) {
        try {
            if (upload_queue.size() < upload_thread_count) add_task();
            else {
                auto &&f = upload_queue.front();

            }
        } catch (execution_over_exception) {
            return;
        }

    }
}

// get a future from list and push it into waiting pool until waiting queue is full.
// then wait for a future object to complete, yield it and get a new future from list, push it into waiting queue.
upload_result Dispatcher::operator()() {
    if (data_queue->no_more_input) {
        if (!upload_queue.empty()) wait_and_return();
        else {
            throw execution_over_exception{};
        }
    } else {
        try {
            while (upload_queue.size() < upload_thread_count) add_task();
            auto m = move(wait_and_return());
            add_task();
            return m;
        } catch (execution_over_exception &e) {
            // no new task, just complete the rest.
            return wait_and_return();
        }
    }
}


