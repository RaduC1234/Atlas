#pragma once
//#include "HTTPClientNetworkInstance.hpp"

class ClientNetworkManager {
public:
    static void init() {
       // networkInstance.init();
    }

    template<typename F, typename... Args>
    static auto submit(F &&f, Args &&... args) -> std::future<std::invoke_result_t<F, Args...>> ;

    static void shutdown() {
    }

private:
    //HTTPClientNetworkInstance networkInstance;
};
