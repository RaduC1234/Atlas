#pragma once

#include <Atlas.hpp>

#pragma once


class MapGenerator {
private:
    struct Room {
        int x, y;
        int width, height;
        std::vector<Room*> connections;

        Room(int x, int y, int w, int h)
            : x(x), y(y), width(w), height(h) {}

        bool intersects(const Room& other, int padding = 2) const {
            return !(x + width + padding < other.x ||
                     other.x + other.width + padding < x ||
                     y + height + padding < other.y ||
                     other.y + other.height + padding < y);
        }

        int centerX() const { return x + width / 2; }
        int centerY() const { return y + height / 2; }
    };

    int rows, cols;
    std::vector<std::vector<int>> map;
    std::vector<Room*> rooms;
    std::random_device rd;
    std::mt19937 rng;
    std::vector<std::pair<int, int>> corridorTiles;

    // Configuration constants
    const int MIN_ROOM_SIZE = 8;
    const int MAX_ROOM_SIZE = 15;
    const int MIN_ROOMS = 8;
    const int MAX_ROOMS = 12;
    const int BORDER = 2;
    const int MIN_CORRIDOR_WIDTH = 3;
    const int MAX_CORRIDOR_WIDTH = 5;

    int randomValue(int min, int max) {
        return std::uniform_int_distribution<>(min, max)(rng);
    }

    void initializeMap() {
        map = std::vector<std::vector<int>>(rows, std::vector<int>(cols, 40)); // Indestructible walls
    }


    bool isValidRoomPlacement(const Room& room) {
        if (room.x < BORDER || room.y < BORDER ||
            room.x + room.width >= cols - BORDER ||
            room.y + room.height >= rows - BORDER) {
            return false;
        }

        for (const auto& existing : rooms) {
            if (room.intersects(*existing)) {
                return false;
            }
        }
        return true;
    }

    void generateRooms() {
        const int MAX_ATTEMPTS = 200;
        int attempts = 0;
        int targetRooms = randomValue(MIN_ROOMS, MAX_ROOMS);

        while (rooms.empty() && attempts < MAX_ATTEMPTS) {
            int centerX = cols / 2 - MAX_ROOM_SIZE / 2;
            int centerY = rows / 2 - MAX_ROOM_SIZE / 2;
            int w = randomValue(MIN_ROOM_SIZE, MAX_ROOM_SIZE);
            int h = randomValue(MIN_ROOM_SIZE, MAX_ROOM_SIZE);

            Room* centralRoom = new Room(centerX, centerY, w, h);
            if (isValidRoomPlacement(*centralRoom)) {
                rooms.push_back(centralRoom);
            } else {
                delete centralRoom;
            }
            attempts++;
        }

        while (rooms.size() < targetRooms && attempts < MAX_ATTEMPTS) {
            Room* parent = rooms[randomValue(0, rooms.size() - 1)];
            int w = randomValue(MIN_ROOM_SIZE, MAX_ROOM_SIZE);
            int h = randomValue(MIN_ROOM_SIZE, MAX_ROOM_SIZE);

            std::vector<std::pair<int, int>> positions = {
                {parent->x - w - MIN_CORRIDOR_WIDTH, parent->y},
                {parent->x + parent->width + MIN_CORRIDOR_WIDTH, parent->y},
                {parent->x, parent->y - h - MIN_CORRIDOR_WIDTH},
                {parent->x, parent->y + parent->height + MIN_CORRIDOR_WIDTH}
            };

            std::shuffle(positions.begin(), positions.end(), rng);

            bool placed = false;
            for (const auto& pos : positions) {
                Room* newRoom = new Room(pos.first, pos.second, w, h);
                if (isValidRoomPlacement(*newRoom)) {
                    rooms.push_back(newRoom);
                    parent->connections.push_back(newRoom);
                    newRoom->connections.push_back(parent);
                    placed = true;
                    break;
                }
                delete newRoom;
            }
            if (!placed) {
                attempts++;
            }
        }
    }

    void carveRooms() {
        for (const auto& room : rooms) {
            for (int y = room->y; y < room->y + room->height; ++y) {
                for (int x = room->x; x < room->x + room->width; ++x) {
                    if (y >= 0 && y < rows && x >= 0 && x < cols) {
                        map[y][x] = 48; // Path
                    }
                }
            }
        }
    }

    void connectRooms(Room* r1, Room* r2) {
        int x1 = r1->centerX();
        int y1 = r1->centerY();
        int x2 = r2->centerX();
        int y2 = r2->centerY();

        int corridorWidth = randomValue(MIN_CORRIDOR_WIDTH, MAX_CORRIDOR_WIDTH);
        int halfWidth = corridorWidth / 2;

        bool horizontalFirst = randomValue(0, 1) == 0;

        if (horizontalFirst) {
            for (int x = std::min(x1, x2); x <= std::max(x1, x2); ++x) {
                for (int dy = -halfWidth; dy <= halfWidth; ++dy) {
                    int y = y1 + dy;
                    if (y >= 0 && y < rows && x >= 0 && x < cols) {
                        map[y][x] = 48; // Path
                    }
                }
            }
            for (int y = std::min(y1, y2); y <= std::max(y1, y2); ++y) {
                for (int dx = -halfWidth; dx <= halfWidth; ++dx) {
                    int x = x2 + dx;
                    if (y >= 0 && y < rows && x >= 0 && x < cols) {
                        map[y][x] = 48; // Path
                    }
                }
            }
        } else {
            for (int y = std::min(y1, y2); y <= std::max(y1, y2); ++y) {
                for (int dx = -halfWidth; dx <= halfWidth; ++dx) {
                    int x = x1 + dx;
                    if (y >= 0 && y < rows && x >= 0 && x < cols) {
                        map[y][x] = 48; // Path
                    }
                }
            }
            for (int x = std::min(x1, x2); x <= std::max(x1, x2); ++x) {
                for (int dy = -halfWidth; dy <= halfWidth; ++dy) {
                    int y = y2 + dy;
                    if (y >= 0 && y < rows && x >= 0 && x < cols) {
                        map[y][x] = 48; // Path
                    }
                }
            }
        }
    }

    void createConnections() {
         for (const auto& room : rooms) {
             for (const auto& connected : room->connections) {
                 connectRooms(room, connected);
            }
         }
    }


    void addExtraConnections(int connectionChance) {
        for (size_t i = 0; i < rooms.size(); ++i) {
            if (randomValue(0, 100) < connectionChance) {
                Room* room1 = rooms[i];
                Room* room2 = rooms[randomValue(0, rooms.size() - 1)];

                if (room1 != room2 &&
                    std::find(room1->connections.begin(), room1->connections.end(), room2) == room1->connections.end()) {
                    room1->connections.push_back(room2);
                    room2->connections.push_back(room1);
                    connectRooms(room1, room2);
                }
            }
        }
    }


public:
    MapGenerator(int numRows, int numCols)
        : rows(numRows), cols(numCols), rng(rd()) {
        initializeMap();

        generateRooms();
        carveRooms();
        createConnections();
        addExtraConnections(30);

    }

    ~MapGenerator() {
        for (auto room : rooms) {
            delete room;
        }
    }

    const std::vector<std::vector<int>>& getMap() const {
        return map;
    }

};

