#include "nbtp.h"

using namespace nbtp;

// chunk is static. After it is create, it can only be moved and its length is fixed.
// what's more, chunk need to hold the only copy of the data(unique ptr),

packed_storage::packed_storage(chunk_id chunk_id, size_t chunk_size, std::unique_ptr<octet> &&input_data, void *tag)
        : data(move(input_data)), cid(chunk_id), size(chunk_size), label(tag) {}

packed_storage::~packed_storage() noexcept {
    data = nullptr;
}

packed_storage::packed_storage(packed_storage &&other) noexcept:
        cid(other.cid), size(other.size), data(move(other.data)), label(other.label) {
    other.data.release(); // let original block not to care about its data.
}

dynamic_storage::dynamic_storage(size_t init_size)
        : p((octet *) malloc(init_size)), max_size(init_size), current_size(0) {}

void dynamic_storage::write_in(octet *d, size_t prepared_size) {
    size_t require_new_size = current_size + prepared_size;
    if (max_size < require_new_size) {
        p.reset((octet *) realloc(p.get(), require_new_size));
    }
    memcpy(p.get() + current_size, d, prepared_size);
    current_size = require_new_size;
}

void dynamic_storage::write_in(octet c, size_t write_times) {
    size_t require_new_size = current_size + write_times;
    if (max_size < require_new_size) {
        p.reset((octet *) realloc(p.get(), require_new_size));
    }
    for (size_t t = 0; t < write_times; t++) {
        *(p.get() + current_size + t) = c;
    }
    current_size += write_times;
}

dynamic_storage::~dynamic_storage() { p = nullptr; }

// After this operation, the original dynamic storage is invalid. Do not use it.
nbtp::packed_storage dynamic_storage::seal(chunk_id cid, void *tag) {
    return packed_storage{
            cid, current_size, std::unique_ptr<octet>(p.release()), tag
    };
}


