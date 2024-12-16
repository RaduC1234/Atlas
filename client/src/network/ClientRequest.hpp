#pragma once

#include <Atlas.hpp>

#include "Packet.hpp"

class ClientRequest {
public:
    using RequestCallback = std::function<void(Packet &)>;
    using ReceiveCallback = std::function<void(Packet &)>;

    ClientRequest() = default;

    ClientRequest(RequestCallback newRequestCallback, ReceiveCallback receiveCallback)
        : onNewRequestCallback(std::move(newRequestCallback)),
          onReceiveCallback(std::move(receiveCallback)) {
    }

    virtual ~ClientRequest() = default;

    virtual void onNewRequest(Packet &packet) {
        if (onNewRequestCallback) {
            onNewRequestCallback(packet);
        }
    }

    virtual void onReceive(Packet &packet) {
        if (onReceiveCallback) {
            onReceiveCallback(packet);
        }
    }

    void setOnNewRequestCallback(RequestCallback callback) {
        onNewRequestCallback = std::move(callback);
    }

    void setOnReceiveCallback(ReceiveCallback callback) {
        onReceiveCallback = std::move(callback);
    }

private:
    RequestCallback onNewRequestCallback = nullptr;
    ReceiveCallback onReceiveCallback = nullptr;
};
