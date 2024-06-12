#ifndef MY_INTERLACEMENT_H
#define MY_INTERLACEMENT_H

#include <vector>

#include "graph.hpp"
#include "segment.hpp"
#include "cycle.hpp"

class InterlacementGraph : public Graph {
private:
    const Cycle& cycle_m;
    const std::vector<Segment>& segments_m;
    void computeConflicts();
    void computeCycleLabels(const Segment& segment, int cycleLabels[]);
public:
    InterlacementGraph(const Cycle& cycle, const std::vector<Segment>& segments);
};

#endif