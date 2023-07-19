#ifndef PATHFINDINGALGORITHM_H
#define PATHFINDINGALGORITHM_H

#include "Board.h" // Assuming Board.h is included for pathfinding purposes

class PathfindingAlgorithm {
public:
    // Virtual function to be overridden by specific algorithm implementations
    virtual std::vector<Cell> findPath(const Grid& grid, int startX, int startY, int goalX, int goalY) = 0;
};

#endif // PATHFINDINGALGORITHM_H
