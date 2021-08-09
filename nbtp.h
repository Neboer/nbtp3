#pragma once
#include "lodepng/lodepng.h"
#include "curlpp/cURLpp.hpp"
#include <array>

using namespace std;

using id = int;

namespace nbtp {
    constexpr size_t chunk_size = 0x4FFFFF; // 4MB
    struct chunk {
        id cid;
        size_t size;
        array<std::byte, chunk_size> data{};
        explicit chunk(id i, size_t s = chunk_size) {
            cid = i;
            size = s;
        }
        chunk(const chunk&) = delete; // uncopyable object.
    };

    struct upload_result {
        id cid = -1;
        string link; // link to get the uploaded image.
        CURLcode code; // upload status
    };
}

