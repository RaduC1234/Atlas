#pragma once

#include <future>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ExecutorService {
public:
    ExecutorService(size_t threadCount);

    // do not move in cpp -> link error
    template<typename F, typename... Args>
    auto submit(F &&f, Args &&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using returnType = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<returnType()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<returnType> result = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (stop)
                throw std::runtime_error("Submit on stopped ExecutorService");

            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return result;
    }

    ~ExecutorService();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};
