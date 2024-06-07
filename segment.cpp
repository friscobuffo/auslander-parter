#include "segment.hpp"

#include <iostream>
#include <cassert>

#include "cycle.hpp"
#include "biconnectedComponent.hpp"
#include "utils.hpp"

Segment::Segment(Component& component, std::vector<int>& nodesInSegment, std::vector<std::pair<int, int>>& edgesInSegment, Cycle& cycle) 
    : GraphWithLabels(nodesInSegment), originalComponent_m(component) {
    #ifdef DEBUG_MODE
    std::cout << "creating segment\n";
    std::cout << "nodes in segment: ";
    printIterable(nodesInSegment);
    #endif
    int oldToNewLabel[component.numberOfNodes()];
    isNodeAnAttachment_m.resize(nodesInSegment.size());
    isNodeInCycle_m.resize(nodesInSegment.size());
    for (int i = 0; i < nodesInSegment.size(); ++i) {
        isNodeAnAttachment_m[i] = false;
        int node = nodesInSegment[i];
        oldToNewLabel[node] = i;
        if (cycle.hasNode(node))
            isNodeInCycle_m[i] = true;
        else
            isNodeInCycle_m[i] = false;
    }
    for (const auto& edge : edgesInSegment) {
        int node1 = oldToNewLabel[edge.first];
        int node2 = oldToNewLabel[edge.second];
        if (cycle.hasNode(edge.first)) {
            if (!isNodeAnAttachment_m[node1]) {
                attachmentNodes_m.push_back(node1);
                isNodeAnAttachment_m[node1] = true;
            }
        }
        if (cycle.hasNode(edge.second)) {
            if (!isNodeAnAttachment_m[node2]) {
                attachmentNodes_m.push_back(node2);
                isNodeAnAttachment_m[node2] = true;
            }
        }
        addEdge(node1, node2);
    }
    for (int i = 0; i < cycle.size()-1; ++i) {
        int node1 = oldToNewLabel[cycle.getNodeByIndex(i)];
        int node2 = oldToNewLabel[cycle.getNodeByIndex(i+1)];
        addEdge(node1, node2);
    }
    addEdge(oldToNewLabel[cycle.getNodeByIndex(0)], oldToNewLabel[cycle.getNodeByIndex(cycle.size()-1)]);
    #ifdef DEBUG_MODE
    std::cout << "created segment\n";
    std::cout << "attachments of segment: ";
    std::vector<int> attachmentsLabels{};
    for (const int attachment : attachmentNodes_m)
        attachmentsLabels.push_back(getLabelOfNode(attachment));
    printIterable(attachmentsLabels);
    #endif
}

Segment::Segment(Component& component, int from, int to, Cycle& cycle) 
    : GraphWithLabels(cycle.nodes()), originalComponent_m(component) {
    #ifdef DEBUG_MODE
    std::cout << "creating chord\n";
    std::cout << "nodes in chord: ";
    cycle.print();
    std::cout << "chord: [" << from << " - " << to << "]\n";
    #endif
    isNodeAnAttachment_m.resize(cycle.size());
    for (int i = 0; i < cycle.size(); ++i) {
        if ((getLabelOfNode(i) == from) || (getLabelOfNode(i) == to)) {
            isNodeAnAttachment_m[i] = true;
            attachmentNodes_m.push_back(i);
        }
        else
            isNodeAnAttachment_m[i] = false;
    }
    addEdge(attachmentNodes_m[0], attachmentNodes_m[1]);
    assert(attachmentNodes_m.size() == 2);
    for (int i = 0; i < cycle.size()-1; ++i) {
        addEdge(i, i+1);
    }
    addEdge(0, cycle.size()-1);
    #ifdef DEBUG_MODE
    std::cout << "created chord\n";
    #endif
}

bool Segment::isPath() {
    #ifdef DEBUG_MODE
    std::cout << "checking if segment is a path\n";
    #endif
    for (int node = 0; node < numberOfNodes(); ++node) {
        if (isNodeAnAttachment(node)) continue;
        if (getNeighborsOfNode(node).size() > 2)
            return false;
    }
    return true;
}

std::vector<int>& Segment::getAttachments() {
    return attachmentNodes_m;
}

bool Segment::isNodeAnAttachment(int node) {
    return isNodeAnAttachment_m[node];
}

std::list<int>* Segment::computePathBetweenAttachments(int start, int end) {
    int prevOfNode[numberOfNodes()];
    for (int node = 0; node < numberOfNodes(); ++node)
        prevOfNode[node] = -1;
    std::list<int> queue{};
    queue.push_back(start);
    while (queue.size() != 0) {
        int node = queue.front();
        queue.pop_front();
        for (const int neighbor : getNeighborsOfNode(node)) {
            if (isNodeInCycle(node) && isNodeInCycle(neighbor)) continue;
            if (prevOfNode[neighbor] == -1) {
                prevOfNode[neighbor] = node;
                queue.push_back(neighbor);
                if (neighbor == end) break;
            }
        }
        if (prevOfNode[end] != -1) break;
    }
    std::list<int>* path = new std::list<int>();
    int crawl = end;
    while (crawl != start) {
        path->push_front(crawl);
        crawl = prevOfNode[crawl];
    }
    path->push_front(crawl);
    return path;
}

bool Segment::isNodeInCycle(int node) {
    return isNodeInCycle_m[node];
}