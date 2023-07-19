#include "DFS.h"

bool DFS::dfsRecursive(const Grid& grid, int x, int y, int goalX, int goalY, std::vector<Cell>& path) {
    if (x == goalX && y == goalY) {
        // We found the goal cell, construct and return the path
        path.push_back(grid.getCell(x, y));
        return true;
    }

    // Mark the current cell as visited
    grid.setVisited(x, y, true);

    // Explore neighbors of the current cell
    for (const auto& neighbor : grid.getNeighbors(x, y)) {
        if (!grid.isVisited(neighbor.x, neighbor.y) && !grid.isObstacle(neighbor.x, neighbor.y)) {
            if (dfsRecursive(grid, neighbor.x, neighbor.y, goalX, goalY, path)) {
                // If a path is found from the current neighbor to the goal, add the current cell to the path
                path.push_back(grid.getCell(x, y));
                return true;
            }
        }
    }

    return false;
}

std::vector<Cell> DFS::findPath(const Grid& grid, int startX, int startY, int goalX, int goalY) {
    std::vector<Cell> path;
    // Perform recursive DFS starting from the start cell
    dfsRecursive(grid, startX, startY, goalX, goalY, path);
    // Reverse the path to get it from start to goal
    std::reverse(path.begin(), path.end());
    return path;
}
