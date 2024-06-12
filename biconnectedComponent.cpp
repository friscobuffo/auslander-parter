#include "biconnectedComponent.hpp"

#include <iostream>
#include <cassert>

#include "utils.hpp"

Component::Component(int numberOfNodes) : Graph(numberOfNodes) {
    nodeLabel_m.resize(numberOfNodes);
    for (int i = 0; i < numberOfNodes; ++i)
        nodeLabel_m[i] = -1;
}

void Component::print() const {
    for (int node = 0; node < size(); node++) {
        int label = getLabelOfNode(node);
        const std::vector<int>& neighbors = getNeighborsOfNode(node);
        std::cout << "node: " << label << " neighbors: " << neighbors.size() << " [ ";
        for (const int neighbor : neighbors)
            std::cout << getLabelOfNode(neighbor) << " ";
        std::cout << "]\n";
    }
}

int Component::getLabelOfNode(int node) const {
    assert(nodeLabel_m[node] != -1);
    return nodeLabel_m[node];
}

void Component::assignNodeLabel(int node, int label) {
    assert(nodeLabel_m[node] == -1);
    nodeLabel_m[node] = label;
}

void BiconnectedComponentsHandler::print() {
    std::cout << "Biconnected components:\n";
    std::cout << "Cutvertices: ";
    printIterable(cutVertices_m);
    int index = 0;
    for (auto& component : components_m) {
        std::cout << "Biconnected component [" << index << "]:\n";
        component.print();
        ++index;
    }
}

const std::vector<Component>& BiconnectedComponentsHandler::getComponents() {
    return components_m;
}

// assumes each edge node is in nodes list
Component BiconnectedComponentsHandler::buildComponent(std::list<int>& nodes, std::list<std::pair<int, int>>& edges) {
    Component component(nodes.size());
    int oldToNewNodes[originalGraph_m.size()];
    int index = 0;
    for (const int node : nodes) {
        oldToNewNodes[node] = index;
        component.assignNodeLabel(index, node);
        ++index;
    }
    for (const std::pair<int, int>& edge: edges) {
        int from = oldToNewNodes[edge.first];
        int to = oldToNewNodes[edge.second];
        component.addEdge(from, to);
    }
    return component;
}


void BiconnectedComponentsHandler::dfsBicCom(int node, int nodeId[], int prevOfNode[], int& nextIdToAssign, int lowPoint[],
        std::list<int>& stackOfNodes, std::list<std::pair<int, int>>& stackOfEdges) {
    nodeId[node] = nextIdToAssign;
    lowPoint[node] = nextIdToAssign;
    ++nextIdToAssign;
    int childrenNumber = 0;
    for (const int neighbor : originalGraph_m.getNeighborsOfNode(node)) {
        if (prevOfNode[node] == neighbor)
            continue;
        if (nodeId[neighbor] == -1) { // means node is not visited
            std::list<int> newStackOfNodes{};
            std::list<std::pair<int, int>> newStackOfEdges{};
            childrenNumber++;
            prevOfNode[neighbor] = node;
            newStackOfNodes.push_back(neighbor);
            newStackOfEdges.push_back(std::make_pair(node, neighbor));
            dfsBicCom(neighbor, nodeId, prevOfNode, nextIdToAssign, lowPoint, newStackOfNodes, newStackOfEdges);
            if (lowPoint[neighbor] < lowPoint[node])
                lowPoint[node] = lowPoint[neighbor];
            if (lowPoint[neighbor] >= nodeId[node]) {
                newStackOfNodes.push_back(node);
                components_m.push_back(buildComponent(newStackOfNodes, newStackOfEdges));
                if (prevOfNode[node] != -1) // the root needs to be handled differently
                    // (handled at end of function)
                    cutVertices_m.push_back(node);
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
    if (prevOfNode[node] == -1) { // handling of node with no parents (the root)
        if (childrenNumber >= 2)
            cutVertices_m.push_back(node);
        else if (childrenNumber == 0) { // node is isolated
            components_m.push_back(Component(1));
            components_m.back().assignNodeLabel(0, node);
        }
    }
}

BiconnectedComponentsHandler::BiconnectedComponentsHandler(const Graph& graph) : originalGraph_m(graph) {
    int nodeId[graph.size()];
    int prevOfNode[graph.size()];
    int lowPoint[graph.size()];
    for (int node = 0; node < graph.size(); ++node) {
        nodeId[node] = -1;
        prevOfNode[node] = -1;
        lowPoint[node] = -1;
    }
    int nextIdToAssign = 0;
    std::list<int> stackOfNodes{}; // its better to use lists instead of vectors since
    // we need to merge lists together (O(1) with lists, O(N) with vectors)
    std::list<std::pair<int, int>> stackOfEdges{}; // same arguments
    for (int node = 0; node < graph.size(); node++)
        if (nodeId[node] == -1) // node not visited
            dfsBicCom(node, nodeId, prevOfNode, nextIdToAssign, lowPoint, stackOfNodes, stackOfEdges);
    assert(stackOfNodes.size() == 0);
    assert(stackOfEdges.size() == 0);
}