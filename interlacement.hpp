#ifndef MY_INTERLACEMENT_H
#define MY_INTERLACEMENT_H

#include <vector>
#include <memory>

#include "graph.hpp"

class Segment;
class Cycle;

class InterlacementGraph : public Graph {
private:
    Cycle& cycle_m;
    std::vector<Segment*> segments_m{};
    void computeConflicts();
    void computeCycleLabels(Segment& segment, int cycleLabels[]);
public:
    InterlacementGraph(Cycle& cycle, std::vector<std::unique_ptr<Segment>>& segments);
    bool isBipartite();
};

#endif