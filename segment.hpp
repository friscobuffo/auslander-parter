#ifndef MY_SEGMENT_H
#define MY_SEGMENT_H

#include <vector>
#include <utility>

#include "graph.hpp"

class Cycle;
class Component;

class Segment : public GraphWithLabels {
private:
    std::vector<bool> isNodeAnAttachment_m{};
    std::vector<bool> isNodeInCycle_m{};
    std::vector<int> attachmentNodes_m{};
    Component& originalComponent_m;
public:
    Segment(Component& component, std::vector<int>& nodesInSegment, std::vector<std::pair<int, int>>& edgesInSegment, Cycle& cycle);
    Segment(Component& component, int from, int to, Cycle& cycle);
    bool isPath();
    std::vector<int>& getAttachments();
    bool isNodeAnAttachment(int node);
    std::list<int>* computePathBetweenAttachments(int start, int end);
    bool isNodeInCycle(int node);
};

#endif