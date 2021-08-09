#pragma once

#include "nbtp.h"
#include "TQueue/TQueue.h"
#include <future>
#include <functional>

using namespace nbtp;

// send data blocks to network. running inside a thread.
class Dispatcher {
public:
    using engine_func = function<future<upload_result>(chunk&&)>;
    TQueue<chunk> *data_queue;
    int upload_thread_count = 5;
    queue<future<upload_result>> upload_queue{};

    engine_func network_uploader;

    Dispatcher() = default;

    [[noreturn]] void start_loop();

    upload_result operator() ();
private:
    void add_task();

    upload_result wait_and_return();
};
