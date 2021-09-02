#include "engine.h"
#include <cstddef>

using namespace nbtp;

static size_t curl_write_cb(octet *data, size_t count, size_t nmemb, void *user_data) {
    auto *user_block = (dynamic_storage *) user_data;
    user_block->write_in(data, nmemb);
    return nmemb;
}

std::future<upload_result> Engine::add_task(packed_storage &&input) {
    auto *info = new information_pass_to_curl_handle{
            dynamic_storage(rough_size), std::promise<upload_result>{}, std::move(input)
    };
    std::promise<upload_result> promise_to_result{};
    CURL *t_handle = curl_easy_init(); // this handle.
    dynamic_storage download_result(rough_size);
    curl_easy_setopt(t_handle, CURLOPT_WRITEDATA, &info->download_data);
    curl_easy_setopt(t_handle, CURLOPT_WRITEFUNCTION, &curl_write_cb);
    curl_easy_setopt(t_handle, CURLOPT_PRIVATE, info); // when transfer is finished, resolve the promise.
    // object and resolve it with completed value.
    define_curl(t_handle, &info->data_need_to_be_sent);
    curl_multi_add_handle(mt_handle, t_handle);
    startable = true;
    return info->promise_to_report_upload_result.get_future();
}

Engine::Engine() {
    mt_handle = curl_multi_init();
}

void Engine::start_engine() {
    do {
        std::this_thread::sleep_for(std::chrono::seconds (1));
    } while (!startable);
    // ready for transmit.
    int running_handles, fds, queue_message_count;
    while (startable) { // loop until exit. When exit, the flag is false.
        curl_multi_perform(mt_handle, &running_handles);
        if (running_handles)
            curl_multi_poll(mt_handle, nullptr, 0, 1000, &fds);
        do {
            CURLMsg *msg = curl_multi_info_read(mt_handle, &queue_message_count);
            if (msg) { // if message is not empty then just keep reading.
                clean_up(msg->easy_handle); // when receive a handle, a transfer is end, then clean up its resource.
                information_pass_to_curl_handle *info;
                curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &info);
                if (msg->data.result == CURLE_OK) { // if the transfer is successful, extract the result and report
                    info->promise_to_report_upload_result.set_value(upload_result{
                            info->data_need_to_be_sent.cid, result_extractor(std::move(info->download_data)
                    )}); // upload_result isn't rely on download_data, so dd go die just be fine.
                    // after the upload is complete, the data will be cleaned with the function.
                } else { // a transfer is ERROR !
//                    Reporter::report_error(info->data_need_to_be_sent.cid, curl_easy_strerror(msg->data.result));
                    if (info->fail_times > max_failed_time_count) {
                        throw MaxTryExceedException{};
                    } else {
                        info->fail_times++;
                        info->download_data.current_size = 0; // this simple operation reset the chunk object.
                        curl_multi_remove_handle(mt_handle, msg->easy_handle);
                        curl_multi_add_handle(mt_handle, msg->easy_handle); // push the transfer info multi interface again.
                    }
                }
            }
        } while (queue_message_count > 0);
    }

}