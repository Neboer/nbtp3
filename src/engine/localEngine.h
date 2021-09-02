#pragma once

#include "engine.h"
#include <map>

class localEngine : public Engine {
public:
    const static size_t rough_size = 1024;
private:
    std::map<CURL *, curl_mime *> curl_cache{};

    std::string result_extractor(dynamic_storage raw_result) override;

    void define_curl(CURL *in_curl, packed_storage *input) override;

    void clean_up(CURL *curl_handle) override;
};
