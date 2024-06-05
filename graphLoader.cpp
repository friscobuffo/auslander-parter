#include "graphLoader.hpp"

#include <fstream>
#include <iostream>
#include <utility>
#include <string>
#include <sstream>

#include "graph.hpp"

Graph* GraphLoader::loadFromFile(char* path) {
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << path << std::endl;
        return nullptr;
    }
    int nodesNumber{};
    inputFile >> nodesNumber;
    Graph* graph = new Graph(nodesNumber);
    int from, to;
    std::string line;
    while (std::getline(inputFile, line)) {
        if (line.find("//") == 0)
            continue;
        std::istringstream iss(line);
        if (iss >> from >> to)
            graph->addEdge(from, to);
    }
    inputFile.close();
    return graph;
}