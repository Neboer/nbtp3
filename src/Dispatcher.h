#pragma once

#include "../TQueue/TQueue.h"
#include "nbtp.h"
#include <functional>
#include <future>

using namespace nbtp;

// send data blocks to network. running inside a thread.
class Dispatcher {
  public:
    using engine_func = function<future<upload_result>(chunk)>;
    using callback_upload_func = function<void(upload_result)>;
    TQueue<chunk>* data_queue;
    int upload_thread_count;
    queue<future<upload_result>> upload_queue{};

    engine_func network_uploader;

    Dispatcher(TQueue<chunk>* dq, int utc, engine_func ef);

    upload_result operator()();

  private:
    void add_task();

    upload_result wait_and_return();
};
