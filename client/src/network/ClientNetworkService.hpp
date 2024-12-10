#pragma once

#include "ClientRequest.hpp"
#include "Packet.hpp"

class ClientNetworkService {
public:
    ClientNetworkService() = default;

    ~ClientNetworkService() = default;

    static void init(const std::string& remoteHost) {
        serverUrl = remoteHost;
        AT_INFO("Server URL is: {0}", serverUrl);
    }

    template<typename T, typename... Args>
    static void addRequestTemplate(Args &&... args) {
        static_assert(std::derived_from<T, ClientRequest>, "Class must derive from IRequest");
        static_assert(std::is_constructible_v<T, Args...>, "Class cannot be constructed with the provided arguments");

        requestInstance<T>() = std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    static void sendRequestAsync(const JsonData &payload) {
        static_assert(std::derived_from<T, ClientRequest> || std::is_same_v<T, ClientRequest>,
            "Class must derive from IClientRequest or be IClientRequest itself.");


        auto instance = requestInstance<T>();
        if (!instance) {
            throw std::invalid_argument("No request template found for the given class type");
        }

        Packet packet;
        packet.authToken = loginToken;
        packet.UUID = Uuid::randomUUID().getMostSignificantBits();
        packet.payload = payload;

        instance->onNewRequest(packet);

        JsonData serializedPacket = packet;

        std::async(std::launch::async, [instance, serializedPacket]() mutable {
            try {
                auto response = cpr::Post(
                    cpr::Url{"http://localhost:8080/"},
                    cpr::Header{{"Content-Type", "application/json"}},
                    cpr::Body{serializedPacket.dump()}
                );

                if (response.status_code == HttpStatus::OK) {
                    // Deserialize response JSON into Packet
                    JsonData responseData = JsonData::parse(response.text);
                    Packet responsePacket = responseData.get<Packet>(); // Assumes `from_json` is defined
                    instance->triggerOnReceive(responsePacket); // Trigger the onReceive callback
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

private:
    template<typename Class>
    static std::shared_ptr<ClientRequest> &requestInstance() {
        static std::shared_ptr<ClientRequest> instance;
        return instance;
    }

    static inline uint64_t loginToken{0};
    static inline std::string serverUrl{};
};
