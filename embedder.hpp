#ifndef MY_EMBEDDER_H
#define MY_EMBEDDER_H

#include <optional>
#include <vector>

#include "graph.hpp"
#include "biconnectedComponent.hpp"
#include "cycle.hpp"
#include "segment.hpp"

class Embedding : public Graph {
public:
    Embedding(int numberOfNodes);
    void addSingleEdge(int from, int to);
};

class Embedder {
private:
    void makeCycleGood(Cycle& cycle, const Segment& segment);
    Embedding baseCaseGraph(const Graph& graph);
    Embedding baseCaseSegment(const Segment& segment);
    Embedding baseCaseCycle(const Cycle& cycle);
public:
    std::optional<Embedding> embed(const Component& component);
    std::optional<Embedding> embed(const Component& component, Cycle& cycle);
    std::optional<Embedding> embed(const Graph& graph);
    Embedding mergeBiconnectedComponents(std::vector<std::optional<Embedding>>& embeddings);
};

#endif