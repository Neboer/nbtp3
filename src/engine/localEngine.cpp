// localEngine, an picture bed runs on localhost for test.
#include "localEngine.h"

std::string localEngine::result_extractor(dynamic_storage raw_result) {
    return {(char *) raw_result.p.get() + 10, 36};
}

void localEngine::define_curl(CURL *in_curl, packed_storage *input) {
    curl_easy_setopt(in_curl, CURLOPT_URL, "https://localhsot:8443/upload");
    curl_mime* form = curl_mime_init(in_curl);
    curl_mimepart* file_field = curl_mime_addpart(form);
    curl_mime_name(file_field, "fname");
    curl_mime_data(file_field,(char*) input->data.get(), input->size);
    curl_easy_setopt(in_curl, CURLOPT_MIMEPOST, form);
    curl_cache.insert({in_curl, form});
}

void localEngine::clean_up(CURL *in_curl) {
    curl_mime_free(curl_cache[in_curl]);
}

