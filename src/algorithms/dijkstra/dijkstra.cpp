#include "dijkstra.h"

#include <algorithm>
#include <queue>

dijkstra::dijkstra(const std::shared_ptr<IDirectedgraph>& G, int s) : G_(G), s_(s), distTo_(G->V(), INF), edgeTo_(G->V(), nullptr){
    validateVertex(s);
    distTo_[s] = 0.0;

    using Pair = std::pair<double, int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> pq;
    pq.push({0.0, s});

    while (!pq.empty()) {
        auto [dist, u] = pq.top();
        pq.pop();

        if (dist > distTo_[u]) {
            continue;
        }

        for (const auto& e : G_->adj(u)) {
            int v = e->to();
            double newDist = distTo_[u] + e->weight();
            if (newDist < distTo_[v]) {
                distTo_[v] = newDist;
                edgeTo_[v] = e;
                pq.emplace(newDist, v); //used emplace instead of push for pair in-place construction!
            }
        }
    }
}

double dijkstra::distTo(int v) const {
    validateVertex(v);
    return distTo_[v];
}

bool dijkstra::hasPathTo(int v) const {
    validateVertex(v);
    return distTo_[v] < dijkstra::INF;
}

std::vector<std::shared_ptr<IEdge>> dijkstra::pathTo(int v) const {
    validateVertex(v);
    if (!hasPathTo(v)) return {};

    std::vector<std::shared_ptr<IEdge>> path;
    for (std::shared_ptr<IEdge> e = edgeTo_[v]; e != nullptr; e = edgeTo_[e->from()]) {
        path.push_back(e);
    }
    std::reverse(path.begin(), path.end());
    return path;
}


void dijkstra::validateVertex(int v) const {
    if (v < 0 || v >= G_->V()) {
        throw std::out_of_range(
            "vertex " + std::to_string(v) +
            " is should be between 0 and " + std::to_string(G_->V() - 1)
        );
    }
}