#pragma once

// Core C++ libraries
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <memory>
#include <random>
#include <sstream>
#include <functional>
#include <csignal>

// External libraries
#include <nlohmann/json.hpp>
#include <crow.h>

// Forward declarations (if full includes are not needed)
class CommandHandler;
class RequestHandler;
class ServerNetworkManager;
class Lobby;

// Project-specific includes
#include "Server.hpp"
#include "ServerNetworkManager.hpp"
#include "CommandHandler.hpp"
#include "RequestHandler.hpp"
#include "Lobby.hpp"

