#ifndef MY_GRAPH_LOADER_H
#define MY_GRAPH_LOADER_H

class MyGraph;

class GraphLoader {
public:
    const MyGraph loadFromFile(char* path);
};

#endif