#include "Dispatcher.h"
#include "engine/localEngine.h"

using namespace nbtp;
void q_writer(TQueue<packed_storage> *tq) {
    for (int i = 0; i < 10; ++i) {
        tq->put(dynamic_storage(10000).seal(i));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    TQueue<packed_storage> GT(5);
    std::thread t1(q_writer, &GT);

    localEngine le{};
    Dispatcher ds(&GT, 5, std::bind(&localEngine::add_task, &le, std::placeholders::_1));
    std::thread t2(&localEngine::start_engine, &le);
}