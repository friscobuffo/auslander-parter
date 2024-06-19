#ifndef MY_BICONNECTED_COMPONENT_H
#define MY_BICONNECTED_COMPONENT_H

#include <vector>
#include <list>
#include <utility>

#include "graph.hpp"

class Component : public MyGraph {
private:
    const MyGraph& originalGraph_m;
    std::vector<int> nodeLabel_m{};

public:
    Component(int numberOfNodes, const MyGraph& graph);

    void print() const override;
    int getLabelOfNode(int node) const;
    int originalGraphSize() const;
    void assignNodeLabel(int node, int label);
};


class BiconnectedComponentsHandler {
private:
    const MyGraph& originalGraph_m;
    std::vector<bool> isCutVertex_m{};
    std::vector<int> cutVertices_m{};
    std::vector<Component> components_m{};

    void dfsBicCom(int node, int nodeId[], int prevOfNode[], int& nextIdToAssign, int lowPoint[],
        std::list<int>& nodes, std::list<std::pair<int, int>>& edges);
    const Component buildComponent(std::list<int>& nodes, std::list<std::pair<int, int>>& edges);

public:
    BiconnectedComponentsHandler(const MyGraph& graph);

    void print() const;
    const std::vector<Component>& getComponents() const;
};

#endif