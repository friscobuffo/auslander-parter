g++ -o main \
    -IOGDF/include \
    -LOGDF \
    main.cpp \
    graph.cpp \
    biconnectedComponent.cpp \
    segment.cpp \
    cycle.cpp \
    graphLoader.cpp \
    interlacement.cpp \
    embedder.cpp \
    -lOGDF -lCOIN