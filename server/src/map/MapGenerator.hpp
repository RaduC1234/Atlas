#pragma once

#include <Atlas.hpp>

using Matrix = std::vector<std::vector<int>>;

class MapGenerator {
private:
    int rows, cols;
    Matrix map;
    std::random_device rd;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;

    int randomValue(int min, int max) {
        dist.param(std::uniform_int_distribution<int>::param_type(min, max));
        return dist(rng);
    }

    void initializeMap() {
        map = Matrix(rows, std::vector<int>(cols, 1));
    }

    void generateWalls() {
        const int targetWallTiles = rows * cols * 45 / 100;
        int wallTilesPlaced = 0;

        while (wallTilesPlaced < targetWallTiles) {
            placeWall(wallTilesPlaced, targetWallTiles);
        }
    }

    void placeWall(int& wallTilesPlaced, int targetWallTiles) {
        const int wallWidth = randomValue(3, 8);
        const int wallHeight = randomValue(3, 8);
        const int randValRow = randomValue(3, cols - wallWidth - 3);
        const int randValCol = randomValue(3, rows - wallHeight - 3);

        for (int row = randValCol; row < randValCol + wallHeight && wallTilesPlaced < targetWallTiles; ++row) {
            for (int col = randValRow; col < randValRow + wallWidth && wallTilesPlaced < targetWallTiles; ++col) {
                if (map[row][col] == 1) {
                    map[row][col] = randomValue(0, 100) < 70 ? 3 : 4;
                    ++wallTilesPlaced;
                }
            }
        }
    }

    void generatePaths() {
        for (int row = randomValue(2, 4); row < rows - 2; row += randomValue(3, 6)) {
            for (int col = randomValue(2, 4); col < cols - 2; col += randomValue(3, 6)) {
                const int length = randomValue(5, 9);
                const bool horizontal = randomValue(0, 1) == 0;
                clearPath(row, col, length, horizontal);
            }
        }
    }

    void clearPath(int row, int col, int length, bool horizontal) {
        for (int i = 0; i < length; ++i) {
            if (horizontal && col + i < cols - 2) {
                map[row][col + i] = 0;
            }
            if (!horizontal && row + i < rows - 2) {
                map[row + i][col] = 0;
            }
        }
    }
    void clearWideEntrance(int row, int col, int dirRow, int dirCol) {
        for (int offset = -1; offset <= 1; ++offset) {
            int newRow = row + (dirCol == 0 ? offset : 0);
            int newCol = col + (dirRow == 0 ? offset : 0);
            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
                map[newRow][newCol] = 0;
            }
        }
    }

    void addGrassMarginsToEntrance(int row, int col, int dirRow, int dirCol) {
        for (int offset = -2; offset <= 2; ++offset) {
            int newRow = row + (dirCol == 0 ? offset : 0);
            int newCol = col + (dirRow == 0 ? offset : 0);

            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols && map[newRow][newCol] == 1) {
                map[newRow][newCol] = randomValue(1, 2);
            }
        }
    }

    void clearEntrancePath(int row, int col, int dirRow, int dirCol) {
        for (int i = 1; i <= 5; ++i) {
            int newRow = row + dirRow * i;
            int newCol = col + dirCol * i;
            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
                map[newRow][newCol] = 0;
            }
            else {
                break;
            }
        }
    }

    void createEntrance(int row, int col, int dirRow, int dirCol) {
        clearWideEntrance(row, col, dirRow, dirCol);
        addGrassMarginsToEntrance(row, col, dirRow, dirCol);
        clearEntrancePath(row, col, dirRow, dirCol);
    }

    void breakEdgeWalls() {
        const int entrancesPerSide = randomValue(4, 7);

        for (int i = 0; i < entrancesPerSide; ++i) {
            createEntrance(0, randomValue(1, cols - 3), 1, 0);
            createEntrance(rows - 1, randomValue(1, cols - 3), -1, 0);
            createEntrance(randomValue(1, rows - 3), 0, 0, 1);
            createEntrance(randomValue(1, rows - 3), cols - 1, 0, -1);
        }
    }

    void retouchBorder(int row, int col) {
        if (map[row][col] == 1 && randomValue(0, 100) < 60) {
            map[row][col] = randomValue(1, 2);
        }
    }

    void retouchBorders() {
        for (int row = 0; row < rows; ++row) {
            retouchBorder(row, 0);
            retouchBorder(row, cols - 1);
        }
        for (int col = 0; col < cols; ++col) {
            retouchBorder(0, col);
            retouchBorder(rows - 1, col);
        }
    }


public:
    MapGenerator(int rows, int cols) : rows(rows), cols(cols), rng(rd()) {
        initializeMap();
        generateWalls();
        generatePaths();
        breakEdgeWalls();
        retouchBorders();
    }

    const Matrix& getMap() const {
        return map;
    }

};