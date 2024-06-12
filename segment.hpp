#ifndef MY_SEGMENT_H
#define MY_SEGMENT_H

#include <vector>
#include <utility>
#include <list>

#include "cycle.hpp"
#include "biconnectedComponent.hpp"

class Segment : public Component {
private:
    std::vector<int> attachmentNodes_m{};
    std::vector<bool> isNodeAnAttachment_m{};
    const Component& originalComponent_m;
    const Cycle& originalCycle_m;
public:
    Segment(int numberOfNodes, const Component& originalComponent, const Cycle& cycle);
    bool isPath() const;
    const std::vector<int>& getAttachments() const;
    void addAttachment(int attachment);
    bool isNodeAnAttachment(int node) const;
    std::list<int> computePathBetweenAttachments(int start, int end) const;
    const Cycle& getOriginalCycle() const;
    const Component& getOriginalComponent();
};

class SegmentsHandler {
private:
    std::vector<Segment> segments_m{};
    const Cycle& originalCycle_m;
    const Component& originalComponent_m;
    Segment buildSegment(std::vector<int>& nodes, std::vector<std::pair<int, int>>& edges);
    Segment buildChord(int attachment1, int attachment2);
    void dfsFindSegments(int node, bool isNodeVisited[], std::vector<int>& nodesInSegment, std::vector<std::pair<int, int>>& edgesInSegment);
    void findSegments();
    void findChords();
public:
    SegmentsHandler(const Component& component, const Cycle& cycle);
    const std::vector<Segment> getSegments();
};

#endif