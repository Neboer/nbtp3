#include "reporter.h"

#define GET_MESSAGE auto* message = main_message_list.mutable_status_list(cid - 1);
#define SEND_MESSAGE send(message->SerializeAsString());

reporter::reporter(int total_chunk_count) : main_message_list() {
    for (int i = 1; i < total_chunk_count; i++) {
        auto *message = main_message_list.add_status_list();
        message->set_cid(i);
        message->set_uploading_status(WAITING);
        message->set_chunk_length(0);
        message->set_already_uploaded_size(0);
        message->set_failed_times(0);
        message->set_last_fail_reason({});
        message->set_result_key({});
    }
}

void reporter::report_chunk_progress(chunk_id cid, size_t upload_size) {
    GET_MESSAGE;
    message->set_already_uploaded_size(upload_size);
}

void reporter::start_chunk(chunk_id cid, size_t chunk_size) {
    GET_MESSAGE;
    message->set_chunk_length(chunk_size);
    message->set_uploading_status(UPLOADING);
    SEND_MESSAGE;
}

void reporter::report_chunk_success(chunk_id cid, std::string result_key) {
    GET_MESSAGE;
    message->set_uploading_status(COMPLETE);
    message->set_result_key(result_key);
    message->set_already_uploaded_size(message->chunk_length());
    SEND_MESSAGE;
}

void reporter::report_chunk_failure(chunk_id cid, std::string failure_information) {
    GET_MESSAGE;
    message->set_failed_times(message->failed_times() + 1);
    message->set_last_fail_reason(failure_information);
    message->set_already_uploaded_size(0);
    SEND_MESSAGE;
}

void reporter::sync_message_list() {
    static std::string buffer{};
    main_message_list.SerializeToString(&buffer);
    send(buffer);
}
