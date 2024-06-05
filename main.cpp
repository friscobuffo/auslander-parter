#include "graph.hpp"
#include "graphLoader.hpp"
#include "embedder.hpp"
#include <memory>
#include <iostream>

int main(int argc, char* argv[]) {
    GraphLoader loader{};
    Embedder embedder{};
    for (int i = 1; i < argc; ++i) {
        std::unique_ptr<Graph> graph(loader.loadFromFile(argv[i]));
        bool isPlanar = embedder.embed(*graph);
        std::cout << "is planar: " <<isPlanar << "\n";
    }
    return 0;
}