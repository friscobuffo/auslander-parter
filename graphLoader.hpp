#ifndef MY_GRAPH_LOADER_H
#define MY_GRAPH_LOADER_H

class Graph;

class GraphLoader {
public:
    Graph* loadFromFile(char* path);
};

#endif