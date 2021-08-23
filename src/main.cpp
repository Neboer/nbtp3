#include "nbtp.h"
#include "Dispatcher.h"
#include "downloader.h"
#include "engine/engine.h"
#include "lodepng/lodepng.h"

using namespace nbtp;

int main() {
    curl_global_init(CURL_GLOBAL_ALL);
}