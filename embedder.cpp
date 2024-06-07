#include "embedder.hpp"

#include <memory>
#include <cassert>
#include <vector>
#include <iostream>
#include <unordered_set>

#include "graph.hpp"
#include "biconnectedComponent.hpp"
#include "interlacement.hpp"
#include "segment.hpp"
#include "cycle.hpp"
#include "utils.hpp"

bool Embedder::embed(Graph& graph) {
    if (graph.numberOfNodes() < 5) return true;
    #ifdef DEBUG_MODE
    std::cout << "embedding graph:\n";
    graph.print();
    #endif
    std::unique_ptr<BiconnectedComponents> bicCompsPtr(graph.computeBiconnectedComponents());
    for (const auto& component : bicCompsPtr->getComponents())
        if (!embed(*component)) return false;
    return true;
}

bool Embedder::embed(Component& component) {
    if (component.numberOfNodes() < 5) return true;
    #ifdef DEBUG_MODE
    std::cout << "embedding biconnected component:\n";
    component.print();
    std::cout << "need to find cycle\n";
    #endif
    std::unique_ptr<Cycle> cyclePtr (component.findCycle());
    Cycle& cycle = *cyclePtr.get();
    #ifdef DEBUG_MODE
    std::cout << "found cycle: ";
    cycle.print();
    #endif
    std::vector<std::unique_ptr<Segment>> segments{};
    component.findSegments(cycle, segments);
    component.findChords(cycle, segments);
    #ifdef DEBUG_MODE
    std::cout << "found segments:\n";
    int c = 0;
    for (const auto& segment : segments) {
        std::cout << "segment [" << c++ << "]\n";
        segment->print();
    }
    #endif
    if (segments.size() == 0) return true;
    if (segments.size() == 1) {
        #ifdef DEBUG_MODE
        std::cout << "found only 1 segment:\n";
        #endif
        Segment& segment = *segments.front();
        if (segment.isPath()) {// its good (base case)
            #ifdef DEBUG_MODE
            std::cout << "the segment is a path: good base case\n";
            #endif
            return true;
        }
        // chosen cycle is bad
        #ifdef DEBUG_MODE
        std::cout << "the segment is not a path: need to recompute cycle\n";
        #endif
        makeCycleGood(cycle, segment);
        #ifdef DEBUG_MODE
        std::cout << "new cycle: ";
        cycle.print();
        #endif
        segments.clear();
        component.findSegments(cycle, segments);
        component.findChords(cycle, segments);
        #ifdef DEBUG_MODE
        std::cout << "new found segments:\n";
        int c = 0;
        for (const auto& segment : segments) {
            std::cout << "segment [" << c++ << "]\n";
            segment->print();
        }
        #endif
        assert(segments.size() > 1);
    }
    // cycle now is good 100%
    InterlacementGraph interlacementGraph(cycle, segments);
    std::vector<int> bipartition{};
    bool isBipartite = interlacementGraph.computeBipartition(bipartition);
    if (!isBipartite) return false;
    for (const auto& segment : segments) {
        if (!embed(*segment)) return false;
    }
    return true;
}

void Embedder::makeCycleGood(Cycle& cycle, Segment& segment) {
    #ifdef DEBUG_MODE
    std::cout << "recomputing cycle\n";
    #endif
    std::vector<int>& attachments = segment.getAttachments();
    std::vector<int> attachmentsLabels{};
    for (int attachment : attachments)
        attachmentsLabels.push_back(segment.getLabelOfNode(attachment));
    // this doesnt work now NEED TO BE CHANGED
    int foundAttachments = 0;
    int attachmentsToFind = 3;
    int attachmentsToUse[3];
    if (attachmentsLabels.size() == 2) {
        attachmentsToFind = 2;
        attachmentsToUse[2] = -1;
    }
    for (int i = 0; i < cycle.size(); ++i) {
        int node = cycle.getNodeByIndex(i);
        int index = findIndex(attachmentsLabels, node);
        if (index == -1) continue;
        attachmentsToUse[foundAttachments++] = attachments[index];
        if (foundAttachments == attachmentsToFind) break;
    }
    if (attachmentsToUse[2] != -1)
        attachmentsToUse[2] = segment.getLabelOfNode(attachmentsToUse[2]);
    assert(foundAttachments == attachmentsToFind);
    #ifdef DEBUG_MODE
    std::cout << "found attachments to use: [ ";
    std::cout << segment.getLabelOfNode(attachmentsToUse[0]) << " ";
    std::cout << segment.getLabelOfNode(attachmentsToUse[1]) << " ";
    std::cout << attachmentsToUse[2] << " ]\n";
    #endif
    std::unique_ptr<std::list<int>> path(segment.computePathBetweenAttachments(attachmentsToUse[0], attachmentsToUse[1]));
    for (int& node : *path)
        node = segment.getLabelOfNode(node);
    #ifdef DEBUG_MODE
    std::cout << "path between first and second in segment:";
    printIterable(*path);
    #endif
    cycle.changeWithPath(*path, attachmentsToUse[2]);
}