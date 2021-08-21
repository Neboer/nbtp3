#include "nbtp.h"

using namespace nbtp;

// chunk is static. After it is create, it can only be moved and its length is fixed.
// what's more, chunk need to hold the only copy of the data(unique ptr),

packed_storage::packed_storage(id chunk_id, size_t chunk_size, unique_ptr<char> &&input_data)
        : data(move(input_data)), cid(chunk_id), size(chunk_size) {
}

packed_storage::~packed_storage() noexcept {
    data = nullptr;
}

packed_storage::packed_storage(packed_storage &&other) noexcept:
        cid(other.cid), size(other.size), data(move(other.data)) {
    other.data.release(); // let original block not to care about its data.
}

dynamic_storage::dynamic_storage(size_t init_size)
        : p((char *) malloc(init_size)), max_size(init_size), current_size(0) {}

void dynamic_storage::write_in(char *d, size_t prepared_size) {
    size_t require_new_size = current_size + prepared_size;
    if (max_size < require_new_size) {
        p.reset((char *) realloc(p.get(), require_new_size));
    }
    memcpy(p.get() + current_size, d, prepared_size);
}

dynamic_storage::~dynamic_storage() { p = nullptr; }

// After this operation, the original dynamic storage is invalid. Do not use it.
nbtp::packed_storage dynamic_storage::seal(id cid) {
    return packed_storage{
            cid, current_size, unique_ptr<char>(p.release())
    };
}