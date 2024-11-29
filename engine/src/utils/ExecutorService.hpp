#pragma once

#include "core/Core.hpp"

class ExecutorService {
public:
    ExecutorService(size_t threadCount);

    template<typename F, typename... Args>
    auto submit(F &&f, Args &&... args) -> std::future<std::invoke_result_t<F, Args...>> ;

    ~ExecutorService();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()> > tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};


