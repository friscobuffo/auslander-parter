#include "cycle.hpp"

#include <cassert>

#include "utils.hpp"

void Cycle::dfsBuildCycle(int node, bool isNodeVisited[], int prev) {
    nodes_m.push_back(node);
    isNodeVisited[node] = true;
    for (const int neighbor : originalComponent_m.getNeighborsOfNode(node)) {
        if (neighbor == prev)
            continue;
        if (!isNodeVisited[neighbor]) {
            dfsBuildCycle(neighbor, isNodeVisited, node);
            break;
        }
        nodes_m.push_back(neighbor);
        return;
    }
}

// removes leading nodes of the list that do not partecipate in the cycle
// assumes starting node of the cycle is contained twice
// (at the start and at the end of the actual cycle)
void Cycle::cleanupCycle() {
    std::vector<int> copy(nodes_m);
    nodes_m.clear();
    bool foundEnd{false};
    for (int i = 0; i < copy.size(); ++i) {
        if (foundEnd) nodes_m.push_back(copy[i]);
        else if (copy[i] == copy.back()) foundEnd = true;
    }
}

Cycle::Cycle(const Component& component) : originalComponent_m(component) {
    bool isNodeVisited[component.size()];
    for (int node = 0; node < component.size(); ++node)
        isNodeVisited[node] = false;
    dfsBuildCycle(0, isNodeVisited, -1);
    cleanupCycle();
    posInCycle_m.resize(component.size());
    for (int i = 0; i < component.size(); ++i)
        posInCycle_m[i] = -1;
    int index = 0;
    for (const int node : nodes_m) {
        posInCycle_m[node] = index;
        ++index;
    }
}

// if nodeToInclude is -1 it is just ignored
void Cycle::changeWithPath(std::list<int>& path, int nodeToInclude) {
    std::list<int> nodesCopy(path); // newCycleList
    int firstOfPath = path.front();
    int lastOfPath = path.back();
    int i = 0;
    while (nodes_m[i] != lastOfPath)
        nextIndex(i);
    nextIndex(i);
    if (nodeToInclude != -1) {
        bool foundNodeToInclude = false;
        while (nodes_m[i] != firstOfPath) {
            if (nodes_m[i] == nodeToInclude) foundNodeToInclude = true;
            nodesCopy.push_back(nodes_m[i]);
            nextIndex(i);
        }
        if (!foundNodeToInclude) { // cycle needs to be reversed
            reverse();
            changeWithPath(path, nodeToInclude);
            return;
        }
    }
    else {
        while (nodes_m[i] != firstOfPath) {
            nodesCopy.push_back(nodes_m[i]);
            nextIndex(i);
        }
    }
    nodes_m.clear();
    for (int i = 0; i < posInCycle_m.size(); ++i)
        posInCycle_m[i] = -1;
    i = 0;
    while (nodesCopy.size() > 0) {
        nodes_m.push_back(nodesCopy.back());
        posInCycle_m[nodesCopy.back()] = i;
        nodesCopy.pop_back();
        ++i;
    }
}

bool Cycle::hasNode(int node) const {
    return posInCycle_m[node] != -1;
}

int Cycle::size() const {
    return nodes_m.size();
}

int Cycle::getPrevOfNode(int node) const {
    int pos = posInCycle_m[node];
    assert(pos != -1);
    if (pos == 0) return nodes_m[size()-1];
    return nodes_m[pos-1];
}

int Cycle::getNextOfNode(int node) const {
    int pos = posInCycle_m[node];
    assert(pos != -1);
    if (pos == size()-1) return nodes_m[0];
    return nodes_m[pos+1];
}

void Cycle::nextIndex(int& index) {
    ++index;
    index %= nodes_m.size();
}

const std::vector<int>& Cycle::nodes() const {
    return nodes_m;
}

void Cycle::print() const {
    printIterable(nodes_m);
}

void Cycle::reverse() {
    int start = 0;
    int end = nodes_m.size()-1;
    while (start < end) {
        int temp = nodes_m[start];
        nodes_m[start] = nodes_m[end];
        nodes_m[end] = temp;
        posInCycle_m[nodes_m[start]] = start;
        posInCycle_m[nodes_m[end]] = end;
        ++start;
        --end;
    }
}

int Cycle::getOriginalComponentSize() const {
    return originalComponent_m.size();
}

std::optional<int> Cycle::getIndexOfNode(int node) const {
    int pos = posInCycle_m[node];
    if (pos != -1) return pos;
    return std::nullopt;
}