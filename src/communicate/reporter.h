// as a server, report the status info to a client.
#include "nbtp.h"
#include <vector>
#include <list>
#include <functional>
#include "nbtp_protocol.pb.h"

using namespace nbtp;

enum uploading_status {
    WAITING = 1,
    UPLOADING = 2,
    COMPLETE = 3
};

constexpr int inform_status_seconds = 5;// report the status every xs. But this is only include the non-critical information.
// report need to be created after the tcp server accepts a connection. The server give it an interface to report data.
class reporter {
public:
    void report_chunk_progress(chunk_id cid, size_t upload_size);

    void start_chunk(chunk_id cid, size_t chunk_size);// start a chunk's transfer.
    void report_chunk_success(chunk_id cid, std::string result_key);

    void report_chunk_failure(chunk_id cid, std::string failure_information); // must be copy.

    explicit reporter(int total_chunk_count);

    void sync_message_list();

private:
    size_t current_total_upload = 0; // store the upload size.
    time_t transfer_start_time = 0;
    std::function<void(const std::string&)> send;// the send function.
    ServerChunkList main_message_list;// store all the ServerChunkStatus.
};