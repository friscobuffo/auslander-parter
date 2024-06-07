#include "cycle.hpp"

#include "biconnectedComponent.hpp"
#include "graph.hpp"
#include "utils.hpp"

Cycle::Cycle(Component& component, std::list<int>& listOfNodes)
    : originalComponent_m(component) {
    posInCycle_m.resize(component.numberOfNodes());
    for (int i = 0; i < component.numberOfNodes(); i++)
        posInCycle_m[i] = -1;
    nodes_m.resize(listOfNodes.size());
    int index = 0;
    for (const int node : listOfNodes) {
        nodes_m[index] = node;
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

bool Cycle::hasNode(int node) {
    return posInCycle_m[node] != -1;
}

int Cycle::size() {
    return nodes_m.size();
}

int Cycle::getPrevOfNode(int node) {
    int pos = posInCycle_m[node];
    if (pos == 0) return nodes_m[size()-1];
    return nodes_m[pos-1];
}

int Cycle::getNextOfNode(int node) {
    int pos = posInCycle_m[node];
    if (pos == size()-1) return nodes_m[0];
    return nodes_m[pos+1];
}

void Cycle::nextIndex(int& index) {
    ++index;
    index %= nodes_m.size();
}

std::vector<int>& Cycle::nodes() {
    return nodes_m;
}

void Cycle::print() {
    printIterable(nodes_m);
}

void Cycle::reverse() {
    int start = 0;
    int end = nodes_m.size()-1;
    while (start < end) {
        int temp = nodes_m[start];
        nodes_m[start] = nodes_m[end];
        nodes_m[end] = temp;
        ++start;
        --end;
    }
}

int Cycle::getNodeByIndex(int index) {
    return nodes_m[index];
}

int Cycle::getOriginalComponentSize() {
    return originalComponent_m.numberOfNodes();
}