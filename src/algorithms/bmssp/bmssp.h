#ifndef BMSSP_H
#define BMSSP_H
#include <limits>
#include "graphs/Directedgraph.h"


class bmssp {
private:
    IDirectedgraph& g;
    std::vector<double> dist;
    std::vector<int> pred;
    std::vector<bool> complete;
    int k, t;
    const double INF = std::numeric_limits<double>::infinity();
    std::pair<std::vector<int>, std::vector<int>> findPivots(double B, const std::vector<int>& S);
    std::pair<double, std::vector<int>> baseCase(double B, const std::vector<int>& S);
    std::pair<double, std::vector<int>> BMSSP(int level, double B, const std::vector<int>& S);

public:
    bmssp(IDirectedgraph& graph);
    void solve(int source);
    std::vector<double> getDistances() const;
    std::vector<int> getPredecessors() const;
    double getDistance(int vertex) const;
};



#endif //BMSSP_H
