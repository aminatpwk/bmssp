#ifndef DIJKSTRA_H
#define DIJKSTRA_H
#include <limits>
#include <memory>
#include <vector>

#include "graphs/Directedgraph.h"
#include "graphs/edge.h"


class dijkstra {
public:
    //all the computing happens inside constructor
    dijkstra(const std::shared_ptr<IDirectedgraph>& G, int s);
    double distTo(int v) const;
    bool hasPathTo(int v) const;
    std::vector<std::shared_ptr<IEdge>> pathTo(int v) const;

private:
    std::vector<double> distTo_;
    std::vector<std::shared_ptr<IEdge>> edgeTo_;
    const std::shared_ptr<IDirectedgraph> G_;
    int s_;
    static constexpr double INF = std::numeric_limits<double>::infinity();

    void validateVertex(int v) const;
};



#endif //DIJKSTRA_H
