#ifndef MY_GRAPH_H
#define MY_GRAPH_H

#include <vector>
#include <list>

class Component;
class BiconnectedComponents;

class Graph {
private:
    int numberOfNodes_m{};
    std::vector<std::vector<int>> neighborsOfNode_m{};
    void dfsBicCom(int node, int nodeId[], int prevOfNode[], int& nextIdToAssign, int lowPoint[],
            std::list<int>& stackOfNodes, std::list<std::pair<int, int>>& stackOfEdges,
            std::vector<int>& cutVertices_m, std::vector<Component*>& components_m);
    bool bfsBipartition(int node, std::vector<int>& bipartition);
public:
    Graph(int numberOfNodes);
    void addEdge(int from, int to);
    std::vector<int>& getNeighborsOfNode(int node);
    virtual void print();
    int numberOfNodes();
    BiconnectedComponents* computeBiconnectedComponents();
    std::list<int>* computePathBetweenNodes(int start, int end);
    bool computeBipartition(std::vector<int>& bipartition);
};

class GraphWithLabels : public Graph {
private:
    std::vector<int> nodeLabel_m{};
public:
    GraphWithLabels(std::list<int>& labels);
    GraphWithLabels(std::vector<int>& labels);
    GraphWithLabels(std::initializer_list<int> labels);
    void print() override;
    int getLabelOfNode(int node);
};

#endif