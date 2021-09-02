#include "engine/localEngine.h"
#include <iostream>
#include <vector>

using namespace nbtp;

int main() {
    localEngine le{};
    std::vector<std::future<upload_result>> upload_result_list;
    for (int i = 0; i < 2; i++) {
        dynamic_storage c(2000);
        c.write_in('g', 2000);
        upload_result_list.push_back(le.add_task(c.seal(i)));
    }
    std::thread startThread(&localEngine::start_engine, &le);
    for (auto &i : upload_result_list) {
        i.wait();
        std::cout << i.get().link << std::endl;
    }
    le.startable = false;
    startThread.join();
}