#ifndef MY_BICONNECTED_COMPONENT_H
#define MY_BICONNECTED_COMPONENT_H

#include <vector>
#include <list>
#include <utility>

#include "graph.hpp"

class Component : public Graph {
private:
    const Graph& originalGraph_m;
    std::vector<int> nodeLabel_m{};
public:
    Component(int numberOfNodes, const Graph& graph);
    void print() const override;
    int getLabelOfNode(int node) const;
    int originalGraphSize() const;
    void assignNodeLabel(int node, int label);
};

class BiconnectedComponentsHandler {
private:
    const Graph& originalGraph_m;
    std::vector<int> cutVertices_m{};
    std::vector<Component> components_m{};
    void dfsBicCom(int node, int nodeId[], int prevOfNode[], int& nextIdToAssign, int lowPoint[],
        std::list<int>& nodes, std::list<std::pair<int, int>>& edges);
    Component buildComponent(std::list<int>& nodes, std::list<std::pair<int, int>>& edges);
public:
    BiconnectedComponentsHandler(const Graph& graph);
    void print();
    const std::vector<Component>& getComponents();
};

#endif