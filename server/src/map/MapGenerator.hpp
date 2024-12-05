#pragma once

#include <Atlas.hpp>

using Matrix = std::vector<std::vector<int>>;

class MapGenerator
{
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

public:
    MapGenerator(int rows, int cols) : rows(rows), cols(cols), rng(rd()) {
        initializeMap();
        generateWalls();
        generatePaths();
    }

    const Matrix& getMap() const {
        return map;
    }

};