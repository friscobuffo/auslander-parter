#include "embedder.hpp"

#include <cassert>
#include <iostream>

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/planarlayout/PlanarStraightLayout.h>
#include <ogdf/planarlayout/PlanarDrawLayout.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/planarity/EmbedderModule.h>

#include "interlacement.hpp"
#include "utils.hpp"

using namespace ogdf;

Embedding::Embedding(int numberOfNodes) : MyGraph(numberOfNodes) {}

void Embedding::addSingleEdge(int from, int to) {
    neighborsOfNode_m[from].push_back(to);
}

Embedding mergeBiconnectedComponents(const MyGraph& graph, const std::vector<Component>& components,
const std::vector<std::optional<Embedding>>& embeddings) {
    Embedding output(graph.size());
    assert(components.size() == embeddings.size());
    for (int i = 0; i < components.size(); ++i) {
        assert(embeddings[i].has_value());
        const Component& component = components[i];
        const Embedding& embedding = embeddings[i].value();
        for (int node = 0; node < embedding.size(); ++node) {
            int label = component.getLabelOfNode(node);
            for (int neighbor : embedding.getNeighborsOfNode(node)) {
                int neighborLabel = component.getLabelOfNode(neighbor);
                output.addSingleEdge(label, neighborLabel);
            }
        }
    }
    return output;
}

Graph myGraphToOgdf(const MyGraph& myGraph) {
    Graph graph;
    std::vector<node> nodes(myGraph.size());
    for (int n = 0; n < myGraph.size(); ++n)
        nodes[n] = graph.newNode();
    for (int n = 0; n < myGraph.size(); ++n)
        for (int neighbor : myGraph.getNeighborsOfNode(n))
            if (n < neighbor)
                graph.newEdge(nodes[n], nodes[neighbor]);
    return graph;
}

Graph embeddingToOgdfGraph(const Embedding& embedding) {
    Graph graph = myGraphToOgdf(embedding);
    std::vector<int> position(embedding.size());
    for (node n : graph.nodes) {
        const int label = n->index();
        const std::vector<int>& neighbors = embedding.getNeighborsOfNode(label);
        for (int i = 0; i < neighbors.size(); ++i)
            position[neighbors[i]] = i;
        std::vector<adjEntry> order(neighbors.size());
        for (adjEntry& adj : n->adjEntries) {
            const int neighbor = adj->twinNode()->index();
            order[position[neighbor]] = adj;
        }
        List<adjEntry> newOrder;
        for (adjEntry& adj : order)
            newOrder.pushBack(adj);
        graph.sort(n, newOrder);
    }
    return graph;
}

void Embedding::saveToSvg(std::string& outputPath) const {
    Graph graph = embeddingToOgdfGraph(*this);
    GraphAttributes GA(graph, GraphAttributes::nodeGraphics | GraphAttributes::edgeGraphics |
                        GraphAttributes::nodeLabel | GraphAttributes::edgeStyle |
                        GraphAttributes::nodeStyle | GraphAttributes::edgeArrow);
    for (node v : graph.nodes) {
        GA.label(v) = std::to_string(v->index());
        GA.shape(v) = Shape::Ellipse;
    }
    for (edge e : graph.edges) {
        GA.strokeWidth(e) = 1.5;
        GA.arrowType(e) = EdgeArrow::None;
    }
    PlanarDrawLayout layout;
    layout.call(GA);
    GraphIO::write(GA, outputPath);
}


std::optional<const Embedding> Embedder::embed(const MyGraph& graph) {
    if (graph.size() < 4) return baseCaseGraph(graph);
    const BiconnectedComponentsHandler bicComps(graph);
    std::vector<std::optional<Embedding>> embeddings{};
    for (const auto& component : bicComps.getComponents()) {
        embeddings.push_back(embed(component));
        if (!embeddings.back().has_value()) return std::nullopt;
    }
    return mergeBiconnectedComponents(graph, bicComps.getComponents(), embeddings);
}

// for each segment, it computes the minimum and the maximum of all of its attachments
void Embedder::computeMinAndMaxSegmentsAttachments(const std::vector<Segment>& segments,
int segmentsMinAttachment[], int segmentsMaxAttachment[]) {
    for (int i = 0; i < segments.size(); i++) {
        int min = segments[i].size();
        int max = 0;
        for (int attachment : segments[i].getAttachments()) {
            if (attachment < min) min = attachment;
            if (attachment > max) max = attachment;
        }
        segmentsMinAttachment[i] = min;
        segmentsMaxAttachment[i] = max;
    }
}

const std::vector<int> Embedder::computeOrder(int cycleNode, const std::vector<int>& segmentsIndexes,
int segmentsMinAttachment[], int segmentsMaxAttachment[], const std::vector<Segment>& segments) {
    std::optional<int> middleSegment;
    std::vector<int> minSegments{};
    std::vector<int> maxSegments{};
    for (int i = 0; i < segmentsIndexes.size(); ++i) {
        int segIndex = segmentsIndexes[i];
        if (segmentsMinAttachment[segIndex] == cycleNode) {
            minSegments.push_back(segIndex);
            continue;
        }
        if (segmentsMaxAttachment[segIndex] == cycleNode) {
            maxSegments.push_back(segIndex);
            continue;
        }
        assert(!middleSegment.has_value());
        middleSegment = segIndex;
    }
    // ordering maxSegments
    for (int i = 0; i < int(maxSegments.size())-1; ++i) {
        int min = maxSegments[i];
        for (int j = i+1; j < maxSegments.size(); ++j) {
            int candidate = maxSegments[j];
            if (segmentsMinAttachment[candidate] > segmentsMinAttachment[min])
                continue;
            if (segmentsMinAttachment[candidate] < segmentsMinAttachment[min]) {
                min = maxSegments[candidate];
                continue;
            }
            int numAttachmentsMin = segments[min].getAttachments().size();
            int numAttachmentsCandidate = segments[candidate].getAttachments().size();
            assert(numAttachmentsMin == 2 || numAttachmentsMin == 3);
            assert(numAttachmentsCandidate == 2 || numAttachmentsCandidate == 3);
            if (numAttachmentsMin == 2 && numAttachmentsCandidate == 2) {
                if (min > candidate)
                    continue;
                min = candidate;
                continue;
            }
            if (numAttachmentsCandidate == 3) {
                assert(numAttachmentsMin == 2);
                min = candidate;
            }
        }
        int temp = maxSegments[min];
        maxSegments[min] = maxSegments[i];
        maxSegments[i] = temp;
    }
    // ordering minSegments
    for (int i = 0; i < int(minSegments.size())-1; ++i) {
        int max = minSegments[i];
        for (int j = i+1; j < minSegments.size(); ++j) {
            int candidate = minSegments[j];
            if (segmentsMaxAttachment[candidate] < segmentsMaxAttachment[max])
                continue;
            if (segmentsMaxAttachment[candidate] > segmentsMaxAttachment[max]) {
                max = minSegments[candidate];
                continue;
            }
            int numAttachmentsMax = segments[max].getAttachments().size();
            int numAttachmentsCandidate = segments[candidate].getAttachments().size();
            assert(numAttachmentsMax == 2 || numAttachmentsMax == 3);
            assert(numAttachmentsCandidate == 2 || numAttachmentsCandidate == 3);
            if (numAttachmentsMax == 2 && numAttachmentsCandidate == 2) {
                if (max > candidate)
                    continue;
                max = candidate;
                continue;
            }
            if (numAttachmentsMax == 3) {
                assert(numAttachmentsCandidate == 2);
                max = candidate;
            }
        }
        int temp = minSegments[max];
        minSegments[max] = minSegments[i];
        minSegments[i] = temp;
    }
    std::vector<int> order{};
    for (int segmentIndex : maxSegments)
        order.push_back(segmentIndex);
    if (middleSegment) order.push_back(middleSegment.value());
    for (int segmentIndex : minSegments)
        order.push_back(segmentIndex);
    assert(order.size() == segmentsIndexes.size());
    return order;
}

const Embedding Embedder::mergeSegmentsEmbeddings(const Component& component, const Cycle& cycle,
const std::vector<std::optional<Embedding>>& embeddings, const std::vector<Segment>& segments,
const std::vector<int>& bipartition) {
    Embedding output(component.size());
    int segmentsMinAttachment[segments.size()];
    int segmentsMaxAttachment[segments.size()];
    computeMinAndMaxSegmentsAttachments(segments, segmentsMinAttachment, segmentsMaxAttachment);
    for (int node = 0; node < cycle.size(); ++node) {
        std::vector<int> insideSegments{};
        std::vector<int> outsideSegments{};
        for (int i = 0; i < segments.size(); ++i)
            if (segments[i].isNodeAnAttachment(node)) {
                if (bipartition[i] == 0) insideSegments.push_back(i);
                else outsideSegments.push_back(i);
            }
        int cycleNodeLabel = cycle.nodes()[node];
        int prevCycleNodeLabel = cycle.getPrevOfNode(cycleNodeLabel);
        int nextCycleNodeLabel = cycle.getNextOfNode(cycleNodeLabel);
        // order of the segments inside the cycle
        std::vector<int> insideOrder = computeOrder(node, insideSegments, segmentsMinAttachment, segmentsMaxAttachment, segments);
        // order of the segments outside the cycle
        std::vector<int> outsideOrder = computeOrder(node, outsideSegments, segmentsMinAttachment, segmentsMaxAttachment, segments);
        output.addSingleEdge(cycleNodeLabel, nextCycleNodeLabel);
        for (int i = 0; i < insideOrder.size(); ++i) {
            const Segment& segment = segments[insideOrder[i]];
            const Embedding& embedding = embeddings[insideOrder[i]].value();
            assert(segment.isNodeAnAttachment(node));
            for (int neighbor : embedding.getNeighborsOfNode(node)) {
                int label = segment.getLabelOfNode(neighbor);
                if (cycle.getNextOfNode(cycleNodeLabel) == label) continue;
                if (cycle.getPrevOfNode(cycleNodeLabel) == label) continue;
                output.addSingleEdge(cycleNodeLabel, label);
            }
        }
        output.addSingleEdge(cycleNodeLabel, prevCycleNodeLabel);
        for (int i = 0; i < outsideOrder.size(); ++i) {
            const Segment& segment = segments[outsideOrder[i]];
            const Embedding& embedding = embeddings[outsideOrder[i]].value();
            for (int neighbor : embedding.getNeighborsOfNode(node)) {
                int label = segment.getLabelOfNode(neighbor);
                if (cycle.getNextOfNode(cycleNodeLabel) == label) continue;
                if (cycle.getPrevOfNode(cycleNodeLabel) == label) continue;
                output.addSingleEdge(cycleNodeLabel, label);
            }
        }
    }
    for (int i = 0; i < segments.size(); ++i) {
        const Segment& segment = segments[i];
        const Embedding& embedding = embeddings[i].value();
        for (int node = 0; node < segment.size(); ++node) {
            int label = segment.getLabelOfNode(node);
            if (cycle.hasNode(label)) continue;
            for (int neighbor : embedding.getNeighborsOfNode(node)) {
                int neighborLabel = segment.getLabelOfNode(neighbor);
                output.addSingleEdge(label, neighborLabel);
            }
        } 
    }
    return output;
}

std::optional<const Embedding> Embedder::embed(const Component& component, Cycle& cycle) {
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
    return mergeSegmentsEmbeddings(component, cycle, embeddings, segments, bipartition.value());
}

std::optional<const Embedding> Embedder::embed(const Component& component) {
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
const Embedding Embedder::baseCaseGraph(const MyGraph& graph) {
    assert(graph.size() < 4);
    Embedding embedding(graph.size());
    for (int node = 0; node < graph.size(); ++node) {
        for (int neighbor : graph.getNeighborsOfNode(node))
            if (node < neighbor) embedding.addEdge(node, neighbor);
    }
    return embedding;
}

// base case: segment is a path
const Embedding Embedder::baseCaseSegment(const Segment& segment) {
    assert(segment.size() == segment.getOriginalComponent().size());
    assert(segment.isPath());
    Embedding embedding(segment.size());
    for (int node = 0; node < segment.size(); ++node) {
        int label = segment.getLabelOfNode(node);
        const std::vector<int>& neighbors = segment.getNeighborsOfNode(node);
        if (neighbors.size() == 2) { // attachment nodes will be handled later
            
            embedding.addSingleEdge(label, segment.getLabelOfNode(neighbors[0]));
            embedding.addSingleEdge(label, segment.getLabelOfNode(neighbors[1]));
        }
    }
    // handling attachment nodes (which have 3 neighbors)
    assert(segment.getAttachments().size() == 2);
    for (int attachment : segment.getAttachments()) {
        int label = segment.getLabelOfNode(attachment);
        int neighbors[3];
        assert(embedding.getNeighborsOfNode(label).size() == 0);
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
            embedding.addSingleEdge(label, segment.getLabelOfNode(neighbors[i]));
        }
    }
    return embedding;
}

// base case: biconnected component is a cycle
const Embedding Embedder::baseCaseCycle(const Cycle& cycle) {
    Embedding embedding(cycle.size());
    for (int node = 1; node < cycle.size()-1; ++node)
        embedding.addEdge(cycle.nodes()[node], cycle.nodes()[node+1]);
    embedding.addEdge(cycle.nodes()[0], cycle.nodes()[cycle.size()-1]);
    return embedding;
}