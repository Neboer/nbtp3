#include "engine.h"
#include <cstddef>
#include <memory>

using namespace nbtp;

block::block(size_t init_size)
        : p((char *) malloc(init_size)), max_size(init_size), current_size(0) {}

void block::write_in(char *d, size_t prepared_size) {
    size_t require_new_size = current_size + prepared_size;
    if (max_size < require_new_size) {
        p.reset((char *) realloc(p.get(), require_new_size));
    }
    memcpy(p.get() + current_size, d, prepared_size);
}

block::~block() { p = nullptr; }


static size_t curl_write_cb(char *data, size_t count, size_t nmemb,
                            void *user_data) {
    auto *user_block = (block *) user_data;
    user_block->write_in(data, nmemb);
    return nmemb;
}

future<upload_result> Engine::add_task(chunk &&input) {
    auto *info = new information_pass_to_curl_handle{
            block(rough_size), promise<upload_result>{}, move(input)
    };
    promise<upload_result> promise_to_result{};
    CURL *t_handle = curl_easy_init(); // this handle.
    block download_result(rough_size);
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
    // run in a loop
    do {
        this_thread::sleep_for(1s);
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
                information_pass_to_curl_handle *info;
                curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &info);
                if (msg->data.result == CURLE_OK) { // if the transfer is successful, extract the result and report
                    info->promise_to_report_upload_result.set_value(
                            result_converter(move(info->download_data))
                    ); // upload_result isn't rely on download_data, so dd go die just be fine.
                    // after the upload is complete, the data will be cleaned with the function.
                } else { // a transfer is ERROR !
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