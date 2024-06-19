#include <iostream>
#include <optional>
#include <string>

#include "graph.hpp"
#include "graphLoader.hpp"
#include "embedder.hpp"

int main(int argc, char* argv[]) {
    GraphLoader loader{};
    Embedder embedder{};
    int index = 0;
    for (int i = 1; i < argc; ++i) {
        MyGraph graph = loader.loadFromFile(argv[i]);
        std::cout << "graph:\n";
        graph.print();
        std::optional<Embedding> embedding = embedder.embed(graph);
        std::cout << std::boolalpha << "graph is planar: " << embedding.has_value() << ".\n";
        if (embedding.has_value()) {
            std::cout << "embedding:\n";
            embedding.value().print();
            std::string path = "embedding" + std::to_string(++index) + ".svg";
            embedding.value().saveToSvg(path);
        }
        std::cout << "\n";
    }
    return 0;
}