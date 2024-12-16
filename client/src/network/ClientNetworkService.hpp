#pragma once

#include "ClientRequest.hpp"
#include "Packet.hpp"
#include <cpr/cpr.h>

class ClientNetworkService {
public:
    ClientNetworkService() = default;

    ~ClientNetworkService() = default;

    static void init(const std::string& remoteHost) {
        serverUrl = remoteHost;
        AT_INFO("Server URL is: {0}", serverUrl);
    }

    template<typename T, typename... Args>
    static void addRequestTemplate(Args &&... args) ;

    template<typename T>
    static void sendRequestAsync();


    static bool reg(const std::string &username, const std::string &password) {
        JsonData requestBody = {
            {"username", username},
            {"password", password}
        };

        auto response = cpr::Post(
            cpr::Url{"http://localhost:8080/register"},
            cpr::Header{{"Content-Type", "application/Json"}},
            cpr::Body{requestBody.dump()}
        );

        try {
            if (response.status_code == 200) {
                JsonData data = JsonData::parse(response.text);
                return data["requestStatus"].get<bool>();
            }
        } catch (const std::exception &e) {
            std::cerr << "Error in registration request: " << e.what() << std::endl;
        }

        return false;
    }

    static bool login(const std::string &username, const std::string &password) {
        JsonData requestBody = {
            {"username", username},
            {"password", password}
        };

        auto response = cpr::Post(
            cpr::Url{"http://localhost:8080/login"},
            cpr::Header{{"Content-Type", "application/JsonData"}},
            cpr::Body{requestBody.dump()}
        );

        try {
            if (response.status_code == 200) {
                JsonData data = JsonData::parse(response.text);
                return data["success"].get<bool>();
            }
        } catch (const std::exception &e) {
            std::cerr << "Error in login request: " << e.what() << std::endl;
        }

        return false;
    }

private:
    template<typename Class>
    static std::shared_ptr<ClientRequest> &requestInstance() {
        static std::shared_ptr<ClientRequest> instance;
        return instance;
    }

    static inline uint64_t loginToken{0};
    static inline std::string serverUrl{};
};
