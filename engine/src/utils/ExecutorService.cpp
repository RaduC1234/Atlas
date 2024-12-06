#include "ExecutorService.hpp"

ExecutorService::ExecutorService(size_t threadCount): stop(false) {
    for (std::size_t i = 0; i < threadCount; i++) {
        workers.emplace_back([this]() {
            while (true) {

                std::function<void()> task; {

                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    this->condition.wait(lock, [this]() {
                        return this->stop || !this->tasks.empty();
                    });

                    if (this->stop && this->tasks.empty())
                        return;

                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
        });
    }
}

ExecutorService::~ExecutorService() { {
    std::unique_lock<std::mutex> lock(queueMutex);
    stop = true;
}
    condition.notify_all();
    for (std::thread &worker: workers) {
        if (worker.joinable())
            worker.join();
    }
}