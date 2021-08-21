// downloader download a file from another location with single thread.
#include "nbtp.h"
#include <curl/curl.h>
#include "TQueue/TQueue.h"

using namespace nbtp;

class Downloader {
public:
    CURL* s_handle;

    id id_counter;

    dynamic_storage operator ()();

    Downloader(const std::string &url);

private:
    TQueue<packed_storage> cache_chunks;
};