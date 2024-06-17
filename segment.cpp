#include "segment.hpp"

#include <iostream>
#include <cassert>

#include "utils.hpp"

Segment::Segment(int numberOfNodes, const Component& component, const Cycle& cycle)
: Component(numberOfNodes, component), originalComponent_m(component), originalCycle_m(cycle) {
    isNodeAnAttachment_m.resize(numberOfNodes);
    for (int i = 0; i < numberOfNodes; ++i)
        isNodeAnAttachment_m[i] = false;
}

void Segment::addAttachment(int attachment) {
    if (isNodeAnAttachment(attachment)) return;
    isNodeAnAttachment_m[attachment] = true;
    attachmentNodes_m.push_back(attachment);
}

bool Segment::isNodeAnAttachment(int node) const {
    return isNodeAnAttachment_m[node];
}

bool Segment::isPath() const {
    for (int node = 0; node < size(); ++node) {
        if (isNodeAnAttachment(node)) continue;
        if (getNeighborsOfNode(node).size() > 2)
            return false;
    }
    return true;
}

const std::vector<int>& Segment::getAttachments() const {
    return attachmentNodes_m;
}

std::list<int> Segment::computePathBetweenAttachments(int start, int end) const {
    assert(isNodeAnAttachment(start));
    assert(isNodeAnAttachment(end));
    int prevOfNode[size()];
    for (int node = 0; node < size(); ++node)
        prevOfNode[node] = -1;
    std::list<int> queue{};
    queue.push_back(start);
    while (queue.size() != 0) {
        int node = queue.front();
        queue.pop_front();
        for (const int neighbor : getNeighborsOfNode(node)) {
            if (originalCycle_m.hasNode(getLabelOfNode(node)) && originalCycle_m.hasNode(getLabelOfNode(neighbor)))
                continue;
            if (prevOfNode[neighbor] == -1) {
                prevOfNode[neighbor] = node;
                queue.push_back(neighbor);
                if (neighbor == end) break;
            }
        }
        if (prevOfNode[end] != -1) break;
    }
    std::list<int> path{};
    int crawl = end;
    while (crawl != start) {
        path.push_front(crawl);
        crawl = prevOfNode[crawl];
    }
    path.push_front(crawl);
    return path;
}

const Cycle& Segment::getOriginalCycle() const {
    return originalCycle_m;
}

const Component& Segment::getOriginalComponent() const {
    return originalComponent_m;
}

SegmentsHandler::SegmentsHandler(const Component& component, const Cycle& cycle)
: originalComponent_m(component), originalCycle_m(cycle) {
    findSegments();
    findChords();
}

void SegmentsHandler::dfsFindSegments(int node, bool isNodeVisited[], std::vector<int>& nodesInSegment,
std::vector<std::pair<int, int>>& edgesInSegment) {
    nodesInSegment.push_back(node);
    isNodeVisited[node] = true;
    for (const int neighbor : originalComponent_m.getNeighborsOfNode(node)) {
        if (originalCycle_m.hasNode(neighbor)) {
            edgesInSegment.push_back(std::make_pair(node, neighbor));
            continue;
        }
        if (node < neighbor)
            edgesInSegment.push_back(std::make_pair(node, neighbor));
        if (!isNodeVisited[neighbor])
            dfsFindSegments(neighbor, isNodeVisited, nodesInSegment, edgesInSegment);
    }
}

void SegmentsHandler::findChords() {
    for (int i = 0; i < originalCycle_m.size(); ++i) {
        int node = originalCycle_m.nodes()[i];
        for (const int neighbor : originalComponent_m.getNeighborsOfNode(node)) {
            if (node < neighbor) continue;
            if (originalCycle_m.hasNode(neighbor))
                if (neighbor != originalCycle_m.getPrevOfNode(node) && neighbor != originalCycle_m.getNextOfNode(node))
                    segments_m.push_back(buildChord(node, neighbor));
        }
    }
}

void SegmentsHandler::findSegments() {
    bool isNodeVisited[originalComponent_m.size()];
    for (int node = 0; node < originalComponent_m.size(); ++node)
        isNodeVisited[node] = false;
    for (int node = 0; node < originalComponent_m.size(); ++node)
        if (originalCycle_m.hasNode(node))
            isNodeVisited[node] = true;    
    for (int node = 0; node < originalComponent_m.size(); ++node) {
        if (!isNodeVisited[node]) {
            std::vector<int> nodes{}; // does NOT contain cycle nodes
            std::vector<std::pair<int, int>> edges{}; // does NOT contain edges of the cycle
            dfsFindSegments(node, isNodeVisited, nodes, edges);
            segments_m.push_back(buildSegment(nodes, edges));
        }
    }
}

// nodes vector does NOT contain cycle nodes
// edges vector does NOT contain cycle edges
Segment SegmentsHandler::buildSegment(std::vector<int>& nodes, std::vector<std::pair<int, int>>& edges) {
    Segment segment(nodes.size()+originalCycle_m.size(), originalComponent_m, originalCycle_m);
    // assigning labels
    // first nodes MUST be the same of the cycle in the SAME ORDER
    int oldToNewLabel[originalComponent_m.size()];
    for (int i = 0; i < originalCycle_m.size(); ++i) {
        int cycleNode = originalCycle_m.nodes()[i];
        oldToNewLabel[cycleNode] = i;
        segment.assignNodeLabel(i, cycleNode);
    }
    for (int i = 0; i < nodes.size(); ++i) { // remember that nodes does not include cycle nodes
        int node = nodes[i];
        oldToNewLabel[node] = i+originalCycle_m.size();
        segment.assignNodeLabel(i+originalCycle_m.size(), node);
    }
    // adding edges
    for (auto& edge : edges) {
        int from = oldToNewLabel[edge.first];
        int to = oldToNewLabel[edge.second];
        segment.addEdge(from, to);
        // adding attachment
        if (originalCycle_m.hasNode(edge.first))
            segment.addAttachment(from);
        if (originalCycle_m.hasNode(edge.second))
            segment.addAttachment(to);
    }
    // adding cycle edges
    for (int i = 0; i < originalCycle_m.size()-1; ++i)
        segment.addEdge(i, i+1);
    segment.addEdge(0, originalCycle_m.size()-1);
    return segment;
}

Segment SegmentsHandler::buildChord(int attachment1, int attachment2) {
    Segment chord(originalCycle_m.size(), originalComponent_m, originalCycle_m);
    // assigning labels
    // first nodes MUST be the same of the cycle in the SAME ORDER
    for (int i = 0; i < originalCycle_m.size(); ++i)
        chord.assignNodeLabel(i, originalCycle_m.nodes()[i]);
    // adding cycle edges
    for (int i = 0; i < originalCycle_m.size()-1; ++i)
        chord.addEdge(i, i+1);
    chord.addEdge(0, originalCycle_m.size()-1);
    // adding chord edge
    std::optional<int> from = originalCycle_m.getIndexOfNode(attachment1);
    std::optional<int> to = originalCycle_m.getIndexOfNode(attachment2);
    assert(from);
    assert(to);
    chord.addEdge(from.value(), to.value());
    chord.addAttachment(from.value());
    chord.addAttachment(to.value());
    return chord;
}

const std::vector<Segment> SegmentsHandler::getSegments() {
    return segments_m;
}