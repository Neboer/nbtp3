// downloader download a file from another location with single thread.
#include "nbtp.h"
#include <curl/curl.h>
#include "TQueue/TQueue.h"

using namespace nbtp;

enum last_chunk_status {
    FULL,
    HALF,
    EMPTY
};

struct data_pass_to_cb;

class Downloader {
public:
    CURL* s_handle;

    static id get_next_id();

    packed_storage operator ()();

    explicit Downloader(const std::string &url);

    void start_transfer();// should be run in another thread!

private:
    TQueue<packed_storage> cache_chunks;

    last_chunk_status last_status;

    dynamic_storage* last_data;

    last_chunk_status serialize_data(octet* data, size_t length);
};