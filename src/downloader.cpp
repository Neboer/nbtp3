#include "downloader.h"
#include <functional>
// this is single thread downloader.
// the callback function

// init buffer muxt size at default_chunk_size
id Downloader::get_next_id() {
    static id i = 0;
    return i++;
}

last_chunk_status Downloader::serialize_data(octet *data, size_t length) {
    // the last_chunk_status cannot be empty. Because we don't alloc unused space.
    size_t left_room = last_data->max_size - last_data->current_size;
    if (left_room > length) {
        last_data->write_in(data, length);
        return HALF;
    } else if (left_room == length) {
        last_data->write_in(data, length);
        // don't pack it yet!
        // you don't know whether this is the last chunk and the data will be tagged or not!
        return FULL;
    } else {
        last_data->write_in(data, left_room);
        cache_chunks.put(last_data->seal(get_next_id()));
        last_data = new dynamic_storage(default_chunk_size);
        return serialize_data(data + left_room, length - left_room);
    }
}

struct data_pass_to_cb {
    std::function<last_chunk_status(octet *, size_t)> serialize_data_func;
    last_chunk_status *last_status_report;
};

// function trys to write income data to buffer. if buffer is full, then it will be sealed to cache queue.
static size_t curl_write_cb(octet *data, size_t i, size_t nmemb, void *user_data) {
    auto cb_data = (data_pass_to_cb *) user_data;
    *(cb_data->last_status_report) = cb_data->serialize_data_func(data, nmemb);
    return nmemb;
}

Downloader::Downloader(const std::string &url) : cache_chunks(5), last_status(EMPTY),
                                                 last_data(new dynamic_storage(default_chunk_size)) {
    s_handle = curl_easy_init();
    auto communication_data = new data_pass_to_cb{
            std::bind(&Downloader::serialize_data, this, std::placeholders::_1, std::placeholders::_2),
            &last_status};
    curl_easy_setopt(s_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(s_handle, CURLOPT_WRITEFUNCTION, &curl_write_cb);
    curl_easy_setopt(s_handle, CURLOPT_WRITEDATA, communication_data);
}

void Downloader::start_transfer() {
    curl_easy_perform(s_handle);
    cache_chunks.no_more_input = true;
    auto *last_size_of_block = new size_t(last_data->current_size);
    last_data->write_in((octet) 0, default_chunk_size - last_data->current_size);
    cache_chunks.put(last_data->seal(get_next_id(), (void *) last_size_of_block));
}

packed_storage Downloader::operator()() {
    return cache_chunks.get();// if the transfer is over, throw over exception.
}

