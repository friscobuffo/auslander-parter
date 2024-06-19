#ifndef MY_GRAPH_H
#define MY_GRAPH_H

#include <vector>
#include <optional>

class MyGraph {
private:
    int numberOfNodes_m{};

    bool bfsBipartition(int node, std::vector<int>& bipartition) const;

protected:
    std::vector<std::vector<int>> neighborsOfNode_m{};

public:
    MyGraph(int numberOfNodes);

    void addEdge(int from, int to);
    const std::vector<int>& getNeighborsOfNode(int node) const;
    virtual void print() const;
    int size() const;
    const std::optional<std::vector<int>> computeBipartition() const;
};

#endif