#include "nbtp_protocol.pb.h"
#include "nbtp.h"

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    nbtp::ServerChunkStatus s{};
    s.set_uploading_status(1);
    s.set_cid(2);
    s.set_chunk_length(3);
    s.set_already_uploaded_size(4);
    s.set_failed_times(5);
    s.set_last_fail_reason("hello");
    s.set_result_key("what?");
    nbtp::ServerChunkList list{};
    list.add_status_list().
    nbtp::ServerReport r{};
    r.set_allocated_status_list(&list);
    std::string good = r.SerializeAsString();

    nbtp::ServerReport sr{};
    sr.ParseFromString(good);
    std::cout << sr.ServerReportMessage_case();
}