//generator pentru path, indestructible wall, destructible wall
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

    void createCornerSpaces() {
        std::vector<std::pair<int, int>> corners = {
            {0, 0}, {0, rows - 2}, {cols - 2, 0}, {cols - 2, rows - 2}
        };

        for (const auto& corner : corners) {
            int cornerX = corner.first;
            int cornerY = corner.second;

            for (int y = cornerY; y < cornerY + 2; ++y) {
                for (int x = cornerX; x < cornerX + 2; ++x) {
                    if (y >= 0 && y < rows && x >= 0 && x < cols) {
                        map[y][x] = 48; // Path
                    }
                }
            }
        }
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

    double distanceBetweenPoints(int x1, int y1, int x2, int y2) {
        int dx = x2 - x1;
        int dy = y2 - y1;
        return std::sqrt(dx * dx + dy * dy);
    }

    void connectCornerToNearestRoom() {
        std::vector<std::pair<int, int>> corners = {
            {0, 0}, {0, rows - 2}, {cols - 2, 0}, {cols - 2, rows - 2}
        };

        for (const auto& corner : corners) {
            int cornerCenterX = corner.first + 1;
            int cornerCenterY = corner.second + 1;

            double minDistance = std::numeric_limits<double>::max();
            Room* nearestRoom = nullptr;

            for (const auto& room : rooms) {
                double dist = distanceBetweenPoints(cornerCenterX, cornerCenterY,
                                                  room->centerX(), room->centerY());
                if (dist < minDistance) {
                    minDistance = dist;
                    nearestRoom = room;
                }
            }

            if (nearestRoom) {
                int roomX = nearestRoom->centerX();
                int roomY = nearestRoom->centerY();

                int x1 = std::min(cornerCenterX, roomX);
                int x2 = std::max(cornerCenterX, roomX);
                for (int x = x1; x <= x2; ++x) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        int y = cornerCenterY + dy;
                        if (y >= 0 && y < rows && x >= 0 && x < cols) {
                            map[y][x] = 48; // Path
                        }
                    }
                }

                int y1 = std::min(cornerCenterY, roomY);
                int y2 = std::max(cornerCenterY, roomY);
                for (int y = y1; y <= y2; ++y) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        int x = roomX + dx;
                        if (x >= 0 && x < cols && y >= 0 && y < rows) {
                            map[y][x] = 48; // Path
                        }
                    }
                }
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

    std::vector<std::vector<int>> labelCorridors() {
        std::vector<std::vector<int>> labels(rows, std::vector<int>(cols, 0));
        int currentLabel = 1;
        std::queue<std::pair<int, int>> q;

        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                if (map[y][x] == 48 && !isInRoom(x, y) && labels[y][x] == 0) {
                    q.push({x, y});
                    labels[y][x] = currentLabel;

                    while (!q.empty()) {
                        auto [cx, cy] = q.front();
                        q.pop();

                        for (int dy = -1; dy <= 1; ++dy) {
                            for (int dx = -1; dx <= 1; ++dx) {
                                if (dy == 0 && dx == 0)
                                    continue;
                                int nx = cx + dx;
                                int ny = cy + dy;
                                if (nx >= 0 && nx < cols && ny >= 0 && ny < rows &&
                                    map[ny][nx] == 48 && !isInRoom(nx, ny) && labels[ny][nx] == 0) {
                                    labels[ny][nx] = currentLabel;
                                    q.push({nx, ny});
                                }
                            }
                        }
                    }
                    currentLabel++;
                }
            }
        }

        return labels;
    }

    bool isInRoom(int x, int y) const {
        for (const auto& room : rooms) {
            if (x >= room->x && x < room->x + room->width &&
                y >= room->y && y < room->y + room->height) {
                return true;
            }
        }
        return false;
    }

    void placeBreakableWalls() {
        std::vector<std::vector<int>> labels = labelCorridors();

        std::vector<std::vector<std::pair<int, int>>> corridors;
        int maxLabel = 0;
        for (const auto& row : labels) {
            for (int label : row) {
                if (label > maxLabel)
                    maxLabel = label;
            }
        }

        corridors.resize(maxLabel + 1);

        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                if (labels[y][x] > 0) {
                    corridors[labels[y][x]].emplace_back(x, y);
                }
            }
        }

        const double WALL_PROBABILITY_PER_CORRIDOR = 0.5;

        for (size_t label = 1; label < corridors.size(); ++label) {
            if (corridors[label].empty())
                continue;

            std::shuffle(corridors[label].begin(), corridors[label].end(), rng);

            int wallsToPlace = static_cast<int>(corridors[label].size() * WALL_PROBABILITY_PER_CORRIDOR);
            if (wallsToPlace < 1 && corridors[label].size() > 0)
                wallsToPlace = 1;

            int wallsPlaced = 0;

            for (const auto& tile : corridors[label]) {
                if (wallsPlaced >= wallsToPlace)
                    break;

                int x = tile.first;
                int y = tile.second;

                map[y][x] = 63; // Breakable box
                wallsPlaced++;
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

        placeBreakableWalls();
        createCornerSpaces();
        connectCornerToNearestRoom();
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
