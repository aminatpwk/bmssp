#include "bmssp.h"
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include "math.h"
#include "block list/AdaptiveDataStructure.h"

bmssp::bmssp(IDirectedgraph &graph): g(graph) {
    dist.resize(g.V(), INF);
    pred.resize(g.V(), -1);
    double logn = log2(static_cast<double>(g.V()));
    k = std::max(1, static_cast<int>(std::floor(std::pow(logn, 1.0 / 3.0))));
    t = std::max(1, static_cast<int>(std::floor(std::pow(logn, 2.0 / 3.0))));
}

std::pair<std::vector<int>, std::vector<int> > bmssp::findPivots(double B, const std::vector<int> &S) {
    std::vector<int> W = S;
    std::set<int> W_set(S.begin(), S.end());
    std::vector<int> current_frontier = S;

    for (int iter = 0; iter < k; iter++) {
        std::vector<int> next_frontier;
        std::set<int> next_set;

        for (int u : current_frontier) {
            auto edges = g.adj(u);
            for (const auto& edge : edges) {
                int v = edge->to();
                double new_dist = dist[u] + edge->weight();

                if (new_dist <= dist[v]) {
                    dist[v] = new_dist;
                    pred[v] = u;

                    if (new_dist < B) {
                        if (W_set.find(v) == W_set.end()) {
                            W.push_back(v);
                            W_set.insert(v);
                        }
                        if (next_set.find(v) == next_set.end()) {
                            next_frontier.push_back(v);
                            next_set.insert(v);
                        }
                    }
                }
            }
        }
        current_frontier = std::move(next_frontier);
        if (current_frontier.empty()) break;
    }

    if (static_cast<int>(W.size()) > k * static_cast<int>(S.size())) {
        return {S, W};
    }

    std::map<int, int> treeSize;
    for (int v : S) {
        treeSize[v] = 1;
    }

    for (int v : W) {
        int curr = v;
        while (pred[curr] != -1 && W_set.count(pred[curr])) {
            curr = pred[curr];
        }
        if (treeSize.count(curr)) {
            treeSize[curr]++;
        }
    }

    std::vector<int> P;
    for (const auto& p : treeSize) {
        if (p.second >= k) {
            P.push_back(p.first);
        }
    }

    return {P, W};
}

std::pair<double, std::vector<int>> bmssp::baseCase(double B, const std::vector<int> &S) {
    if (S.empty()) {
        return {B, {}};
    }
    if (S.size() != 1) {
        throw std::runtime_error("BaseCase requires singleton set");
    }

    int x = S[0];
    std::vector<int> U0;

    std::priority_queue<
        std::pair<double, int>,
        std::vector<std::pair<double, int>>,
        std::greater<>
    > pq;

    pq.push({dist[x], x});
    std::set<int> visited;

    while (!pq.empty() && static_cast<int>(U0.size()) < k + 1) {
        auto [d, u] = pq.top();
        pq.pop();

        if (visited.count(u)) continue;
        visited.insert(u);

        if (d < B) {
            U0.push_back(u);
        }

        auto edges = g.adj(u);
        for (const auto& edge : edges) {
            int v = edge->to();
            double new_dist = dist[u] + edge->weight();

            if (new_dist <= dist[v] && new_dist < B) {
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                }
                pred[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    double maxDist = U0.empty() ? B : dist[U0[0]];
    for (int v : U0) {
        maxDist = std::max(maxDist, dist[v]);
    }
    if (static_cast<int>(U0.size()) <= k) {
        return {B, U0};
    } else {
        std::vector<int> U_filtered;
        for (int v : U0) {
            if (dist[v] < maxDist) U_filtered.push_back(v);
        }
        return {maxDist, U_filtered};
    }
}

std::pair<double, std::vector<int> > bmssp::BMSSP(int level, double B, const std::vector<int> &S) {
    if (S.empty()) {
        return {B, {}};
    }

    if (level == 0) {
        return baseCase(B, S);
    }

    auto [P, W] = findPivots(B, S);

    if (P.empty()) {
        std::vector<int> U;
        for (int x : W) {
            if (dist[x] < B) {
                U.push_back(x);
            }
        }
        return {B, U};
    }

    int M = static_cast<int>(std::pow(2, (level - 1) * t));
    AdaptiveDataStructure D(M, B, g.V());

    for (int x : P) {
        D.insert(x, dist[x]);
    }

    std::vector<int> U;
    std::vector<bool> markU(g.V(), false);
    double B_prev = INF;
    for (int x : P) {
        B_prev = std::min(B_prev, dist[x]);
    }

    int targetSize = k * static_cast<int>(std::pow(2, level * t));

    while (static_cast<int>(U.size()) < targetSize && !D.empty()) {
        auto [S_i, B_i] = D.pull();

        auto [B_i_prime, U_i] = BMSSP(level - 1, B_i, S_i);

        U.insert(U.end(), U_i.begin(), U_i.end());

        std::vector<std::pair<int, double>> K;

        size_t old_size = U.size();
        for (int u : U_i) {
            if (!markU[u]) {
                markU[u] = true;
                U.push_back(u);
            }
        }

        for (size_t proc = old_size; proc < U.size(); ++proc) {
            int u = U[proc];
            auto edges = g.adj(u);
            for (const auto& edge : edges) {
                int v = edge->to();
                double new_dist = dist[u] + edge->weight();

                if (new_dist <= dist[v]) {
                    if (new_dist < dist[v]) {
                        dist[v] = new_dist;
                    }
                    pred[v] = u;

                    if (new_dist >= B_i && new_dist < B) {
                        D.insert(v, new_dist);
                    } else if (new_dist >= B_i_prime && new_dist < B_i) {
                        K.emplace_back(v, new_dist);
                    }

                    if (!markU[v]) {
                        markU[v] = true;
                        U.push_back(v);
                    }
                }
            }
        }
        for (int x : S_i) {
            if (dist[x] >= B_i_prime && dist[x] < B_i) {
                K.emplace_back(x, dist[x]);
            }
        }
        D.batchPrepend(K);
        B_prev = B_i_prime;
    }

    double B_prime = D.empty() ? B : B_prev;
    for (int x : W) {
        if (dist[x] < B_prime && !markU[x]) {
            markU[x] = true;
            U.push_back(x);
        }
    }

    return {B_prime, U};
}

void bmssp::solve(int source) {
    std::fill(dist.begin(), dist.end(), INF);
    std::fill(pred.begin(), pred.end(), -1);
    dist[source] = 0;
    int maxLevel = std::max(1, static_cast<int>(std::ceil(log2(static_cast<double>(g.V())) / t)));
    BMSSP(maxLevel, INF, {source});
}

std::vector<double> bmssp::getDistances() const{
    return dist;
}

std::vector<int> bmssp::getPredecessors() const {
    return pred;
}

double bmssp::getDistance(int vertex) const {
    if (vertex < 0 || vertex >= g.V()) {
        throw std::out_of_range("Vertex out of range");
    }

    return dist[vertex];
}

