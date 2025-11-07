#include "bmssp.h"
#include <map>
#include <queue>
#include <set>
#include "math.h"

bmssp::bmssp(IDirectedgraph &graph): g(graph) {
    dist.resize(g.V(), INF);
    pred.resize(g.V(), -1);
    k = std::max(1, (int)std::floor(std::pow(g.V(), 1.0/3.0)));
    t = std::max(1, (int)std::floor(std::pow(g.V(), 2.0/3.0)));
}

std::pair<std::vector<int>, std::vector<int> > bmssp::findPivots(double B, const std::vector<int> &S) {
    std::vector<int> W = S;
    std::set<int> W_set(S.begin(), S.end());
    std::vector<int> W_prev = S;

    for (int iter = 0; iter < k; iter++) {
        std::vector<int> W_next;
        std::set<int> W_next_set;

        for (int u : W_prev) {
            auto edges = g.adj(u);
            for (const auto& edge : edges) {
                int v = edge->to();
                double new_dist = dist[u] + edge->weight();

                if (new_dist <= dist[v]) {
                    dist[v] = new_dist;
                    pred[v] = u;

                    if (new_dist < B && W_next_set.find(v) == W_next_set.end()) {
                        W_next.push_back(v);
                        W_next_set.insert(v);
                        if (W_set.find(v) == W_set.end()) {
                            W.push_back(v);
                            W_set.insert(v);
                        }
                    }
                }
            }
        }

        W_prev = W_next;

        if ((int)W.size() > k * (int)S.size()) {
            return {S, W};
        }
    }

    std::vector<int> P;
    std::map<int, int> treeSize;

    for (int v : S) {
        treeSize[v] = 1;
    }

    for (int v : W) {
        int curr = v;
        while (pred[curr] != -1 && W_set.count(pred[curr])) {
            curr = pred[curr];
        }
        if (treeSize.find(curr) != treeSize.end() && curr != v) {
            treeSize[curr]++;
        }
    }

    for (const auto& p : treeSize) {
        if (p.second >= k) {
            P.push_back(p.first);
        }
    }

    return {P, W};
}

std::pair<double, std::vector<int>> bmssp::baseCase(double B, const std::vector<int> &S) {
    if (S.size() != 1) {
        throw std::runtime_error("BaseCase requires singleton set");
    }

    int x = S[0];
    std::vector<int> U0 = {x};

    std::priority_queue<
        std::pair<double, int>,
        std::vector<std::pair<double, int>>,
        std::greater<>
    > pq;

    pq.push({dist[x], x});
    std::set<int> visited;

    while (!pq.empty() && (int)U0.size() < k + 1) {
        auto [d, u] = pq.top();
        pq.pop();

        if (visited.count(u)) continue;
        visited.insert(u);

        if (u != x) U0.push_back(u);

        auto edges = g.adj(u);
        for (const auto& edge : edges) {
            int v = edge->to();
            double new_dist = dist[u] + edge->weight();

            if (new_dist <= dist[v] && new_dist < B) {
                dist[v] = new_dist;
                pred[v] = u;
                if (!visited.count(v)) {
                    pq.push({new_dist, v});
                }
            }
        }
    }

    if ((int)U0.size() <= k) {
        return {B, U0};
    } else {
        double maxDist = 0;
        for (int v : U0) {
            maxDist = std::max(maxDist, dist[v]);
        }
        std::vector<int> U;
        for (int v : U0) {
            if (dist[v] < maxDist) {
                U.push_back(v);
            }
        }
        return {maxDist, U};
    }
}

std::pair<double, std::vector<int> > bmssp::BMSSP(int level, double B, const std::vector<int> &S) {
    if (level == 0) {
        return baseCase(B, S);
    }

    auto [P, W] = findPivots(B, S);

    int M = (int)std::pow(2, (level - 1) * t);
    AdaptiveDataStructure D(M, B);

    for (int x : P) {
        D.insert(x, dist[x]);
    }

    std::vector<int> U;
    double B_prev = INF;
    for (int x : P) {
        B_prev = std::min(B_prev, dist[x]);
    }

    int targetSize = k * (int)std::pow(2, level * t);

    while ((int)U.size() < targetSize && !D.empty()) {
        auto [S_i, B_i] = D.pull();

        auto [B_i_prime, U_i] = BMSSP(level - 1, B_i, S_i);

        U.insert(U.end(), U_i.begin(), U_i.end());

        std::vector<std::pair<int, double>> K;

        for (int u : U_i) {
            auto edges = g.adj(u);
            for (const auto& edge : edges) {
                int v = edge->to();
                double new_dist = dist[u] + edge->weight();

                if (new_dist <= dist[v]) {
                    dist[v] = new_dist;
                    pred[v] = u;

                    if (new_dist >= B_i && new_dist < B) {
                        D.insert(v, new_dist);
                    } else if (new_dist >= B_i_prime && new_dist < B_i) {
                        K.push_back({v, new_dist});
                    }
                }
            }
        }
        for (int x : S_i) {
            if (dist[x] >= B_i_prime && dist[x] < B_i) {
                K.push_back({x, dist[x]});
            }
        }
        D.batchPrepend(K);
        B_prev = B_i_prime;
    }

    double B_prime = B_prev;
    for (int x : W) {
        if (dist[x] < B_prime) {
            U.push_back(x);
        }
    }

    return {B_prime, U};
}

void bmssp::solve(int source) {
    dist[source] = 0;
    int maxLevel = std::max(1, (int)std::ceil(std::log(g.V()) / t));
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

