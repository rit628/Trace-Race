#ifndef BFS_H
#define BFS_H

#include "PathfindingAlgorithm.h"

class BFS : public PathfindingAlgorithm {
public:
    // Override the findPath function from the base class
    virtual std::vector<Cell> findPath(const Grid& grid, int startX, int startY, int goalX, int goalY) override;
};

#endif // BFS_H
