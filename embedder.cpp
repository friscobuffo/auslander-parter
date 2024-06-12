#include "embedder.hpp"

#include <cassert>
#include <iostream>

#include "interlacement.hpp"
#include "utils.hpp"

Embedding::Embedding(int numberOfNodes) : Graph(numberOfNodes) {}

void Embedding::addSingleEdge(int from, int to) {
    neighborsOfNode_m[from].push_back(to);
}

// TODO
Embedding Embedder::mergeBiconnectedComponents(std::vector<std::optional<Embedding>>& embeddings) {
    return Embedding(1);
}

std::optional<Embedding> Embedder::embed(const Graph& graph) {
    if (graph.size() < 4) return baseCaseGraph(graph);
    BiconnectedComponentsHandler bicComps(graph);
    std::vector<std::optional<Embedding>> embeddings{};
    for (const auto& component : bicComps.getComponents()) {
        embeddings.push_back(embed(component));
        if (!embeddings.back().has_value()) return std::nullopt;
    }
    return mergeBiconnectedComponents(embeddings);
}

// TODO
Embedding mergeSegmentsEmbeddings(const Component& component, const Cycle& cycle, std::vector<std::optional<Embedding>>& embeddings) {
    Embedding output(component.size());
    return output;
}

std::optional<Embedding> Embedder::embed(const Component& component, Cycle& cycle) {
    SegmentsHandler segmentsHandler = SegmentsHandler(component, cycle);
    const std::vector<Segment>& segments = segmentsHandler.getSegments();
    if (segments.size() == 0) // entire biconnected component IS the cycle
        return baseCaseCycle(cycle); // base case
    if (segments.size() == 1) {
        const Segment& segment = segments[0];
        if (segment.isPath()) // base case
            return baseCaseSegment(segment);
        // chosen cycle is bad
        makeCycleGood(cycle, segment);
        return embed(component, cycle);
    }
    InterlacementGraph interlacementGraph(cycle, segments);
    std::optional<std::vector<int>> bipartition = interlacementGraph.computeBipartition();
    if (!bipartition) return std::nullopt;
    std::vector<std::optional<Embedding>> embeddings{};
    for (const Segment& segment : segments) {
        embeddings.push_back(embed(segment));
        if (!embeddings.back().has_value()) return std::nullopt;
        for (int i = 0; i < cycle.size(); ++i) {
            assert(segment.getLabelOfNode(i) == cycle.nodes()[i]);
        }
    }
    Embedding output = mergeSegmentsEmbeddings(component, cycle, embeddings);
    return output;
}

std::optional<Embedding> Embedder::embed(const Component& component) {
    if (component.size() < 4) return baseCaseGraph(component); // base case
    Cycle cycle(component);
    return embed(component, cycle);
}

void Embedder::makeCycleGood(Cycle& cycle, const Segment& segment) {
    const std::vector<int>& attachments = segment.getAttachments();
    std::vector<int> attachmentsLabels{};
    for (int attachment : attachments)
        attachmentsLabels.push_back(segment.getLabelOfNode(attachment));
    int foundAttachments = 0;
    int attachmentsToFind = 3;
    int attachmentsToUse[3];
    if (attachmentsLabels.size() == 2) {
        attachmentsToFind = 2;
        attachmentsToUse[2] = -1;
    }
    for (int i = 0; i < cycle.size(); ++i) {
        int node = cycle.nodes()[i];
        int index = findIndex(attachmentsLabels, node);
        if (index == -1) continue;
        attachmentsToUse[foundAttachments++] = attachments[index];
        if (foundAttachments == attachmentsToFind) break;
    }
    if (attachmentsToUse[2] != -1)
        attachmentsToUse[2] = segment.getLabelOfNode(attachmentsToUse[2]);
    assert(foundAttachments == attachmentsToFind);
    std::list<int> path = segment.computePathBetweenAttachments(attachmentsToUse[0], attachmentsToUse[1]);
    for (int& node : path)
        node = segment.getLabelOfNode(node);
    cycle.changeWithPath(path, attachmentsToUse[2]);
}

// base case: graph has <4 nodes
Embedding Embedder::baseCaseGraph(const Graph& graph) {
    assert(graph.size() < 4);
    Embedding embedding(graph.size());
    for (int node = 0; node < graph.size(); ++node) {
        for (int neighbor : graph.getNeighborsOfNode(node))
            if (node < neighbor) embedding.addEdge(node, neighbor);
    }
    return embedding;
}

// base case: segment is a path
Embedding Embedder::baseCaseSegment(const Segment& segment) {
    assert(segment.isPath());
    Embedding embedding(segment.size());
    for (int node = 0; node < segment.size(); ++node) {
        const std::vector<int>& neighbors = segment.getNeighborsOfNode(node);
        if (neighbors.size() == 2) { // attachment nodes will be handled later
            embedding.addSingleEdge(node, neighbors[0]);
            embedding.addSingleEdge(node, neighbors[1]);
        }
    }
    // handling attachment nodes (which have 3 neighbors)
    assert(segment.getAttachments().size() == 2);
    for (int attachment : segment.getAttachments()) {
        int label = segment.getLabelOfNode(attachment);
        int neighbors[3];
        assert(embedding.getNeighborsOfNode(attachment).size() == 0);
        assert(segment.getNeighborsOfNode(attachment).size() == 3);
        for (int i = 0; i < 3; ++i)
            neighbors[i] = -1;
        for (int neighbor : segment.getNeighborsOfNode(attachment)) {
            int neighborLabel = segment.getLabelOfNode(neighbor);
            if (segment.getOriginalCycle().getNextOfNode(label) == neighborLabel) {
                neighbors[0] = neighbor;
                continue;
            }
            if (segment.getOriginalCycle().getPrevOfNode(label) == neighborLabel) {
                neighbors[2] = neighbor;
                continue;
            }
            neighbors[1] = neighbor;
        }
        for (int i = 0; i < 3; ++i) {
            assert(neighbors[i] != -1);
            embedding.addSingleEdge(attachment, neighbors[i]);
        }
    }
    return embedding;
}

// base case: biconnected component is a cycle
Embedding Embedder::baseCaseCycle(const Cycle& cycle) {
    Embedding embedding(cycle.size());
    for (int node = 0; node < cycle.size()-1; ++node)
        embedding.addEdge(node, node+1);
    embedding.addEdge(0, cycle.size()-1);
    return embedding;
}