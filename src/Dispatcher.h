#pragma once

#include "TQueue/TQueue.hpp"
#include "nbtp.h"
#include <functional>
#include <future>

using namespace nbtp;

// send data blocks to network. running inside a thread.
class Dispatcher {
public:
    using engine_func = std::function<std::future<upload_result>(packed_storage)>;
    using callback_upload_func = std::function<void(upload_result)>;
    TQueue<packed_storage> *data_queue;
    int upload_thread_count;
    std::queue<std::future<upload_result>> upload_queue{};

    engine_func network_uploader;

    Dispatcher(TQueue<packed_storage> *dq, int upload_limit, engine_func ef);

    upload_result operator()();

private:
    void add_task();

    upload_result wait_and_return();
};
