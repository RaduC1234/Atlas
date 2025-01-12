#pragma once

#include "ClientRequest.hpp"
#include "Packet.hpp"
#include <cpr/cpr.h>

class ClientNetworkService {
public:
    ClientNetworkService() = default;

    ~ClientNetworkService() = default;

    static void init(const std::string &remoteHost) {
        serverUrl = remoteHost;
        AT_INFO("Server URL is: {0}", serverUrl);
    }

    static bool reg(const std::string &username, const std::string &password) {
        JsonData requestBody = {
            {"username", username},
            {"password", password}
        };

        auto response = cpr::Get(
            cpr::Url{"http://localhost:8080/register"},
            cpr::Header{{"Content-Type", "application/Json"}},
            cpr::Body{requestBody.dump()}
        );


        JsonData data;
        TRY_CATCH(data = JsonData::parse(response.text);,return false;);

        if (response.status_code == 200) {
            return data["requestStatus"].get<bool>();
        }

        throw std::runtime_error(data["message"].get<std::string>());
    }

    static bool login(const std::string &username, const std::string &password) {
        JsonData requestBody = {
            {"username", username},
            {"password", password}
        };

        auto response = cpr::Get(
            cpr::Url{"http://localhost:8080/login"},
            cpr::Header{{"Content-Type", "application/JsonData"}},
            cpr::Body{requestBody.dump()}
        );

        try {
            if (response.status_code == 200) {
                JsonData data = JsonData::parse(response.text);
                loginToken = data["authToken"].get<uint64_t>();
                return data["requestStatus"].get<bool>();
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
