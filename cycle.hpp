#ifndef MY_CYCLE_H
#define MY_CYCLE_H

#include <vector>
#include <list>

class Component;

class Cycle {
private:
    std::vector<int> nodes_m{};
    std::vector<int> posInCycle_m{};
    Component& originalComponent_m;

public:
    Cycle(Component& component, std::list<int>& listOfNodes);
    void changeWithPath(std::list<int>& path, int nodeToInclude);
    bool hasNode(int node);
    int size();
    int getPrevOfNode(int node);
    int getNextOfNode(int node);
    void nextIndex(int& index);
    std::vector<int>& nodes();
    void reverse();
    void print();
    int getNodeByIndex(int index);
    int getOriginalComponentSize();
};

#endif