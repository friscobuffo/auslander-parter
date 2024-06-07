#include "biconnectedComponent.hpp"

#include <iostream>
#include <utility>
#include <cassert>

#include "utils.hpp"
#include "segment.hpp"
#include "cycle.hpp"

Component::Component(int node) : GraphWithLabels({node}) {}

Component::Component(int originalNumberOfNodes, std::list<int>& listOfNodes, std::list<std::pair<int, int>>& listOfEdges) 
    : GraphWithLabels(listOfNodes) {
    int oldToNewNodes[originalNumberOfNodes];
    int index = 0;
    for (const int node : listOfNodes) {
        oldToNewNodes[node] = index;
        ++index;
    }
    for (const std::pair<int, int>& edge: listOfEdges) {
        int from = oldToNewNodes[edge.first];
        int to = oldToNewNodes[edge.second];
        addEdge(from, to);
    }
}

void Component::dfsCycle(int node, bool isNodeVisited[], int prev, std::list<int>& nodeList) {
    nodeList.push_back(node);
    isNodeVisited[node] = true;
    for (const int neighbor : getNeighborsOfNode(node)) {
        if (neighbor == prev)
            continue;
        if (!isNodeVisited[neighbor]) {
            dfsCycle(neighbor, isNodeVisited, node, nodeList);
            break;
        }
        nodeList.push_back(neighbor);
        return;
    }
}

void Component::cleanupCycle(std::list<int>& nodeList) {
    while (nodeList.front() != nodeList.back())
        nodeList.pop_front();
    nodeList.pop_back();
}

Cycle* Component::findCycle() {
    #ifdef DEBUG_MODE
    std::cout << "finding cycle in biconnected component:\n";
    #endif
    bool isNodeVisited[numberOfNodes()];
    for (int node = 0; node < numberOfNodes(); ++node)
        isNodeVisited[node] = false;
    std::list<int> nodeList;
    dfsCycle(0, isNodeVisited, -1, nodeList);
    cleanupCycle(nodeList);
    return new Cycle(*this, nodeList);
}

// parameter isCycleNodeAlreadyInSegment is necessary otherwise attachment nodes may be added multiple times
void Component::dfsFindSegments(int node, bool isNodeVisited[], std::vector<int>& nodesInSegment,
    std::vector<std::pair<int, int>>& edgesInSegment, Cycle& cycle) {
    nodesInSegment.push_back(node);
    isNodeVisited[node] = true;
    for (const int neighbor : getNeighborsOfNode(node)) {
        if (cycle.hasNode(neighbor)) {
            edgesInSegment.push_back(std::make_pair(node, neighbor));
            continue;
        }
        if (node < neighbor)
            edgesInSegment.push_back(std::make_pair(node, neighbor));
        if (!isNodeVisited[neighbor])
            dfsFindSegments(neighbor, isNodeVisited, nodesInSegment, edgesInSegment, cycle);
    }
}

void Component::findChords(Cycle& cycle, std::vector<std::unique_ptr<Segment>>& segments) {
    for (int node = 0; node < numberOfNodes(); ++node) {
        if (!cycle.hasNode(node)) continue;
        for (const int neighbor : getNeighborsOfNode(node)) {
            if (node < neighbor) continue;
            if (cycle.hasNode(neighbor))
                if (neighbor != cycle.getPrevOfNode(node) && neighbor != cycle.getNextOfNode(node))
                    segments.push_back(std::make_unique<Segment>(*this, node, neighbor, cycle));
        }
    }
}

void Component::findSegments(Cycle& cycle, std::vector<std::unique_ptr<Segment>>& segments) {
    #ifdef DEBUG_MODE
    std::cout << "finding segments\n";
    #endif
    bool isNodeVisited[numberOfNodes()];
    for (int node = 0; node < numberOfNodes(); ++node)
        isNodeVisited[node] = false;
    for (int node = 0; node < numberOfNodes(); ++node)
        if (cycle.hasNode(node))
            isNodeVisited[node] = true;
    for (int node = 0; node < numberOfNodes(); ++node) {
        if (!isNodeVisited[node]) {
            std::vector<int> nodesInSegment{};
            std::vector<std::pair<int, int>> edgesInSegment{};
            dfsFindSegments(node, isNodeVisited, nodesInSegment, edgesInSegment, cycle);
            for (const int node : cycle.nodes())
                nodesInSegment.push_back(node);
            segments.push_back(std::make_unique<Segment>(*this, nodesInSegment, edgesInSegment, cycle));
        }
    }
}

BiconnectedComponents::BiconnectedComponents(Graph& graph, std::vector<int>& cutVertices, std::vector<Component*>& components) 
    : originalGraph_m(graph) {
    for (const int cutVertex : cutVertices)
        cutVertices_m.push_back(cutVertex);
    for (Component* component : components)
        components_m.push_back(std::unique_ptr<Component>(component));
}

void BiconnectedComponents::print() {
    std::cout << "Biconnected components:\n";
    std::cout << "Cutvertices: ";
    printIterable(cutVertices_m);
    printf("\n");
    int index = 0;
    for (auto& component : components_m) {
        std::cout << "Biconnected component [" << index << "]:\n";
        component->print();
        ++index;
    }
}

std::vector<std::unique_ptr<Component>>& BiconnectedComponents::getComponents() {
    return components_m;
}