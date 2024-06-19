#ifndef MY_CYCLE_H
#define MY_CYCLE_H

#include <vector>
#include <list>
#include <optional>

#include "biconnectedComponent.hpp"

class Cycle {
private:
    std::vector<int> nodes_m{};
    std::vector<int> posInCycle_m{};
    const Component& originalComponent_m;

    void dfsBuildCycle(int node, bool isNodeVisited[], int prev);
    void cleanupCycle();
    void nextIndex(int& index);

public:
    Cycle(const Component& component);

    void changeWithPath(std::list<int>& path, int nodeToInclude);
    bool hasNode(int node) const;
    int size() const;
    int getPrevOfNode(int node) const;
    int getNextOfNode(int node) const;
    const std::vector<int>& nodes() const;
    void reverse();
    void print() const;
    int getOriginalComponentSize() const;
    std::optional<int> getIndexOfNode(int node) const;
};

#endif