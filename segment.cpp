#include "segment.hpp"

#include <cassert>

#include "cycle.hpp"
#include "biconnectedComponent.hpp"

Segment::Segment(Component& component, std::vector<int>& nodesInSegment, std::vector<std::pair<int, int>>& edgesInSegment, Cycle& cycle) 
    : GraphWithLabels(nodesInSegment), originalComponent_m(component) {
    int oldToNewLabel[component.numberOfNodes()];
    isNodeAnAttachment_m.resize(nodesInSegment.size());
    for (int i = 0; i < nodesInSegment.size(); ++i)
        isNodeAnAttachment_m[i] = false;
    for (int i = 0; i < nodesInSegment.size(); ++i)
        oldToNewLabel[nodesInSegment[i]] = i;
    for (const int node : nodesInSegment)
        if (cycle.hasNode(node))
            isNodeAnAttachment_m[oldToNewLabel[node]] = true;
    for (const int node : nodesInSegment)
        if (isNodeAnAttachment_m[oldToNewLabel[node]])
            attachmentNodes_m.push_back(oldToNewLabel[node]);
    for (const auto& edge : edgesInSegment)
        addEdge(oldToNewLabel[edge.first], oldToNewLabel[edge.second]);
}

Segment::Segment(Component& component, int from, int to) 
    : GraphWithLabels({from, to}), originalComponent_m(component) {
    isNodeAnAttachment_m.resize(2);
    isNodeAnAttachment_m[0] = true;
    isNodeAnAttachment_m[1] = true;
    addEdge(0, 1);
    attachmentNodes_m.resize(2);
    attachmentNodes_m[0] = 0;
    attachmentNodes_m[1] = 1;
    assert(attachmentNodes_m.size() == 2);
}

bool Segment::isPath() {
    if (numberOfNodes() == 2) // segment is a chord
        return true;
    for (int node = 0; node < numberOfNodes(); ++node)
        if (getNeighborsOfNode(node).size() > 2)
            return false;
    return true;
}

std::vector<int>& Segment::getAttachments() {
    return attachmentNodes_m;
}

bool Segment::isNodeAnAttachment(int node) {
    return isNodeAnAttachment_m[node];
}