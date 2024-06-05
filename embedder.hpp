#ifndef MY_EMBEDDER_H
#define MY_EMBEDDER_H

class Cycle;
class Graph;
class Segment;
class Component;

class Embedder {
private:
    void makeCycleGood(Cycle& cycle, Segment& segment);
public:
    bool embed(Graph& graph);
    bool embed(Component& component);
};

#endif