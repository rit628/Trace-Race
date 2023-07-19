#include "BFS.h"
#include <queue>

std::vector<Cell> BFS::findPath(const Grid& grid, int startX, int startY, int goalX, int goalY) {
    // Create a queue for BFS traversal
    std::queue<Cell> queue;
    // Mark the start cell as visited and enqueue it
    grid.setVisited(startX, startY, true);
    queue.push(grid.getCell(startX, startY));

    while (!queue.empty()) {
        // Dequeue a cell from the queue and explore its neighbors
        Cell currentCell = queue.front();
        queue.pop();

        if (currentCell.x == goalX && currentCell.y == goalY) {
            // We found the goal cell, construct and return the path
            std::vector<Cell> path;
            while (currentCell.parent != nullptr) {
                path.push_back(currentCell);
                currentCell = *(currentCell.parent);
            }
            // Add the start cell to the path
            path.push_back(grid.getCell(startX, startY));
            std::reverse(path.begin(), path.end()); // Reverse the path to get it from start to goal
            return path;
        }

        // Explore neighbors of the current cell
        for (const auto& neighbor : grid.getNeighbors(currentCell.x, currentCell.y)) {
            if (!grid.isVisited(neighbor.x, neighbor.y) && !grid.isObstacle(neighbor.x, neighbor.y)) {
                grid.setVisited(neighbor.x, neighbor.y, true);
                neighbor.parent = &currentCell;
                queue.push(neighbor);
            }
        }
    }

    // If BFS reaches here, no path was found, return an empty vector
    return std::vector<Cell>();
}
