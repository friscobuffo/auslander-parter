#include "graph.hpp"

#include <iostream>
#include <cassert>
#include <memory>

#include "biconnectedComponent.hpp"
#include "utils.hpp"

Graph::Graph(int numberOfNodes) : numberOfNodes_m(numberOfNodes) {
    neighborsOfNode_m.resize(numberOfNodes);
}

// assumes edge is not already in graph
// adds edge from-to and edge to-from
void Graph::addEdge(int from, int to) {
    neighborsOfNode_m[from].push_back(to);
    neighborsOfNode_m[to].push_back(from);
}

std::vector<int>& Graph::getNeighborsOfNode(int node) {
    return neighborsOfNode_m[node];
}

void Graph::print() {
    for (int node = 0; node < numberOfNodes(); ++node) {
        std::vector<int>& neighbors = getNeighborsOfNode(node);
        std::cout << "node: " << node << " neighbors: " << neighbors.size() << " ";
        printVector(neighbors);
        std::cout << std::endl;
    }
}

int Graph::numberOfNodes() {
    return numberOfNodes_m;
}

std::list<int>* Graph::computePathBetweenNodes(int start, int end) {
    int prevOfNode[numberOfNodes_m];
    for (int node = 0; node < numberOfNodes_m; ++node)
        prevOfNode[node] = -1;
    std::list<int> queue{};
    queue.push_back(start);
    while (queue.size() != 0) {
        int node = queue.front();
        queue.pop_front();
        for (const int neighbor : getNeighborsOfNode(node)) {
            if (prevOfNode[neighbor] == -1) {
                prevOfNode[neighbor] = node;
                queue.push_back(neighbor);
                if (neighbor == end) break;
            }
        }
        if (prevOfNode[end] != -1) break;
    }
    std::list<int>* path = new std::list<int>();
    int crawl = end;
    while (crawl != start) {
        path->push_front(crawl);
        crawl = prevOfNode[crawl];
    }
    path->push_front(crawl);
    return path;
}

// assumes bipartition vector is initially empty
// populates the vector with 0s and 1s for each node dividing the nodes into two partitions
bool Graph::computeBipartition(std::vector<int>& bipartition) {
    assert(bipartition.size() == 0);
    bipartition.resize(numberOfNodes_m);
    for (int node = 0; node < numberOfNodes_m; ++node)
        bipartition[node] = -1;
    for (int node = 0; node < numberOfNodes_m; ++node) {
        if (bipartition[node] == -1) {
            bool check = bfsBipartition(node, bipartition);
            if (!check) return false;
        }
    }
    return true;
}

bool Graph::bfsBipartition(int node, std::vector<int>& bipartition) {
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


void Graph::dfsBicCom(int node, int nodeId[], int prevOfNode[], int& nextIdToAssign, int lowPoint[],
        std::list<int>& stackOfNodes, std::list<std::pair<int, int>>& stackOfEdges,
        std::vector<int>& cutVertices, std::vector<Component*>& components) {
    nodeId[node] = nextIdToAssign;
    lowPoint[node] = nextIdToAssign;
    ++nextIdToAssign;
    int childrenNumber = 0;
    for (const int neighbor : getNeighborsOfNode(node)) {
        if (prevOfNode[node] == neighbor)
            continue;
        if (nodeId[neighbor] == -1) { // means node is not visited
            std::list<int> newStackOfNodes{};
            std::list<std::pair<int, int>> newStackOfEdges{};
            childrenNumber++;
            prevOfNode[neighbor] = node;
            newStackOfNodes.push_back(neighbor);
            newStackOfEdges.push_back(std::make_pair(node, neighbor));
            dfsBicCom(neighbor, nodeId, prevOfNode, nextIdToAssign, lowPoint, newStackOfNodes, newStackOfEdges, cutVertices, components);
            if (lowPoint[neighbor] < lowPoint[node])
                lowPoint[node] = lowPoint[neighbor];
            if (lowPoint[neighbor] >= nodeId[node]) {
                newStackOfNodes.push_back(node);
                components.push_back(new Component(numberOfNodes(), newStackOfNodes, newStackOfEdges));
                if (prevOfNode[node] != -1) // if it has no parents needs to be handled differently
                    // (handled at end of function)
                    cutVertices.push_back(node);
            }
            else {
                stackOfNodes.splice(stackOfNodes.end(), newStackOfNodes);
                stackOfEdges.splice(stackOfEdges.end(), newStackOfEdges);
            }
        }
        else { // node got already visited
            int neighborNodeId = nodeId[neighbor];
            if (neighborNodeId < nodeId[node]) {
                stackOfEdges.push_back(std::make_pair(node, neighbor));
                if (neighborNodeId < lowPoint[node])
                    lowPoint[node] = neighborNodeId;
            }
        }
    }
    if (prevOfNode[node] == -1) { // handling of node with no parents
        if (childrenNumber >= 2)
            cutVertices.push_back(node);
        else if (childrenNumber == 0)
            components.push_back(new Component(node));
    }
}

BiconnectedComponents* Graph::computeBiconnectedComponents() {
    int nodeId[numberOfNodes()];
    int prevOfNode[numberOfNodes()];
    int lowPoint[numberOfNodes()];
    for (int node = 0; node < numberOfNodes(); ++node) {
        nodeId[node] = -1;
        prevOfNode[node] = -1;
        lowPoint[node] = -1;
    }
    int nextIdToAssign = 0;
    std::list<int> stackOfNodes; // its better to use lists instead of vectors since
    // we need to merge lists together (O(1) with lists, O(N) with vectors)
    std::list<std::pair<int, int>> stackOfEdges{}; // same arguments
    std::vector<int> cutVertices{};
    std::vector<Component*> components{};
    for (int node = 0; node < numberOfNodes(); node++)
        if (nodeId[node] == -1) // node not visited
            dfsBicCom(node, nodeId, prevOfNode, nextIdToAssign, lowPoint, stackOfNodes,
                    stackOfEdges, cutVertices, components);
    assert(stackOfNodes.size() == 0);
    assert(stackOfEdges.size() == 0);
    return new BiconnectedComponents(*this, cutVertices, components);
}

GraphWithLabels::GraphWithLabels(std::list<int>& labels) : Graph(labels.size()) {
    for (const int label : labels)
        nodeLabel_m.push_back(label);
}

GraphWithLabels::GraphWithLabels(std::vector<int>& labels) : Graph(labels.size()) {
    for (const int label : labels)
        nodeLabel_m.push_back(label);
}

GraphWithLabels::GraphWithLabels(std::initializer_list<int> labels) : Graph(labels.size()){
    for (const int label : labels)
        nodeLabel_m.push_back(label);
}

void GraphWithLabels::print() {
    for (int node = 0; node < numberOfNodes(); node++) {
        int label = nodeLabel_m[node];
        std::vector<int>& neighbors = getNeighborsOfNode(node);
        std::cout << "node: " << label << " neighbors: " << neighbors.size() << " [ ";
        for (const int neighbor : neighbors)
            std::cout << nodeLabel_m[neighbor] << " ";
        std::cout << "]\n";
    }
}

int GraphWithLabels::getLabelOfNode(int node) {
    return nodeLabel_m[node];
}