#include "interlacement.hpp"

#include <cassert>

#include "utils.hpp"

InterlacementGraph::InterlacementGraph(const Cycle& cycle, const std::vector<Segment>& segments) 
    : Graph(segments.size()), cycle_m(cycle) , segments_m(segments) {
    computeConflicts();
}

void InterlacementGraph::computeCycleLabels(const Segment& segment, int cycleLabels[]) {
    int originalComponentSize = cycle_m.getOriginalComponentSize();
    bool isCycleNodeAnAttachment[originalComponentSize];
    for (int i = 0; i < originalComponentSize; ++i)
        isCycleNodeAnAttachment[i] = false;
    for (const int attachment : segment.getAttachments())
        isCycleNodeAnAttachment[segment.getLabelOfNode(attachment)] = true;
    int foundAttachments = 0;
    int totalAttachments = segment.getAttachments().size();
    for (int i = 0; i < cycle_m.size(); ++i) {
        int node = cycle_m.nodes()[i];
        if (isCycleNodeAnAttachment[node])
            cycleLabels[node] = 2*(foundAttachments++);
        else
            if (foundAttachments == 0)
                cycleLabels[node] = 2*totalAttachments-1;
            else
                cycleLabels[node] = 2*foundAttachments-1;
    }
    assert(foundAttachments == totalAttachments);
}

void InterlacementGraph::computeConflicts() {
    int cycleLabels[cycle_m.getOriginalComponentSize()];
    for (int i = 0; i < segments_m.size()-1; ++i) {
        const Segment& segment = segments_m[i];
        computeCycleLabels(segment, cycleLabels);
        int numberOfLabels = 2*segment.getAttachments().size();
        int labels[numberOfLabels];
        for (int j = i+1; j < segments_m.size(); ++j) {
            const Segment& otherSegment = segments_m[j];
            for (int k = 0; k < numberOfLabels; ++k)
                labels[k] = 0;
            for (const int attachment : otherSegment.getAttachments()) {
                int attachmentLabel = otherSegment.getLabelOfNode(attachment);
                labels[cycleLabels[attachmentLabel]] = 1;
            }
            int sum = 0;
            for (int k = 0; k < numberOfLabels; ++k)
                sum += labels[k];
            int partSum = labels[0] + labels[1] + labels[2];
            bool areInConflict = true;
            for (int k = 0; k <= numberOfLabels-2; k += 2) {
                if (partSum == sum) {
                    areInConflict = false;
                    break;
                }
                partSum = partSum + labels[(3+k) % numberOfLabels] + labels[(4+k) % numberOfLabels];
                partSum = partSum - labels[k] - labels[(1+k) % numberOfLabels];
            }
            if (areInConflict) addEdge(i, j);
        }
    }   
}