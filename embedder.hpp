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
    std::optional<Embedding> embed(const Component& component);
    std::optional<Embedding> embed(const Component& component, Cycle& cycle);
    Embedding mergeBiconnectedComponents(std::vector<std::optional<Embedding>>& embeddings);
    void computeMinAndMaxSegmentsAttachments(const std::vector<Segment>& segments,
        int segmentsMinAttachment[], int segmentsMaxAttachment[]);
    std::vector<int> computeOrder(int cycleNode, const std::vector<int>& segmentsIndexes,
    int segmentsMinAttachment[], int segmentsMaxAttachment[], const std::vector<Segment>& segments);
    Embedding mergeSegmentsEmbeddings(const Component& component, const Cycle& cycle,
        const std::vector<std::optional<Embedding>>& embeddings, const std::vector<Segment>& segments,
        const std::vector<int>& bipartition);
public:
    std::optional<Embedding> embed(const Graph& graph);
};

#endif