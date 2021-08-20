#pragma once

#include "../nbtp.h"
#include <curl/curl.h>
#include <future>
#include <stdexcept>

using namespace nbtp;

struct block { // block is dynamic. But chunk is static.
    size_t current_size;
    size_t max_size;
    unique_ptr<char> p;

    explicit block(size_t init_size);

    void write_in(char *d, size_t prepared_size);

    ~block();

    block(const block &) = delete;

    block(block &&) noexcept = default;
};

class MaxTryExceedException : exception {};

struct information_pass_to_curl_handle {
    block download_data;
    promise<upload_result> promise_to_report_upload_result{};
    chunk data_need_to_be_sent;
    short fail_times = 0;
};


class Engine {
public:
    CURLM *mt_handle{};

    bool startable = false; // no mission, cannot start.

    Engine();

    static size_t rough_size;

    future<upload_result> add_task(chunk &&input);

    void start_engine();

private:
    virtual upload_result result_converter(block raw_result) = 0; // convert the server response to a result

    virtual void define_curl(CURL *in_curl, chunk *input) = 0; // set url and data for input CURL. Dont care about input object.
};
