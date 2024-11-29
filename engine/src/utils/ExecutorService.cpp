#include "ExecutorService.hpp"

ExecutorService::ExecutorService(size_t threadCount): stop(false) {
    for (size_t i = 0; i < threadCount; ++i) {
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

template<typename F, typename ... Args>
auto ExecutorService::submit(F &&f, Args &&...args) -> std::future<std::invoke_result_t<F, Args...>> {
    using returnType = std::invoke_result_t<F, Args...>;

    auto task = std::make_shared<std::packaged_task<returnType()> >(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<returnType> result = task->get_future(); {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (stop)
            throw std::runtime_error("Submit on stopped ExecutorService");

        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return result;
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