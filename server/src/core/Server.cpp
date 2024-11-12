//
// Created by Asula on 11/9/2024.
//
#include "Server.hpp"

namespace {
    std::function<void(int)> shutdown_handler;
    void signal_handler(int signal) {
        if (shutdown_handler) {
            shutdown_handler(signal);
        }
    }
}

Server::Server(const std::string& ip, uint16_t port)
    : ip(ip)
    , port(port)
    , running(false) {
    initializeServer();
    setupSignalHandlers();
}

Server::~Server() {
    if (running) {
        stop();
    }
}

void Server::initializeServer() {
    try {
        commandHandler = std::make_unique<CommandHandler>();
        requestHandler = std::make_unique<RequestHandler>();
        networkManager = std::make_unique<ServerNetworkManager>(ip, port, commandHandler.get(), requestHandler.get());
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to initialize server: " + std::string(e.what()));
    }
}

void Server::setupSignalHandlers() {
    shutdown_handler = [this](int signal) {
        std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
        this->stop();
    };

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
}

void Server::start() {
    if (running) {
        throw std::runtime_error("Server is already running");
    }

    running = true;
    std::cout << "Starting server on " << ip << ":" << port << std::endl;

    try {
        networkManager->start();
    } catch (const std::exception& e) {
        running = false;
        throw std::runtime_error("Failed to start server: " + std::string(e.what()));
    }
}

void Server::stop() {
    if (!running) {
        return;
    }

    std::cout << "Stopping server..." << std::endl;
    networkManager->stop();
    running = false;
}

bool Server::isRunning() const {
    return running;
}