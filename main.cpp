#include "graph.hpp"
#include "graphLoader.hpp"

#include <memory>
#include <iostream>
#include <optional>

#include "embedder.hpp"

int main(int argc, char* argv[]) {
    GraphLoader loader{};
    Embedder embedder{};
    for (int i = 1; i < argc; ++i) {
        Graph graph = loader.loadFromFile(argv[i]);
        graph.print();
        std::optional<Embedding> embedding = embedder.embed(graph);
        std::cout << std::boolalpha << "graph is planar: " << embedding.has_value() << "\n";
    }
    return 0;
}