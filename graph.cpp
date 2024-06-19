#include "graph.hpp"

#include <iostream>
#include <list>

#include "utils.hpp"

MyGraph::MyGraph(int numberOfNodes) : numberOfNodes_m(numberOfNodes) {
    neighborsOfNode_m.resize(numberOfNodes);
}

// assumes edge is not already in graph
// adds edge from-to and edge to-from
void MyGraph::addEdge(int from, int to) {
    neighborsOfNode_m[from].push_back(to);
    neighborsOfNode_m[to].push_back(from);
}

const std::vector<int>& MyGraph::getNeighborsOfNode(int node) const {
    return neighborsOfNode_m[node];
}

void MyGraph::print() const {
    for (int node = 0; node < size(); ++node) {
        const std::vector<int>& neighbors = getNeighborsOfNode(node);
        std::cout << "node: " << node << " neighbors: " << neighbors.size() << " ";
        printIterable(neighbors);
    }
}

int MyGraph::size() const {
    return numberOfNodes_m;
}

// if the graph is bipartite: returns a vector
// with 0s and 1s for each node dividing the nodes into two partitions
const std::optional<std::vector<int>> MyGraph::computeBipartition() const {
    std::vector<int> bipartition{};
    bipartition.resize(size());
    for (int node = 0; node < size(); ++node)
        bipartition[node] = -1;
    for (int node = 0; node < size(); ++node) {
        if (bipartition[node] == -1) {
            bool check = bfsBipartition(node, bipartition);
            if (!check) return std::nullopt;
        }
    }
    return bipartition;
}

bool MyGraph::bfsBipartition(int node, std::vector<int>& bipartition) const {
    bipartition[node] = 0;
    std::list<int> queue{};
    queue.push_back(node);
    while (queue.size() > 0) {
        node = queue.front();
        queue.pop_front();
        for (const int neighbor : getNeighborsOfNode(node)) {
            if (bipartition[neighbor] == -1) {
                bipartition[neighbor] = 1-bipartition[node];
                queue.push_back(neighbor);
            }
            else
                if (bipartition[neighbor] == bipartition[node]) return false;
        }
    }
    return true;
}