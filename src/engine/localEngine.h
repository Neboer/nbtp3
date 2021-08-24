#pragma once

#include "engine.h"
#include <map>

class localEngine : Engine {
private:
    std::map<CURL *, curl_mime *> curl_cache{};

    std::string result_extractor(dynamic_storage raw_result) override;

    void define_curl(CURL *in_curl, packed_storage *input) override;

    void clean_up(CURL *curl_handle) override;
};
