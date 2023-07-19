#ifndef DFS_H
#define DFS_H

#include "PathfindingAlgorithm.h"

class DFS : public PathfindingAlgorithm {
private:
    // Recursive helper function for DFS traversal
    bool dfsRecursive(const Grid& grid, int x, int y, int goalX, int goalY, std::vector<Cell>& path);

public:
    // Override the findPath function from the base class
    virtual std::vector<Cell> findPath(const Grid& grid, int startX, int startY, int goalX, int goalY) override;
};

#endif // DFS_H
