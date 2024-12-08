#pragma once

#include "Packet.hpp"

class IRequest {
};

class ClientNetworkManager {
public:
    ClientNetworkManager() = default;

    ~ClientNetworkManager() = default;

    static void init() {
        std::cout << "Network manager initialized." << std::endl;
    }

    template<typename T, typename... Args>
    void addRequestTemplate(Args &&... args) {
        static_assert(std::derived_from<T, IRequest>, "Class must derive from IRequest");
        static_assert(std::is_constructible_v<T, Args...>, "Class cannot be constructed with the provided arguments");

        requestInstance<T>() = std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    void sendRequestAsync(Args &&... args) {
        static_assert(std::derived_from<T, IRequest>, "Class must derive from IRequest");

        auto &instance = requestInstance<T>();
        if (!instance) {
            throw std::invalid_argument("No request template found for the given class type");
        }

        Packet packet;
        packet.authToken = loginToken;
        packet.UUID = Uuid::randomUUID().getMostSignificantBits();
        packet.payload = JsonData{};
        instance->onNewRequest(packet, {std::forward<Args>(args)...});

        std::async(std::launch::async, [instance, packet]() mutable {
            try {
                auto response = cpr::Post(
                    cpr::Url{"http://localhost:8080/" + std::to_string(packet.UUID)},
                    cpr::Header{{"Content-Type", "application/JsonData"}},
                    cpr::Body{packet.payload.dump()}
                );

                if (response.status_code == 200) {
                    JsonData responseData = JsonData::parse(response.text);
                    packet.payload = responseData;
                    instance->onAnswer(packet);
                } else {
                    std::cerr << "HTTP Request failed with status: " << response.status_code << std::endl;
                }
            } catch (const std::exception &e) {
                std::cerr << "Exception in async request: " << e.what() << std::endl;
            }
        });
    }

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

    static void shutdown() {
        std::cout << "Network manager shutting down." << std::endl;
    }

private:
    template<typename Class>
    static std::shared_ptr<IRequest> &requestInstance() {
        static std::shared_ptr<IRequest> instance;
        return instance;
    }

    static inline uint64_t loginToken{0};
};
