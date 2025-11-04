//
// Created by User on 11/4/2025.
//

#ifndef GRAPH_H
#define GRAPH_H
#include <memory>
#include <string>
#include <vector>

#include "edge.h"

class IDirectedgraph {
public:
    explicit IDirectedgraph(int V, bool directed = true);
    static std::shared_ptr<IDirectedgraph> loadFromFile(const std::string& filename, bool directed = true);
    int V() const;
    int E() const;
    bool isDirected() const;
    void addEdge(const std::shared_ptr<IEdge>& e);
    std::vector<std::shared_ptr<IEdge>> adj(int v) const;
    int outdegree(int v) const;
    int indegree(int v) const;
    std::vector<std::shared_ptr<IEdge>> edges() const;
    std::string toString() const;

};



#endif //GRAPH_H
