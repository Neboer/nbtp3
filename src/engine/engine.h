#pragma once

#include "../nbtp.h"
#include <curl/curl.h>
#include <future>
#include <stdexcept>

using namespace nbtp;



class MaxTryExceedException : exception {};

struct information_pass_to_curl_handle {
    dynamic_storage download_data;
    promise<upload_result> promise_to_report_upload_result{};
    packed_storage data_need_to_be_sent;
    short fail_times = 0;
};


class Engine {
public:
    CURLM *mt_handle{};

    bool startable = false; // no mission, cannot start.

    Engine();

    static size_t rough_size;

    future<upload_result> add_task(packed_storage &&input);

    void start_engine();

private:
    virtual upload_result result_converter(dynamic_storage raw_result) = 0; // convert the server response to a result

    virtual void define_curl(CURL *in_curl, packed_storage *input) = 0; // set url and data for input CURL. Dont care about input object.
};
