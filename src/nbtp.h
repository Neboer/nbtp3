#pragma once
#include <string>
#include <array>
#include <memory>


using namespace std;

using id = int;
using octet = unsigned char;

constexpr size_t default_chunk_size = 4 * 1024 * 1024; // 4MB
constexpr size_t d_shape_width = 1024;
constexpr size_t d_shape_height = 1024;

namespace nbtp {

    constexpr int max_failed_time_count = 3;

    struct packed_storage {
        id cid;
        size_t size;
        void* label;
        unique_ptr<octet> data;

        packed_storage(id chunk_id, size_t chunk_size, unique_ptr<octet> &&input_data, void* tag=nullptr);

        ~packed_storage() noexcept;

        packed_storage(const packed_storage &) = delete; // uncopyable object.

        packed_storage(packed_storage &&other) noexcept;
    };

    struct upload_result {
        id cid = -1;
        string link; // link to get the uploaded image.
    };
}

struct dynamic_storage { // block is dynamic. But chunk is static.
    size_t current_size;
    size_t max_size;
    unique_ptr<octet> p;

    explicit dynamic_storage(size_t init_size);

    void write_in(octet *d, size_t prepared_size);

    void write_in(octet c, size_t write_times);

    nbtp::packed_storage seal(id cid, void* tag = nullptr); // make current dynamic storage static and seal it permanently

    ~dynamic_storage();

    dynamic_storage(const dynamic_storage &) = delete;

    dynamic_storage(dynamic_storage &&) noexcept = default;
};