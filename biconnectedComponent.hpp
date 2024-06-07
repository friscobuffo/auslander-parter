#ifndef MY_BICONNECTED_COMPONENT_H
#define MY_BICONNECTED_COMPONENT_H

#include <vector>
#include <list>
#include <memory>

#include "graph.hpp"

class Cycle;
class Segment;

class Component : public GraphWithLabels {
private:
    void dfsCycle(int node, bool isNodeVisited[], int prev, std::list<int>& nodeList);
    void cleanupCycle(std::list<int>& nodeList);
    void dfsFindSegments(int node, bool isNodeVisited[], std::vector<int>& nodesInSegment, 
        std::vector<std::pair<int, int>>& edgesInSegment, Cycle& cycle);
public:
    Component(int node);
    Component(int originalNumberOfNodes, std::list<int>& listOfNodes, std::list<std::pair<int, int>>& listOfEdges);
    Cycle* findCycle();
    void findSegments(Cycle& cycle, std::vector<std::unique_ptr<Segment>>& segments);
    void findChords(Cycle& cycle, std::vector<std::unique_ptr<Segment>>& chords);
};

class BiconnectedComponents {
private:
    Graph& originalGraph_m;
    std::vector<int> cutVertices_m{};
    std::vector<std::unique_ptr<Component>> components_m{};
public:
    BiconnectedComponents(Graph& graph, std::vector<int>& cutVertices, std::vector<Component*>& components);
    void print();
    std::vector<std::unique_ptr<Component>>& getComponents();
};

#endif