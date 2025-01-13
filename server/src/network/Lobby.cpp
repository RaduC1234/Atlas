#include "Lobby.hpp"

Lobby::Lobby(Lobby &&other) noexcept: registry(std::move(other.registry)),
                                      players(std::move(other.players)),
                                      entId(other.entId) {
}

Lobby & Lobby::operator=(Lobby &&other) noexcept {
    if (this != &other) {
        std::lock_guard<std::mutex> lock(other.registryMutex);
        registry = std::move(other.registry);
        players = std::move(other.players);
        entId = other.entId;
    }
    return *this;
}

bool Lobby::containsPlayer(uint64_t playerId) {
    return std::ranges::find(players, playerId) != players.end();
}

void Lobby::serializeRegistry(nlohmann::json &outJson) {
    std::lock_guard<std::mutex> lock(registryMutex);
    outJson["entities"] = nlohmann::json::array();

    auto view = registry.view<NetworkComponent, TransformComponent>();
    for (auto entity: view) {
        auto &network = view.get<NetworkComponent>(entity);
        auto &transform = registry.get<TransformComponent>(entity);

        nlohmann::json entityJson;
        entityJson["id"] = static_cast<int>(entity);
        entityJson["networkId"] = network.networkId;
        entityJson["entityType"] = static_cast<int>(network.entityType);
        entityJson["tile-code"] = static_cast<int>(network.tileCode);

        entityJson["TransformComponent"] = {
            {"position", {transform.position.x, transform.position.y, transform.position.z}},
            {"rotation", transform.rotation},
            {"scale", {transform.scale.x, transform.scale.y}}
        };

        if (registry.any_of<PawnComponent>(entity)) {
            auto &pawn = registry.get<PawnComponent>(entity);
            entityJson["PawnComponent"] = {
                {"playerId", pawn.playerId},
                {"moveForward", pawn.moveForward},
                {"moveBackwards", pawn.moveBackwards},
                {"moveLeft", pawn.moveLeft},
                {"moveRight", pawn.moveRight},
                {"aimRotation", pawn.aimRotation}
            };
        }

        outJson["entities"].push_back(entityJson);
    }
}

uint64_t Lobby::nextId() {
    return entId++;
}
