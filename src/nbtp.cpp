#include "nbtp.h"

using namespace nbtp;

// chunk is static. After it is create, it can only be moved and its length is fixed.
// what's more, chunk need to hold the only copy of the data(unique ptr),

chunk::chunk(id chunk_id, size_t chunk_size, unique_ptr<char> &&input_data)
    : data(move(input_data)), cid(chunk_id), size(chunk_size) {
}

chunk::~chunk() noexcept {
    data = nullptr;
}

chunk::chunk(chunk &&other) noexcept :
    cid(other.cid), size(other.size), data(move(other.data))
{
    other.data.release(); // let original block not to care about its data.
}