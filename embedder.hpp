#ifndef MY_EMBEDDER_H
#define MY_EMBEDDER_H

#include <optional>
#include <vector>
#include <string>

#include "graph.hpp"
#include "biconnectedComponent.hpp"
#include "cycle.hpp"
#include "segment.hpp"

class Embedding : public MyGraph {
public:
    Embedding(int numberOfNodes);

    void addSingleEdge(int from, int to);
    void saveToSvg(std::string& path) const;
};

class Embedder {
private:
    void makeCycleGood(Cycle& cycle, const Segment& segment);
    const Embedding baseCaseGraph(const MyGraph& graph);
    const Embedding baseCaseSegment(const Segment& segment);
    const Embedding baseCaseCycle(const Cycle& cycle);
    std::optional<const Embedding> embed(const Component& component);
    std::optional<const Embedding> embed(const Component& component, Cycle& cycle);
    void computeMinAndMaxSegmentsAttachments(const std::vector<Segment>& segments,
        int segmentsMinAttachment[], int segmentsMaxAttachment[]);
    const std::vector<int> computeOrder(int cycleNode, const std::vector<int>& segmentsIndexes,
        int segmentsMinAttachment[], int segmentsMaxAttachment[], const std::vector<Segment>& segments);
    const Embedding mergeSegmentsEmbeddings(const Component& component, const Cycle& cycle,
        const std::vector<std::optional<Embedding>>& embeddings, const std::vector<Segment>& segments,
        const std::vector<int>& bipartition);

public:
    std::optional<const Embedding> embed(const MyGraph& graph);
};

#endif