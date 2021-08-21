#include "downloader.h"
// this is single thread downloader.
// the callback function

struct data_pass_to_cb {
    TQueue<packed_storage> *write_target_queue;
    id *next_id;
    dynamic_storage *buffer_wait_for_seal;
};

// function trys to write income data to buffer. if buffer is full, then it will be sealed to cache queue.
static size_t curl_write_cb(char *data, size_t i, size_t nmemb, void *user_data) {
    auto *cb_data = (data_pass_to_cb *) user_data;
    // three condition: buffer is large enough / buffer is just enough / buffer is not enough.
    size_t free_size = cb_data->buffer_wait_for_seal->max_size - cb_data->buffer_wait_for_seal->current_size;
    if (free_size > nmemb) cb_data->buffer_wait_for_seal->write_in(data, nmemb); // if not full, just make it full.
    else {
        // if not enough, keep write into it.
        cb_data->write_target_queue->put(cb_data->buffer_wait_for_seal->seal(*cb_data->next_id));
        (*cb_data->next_id)++;
        int chunks_count_need_to_create_then = (nmemb - free_size) / default_chunk_size + 1;
        for (int c_number = 0; c_number <= chunks_count_need_to_create_then; c_number++) {
            dynamic_storage current_data(default_chunk_size);
            if (c_number != chunks_count_need_to_create_then) {
                // this is not the last chunk
                current_data.write_in(data + free_size + c_number * default_chunk_size, default_chunk_size);
            }
            cb_data->write_target_queue->put(current_data.seal(*cb_data->next_id));
            (*cb_data->next_id)++;
        }
    }
    return nmemb;
}

Downloader::Downloader(const std::string &url) : cache_chunks(5), id_counter(0) {
    s_handle = curl_easy_init();
    auto *cb_communicate = new data_pass_to_cb{&cache_chunks, &id_counter, new dynamic_storage(default_chunk_size)};
    curl_easy_setopt(s_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(s_handle, CURLOPT_WRITEFUNCTION, &curl_write_cb);
    curl_easy_setopt(s_handle, CURLOPT_WRITEDATA, cb_communicate);
}

dynamic_storage Downloader::operator()() {

}