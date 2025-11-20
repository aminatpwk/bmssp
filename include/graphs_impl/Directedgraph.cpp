#include "graphs/Directedgraph.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

IDirectedgraph::IDirectedgraph(int V, bool directed) : V_(V), E_(0), directed_(directed) {
    if (V < 0) {
        throw std::invalid_argument("Number of vertices must be greater than 0");
    }
    adj_.resize(V);
    indegree_.resize(V, 0);
}

int IDirectedgraph::V() const {
    return V_;
}

int IDirectedgraph::E() const {
    return E_;
}

bool IDirectedgraph::isDirected() const {
    return directed_;
}

void IDirectedgraph::addEdge(const std::shared_ptr<IEdge>& e) {
    int v = e->from();
    int w = e->to();
    validateVertex(v);
    validateVertex(w);
    adj_[v].push_back(e);
    indegree_[w]++;
    E_++;
}

std::vector<std::shared_ptr<IEdge>> IDirectedgraph::adj(int v) const {
        validateVertex(v);
        return adj_[v];
    }

int IDirectedgraph::outdegree(int v) const {
    validateVertex(v);
    return static_cast<int>(adj_[v].size());
}

int IDirectedgraph::indegree(int v) const {
    validateVertex(v);
    return indegree_[v];
}

std::vector<std::shared_ptr<IEdge>> IDirectedgraph::edges() const {
        std::vector<std::shared_ptr<IEdge>> list;
        list.reserve(E_);

        for (int v = 0; v < V_; v++) {
            for (const auto& e : adj_[v]) {
                list.push_back(e);
            }
        }

        return list;
    }

std::shared_ptr<IDirectedgraph> IDirectedgraph::loadFromFile(const std::string& filename, bool directed) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;

    // Skip comments and find header
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#')
            continue;
        if (line.find("source") != std::string::npos)
            break; // header line found
    }

    // Temporary storage for edges
    std::vector<std::tuple<long long, long long, double>> edges;
    edges.reserve(100000); // reserve some space

    // Read edges
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string srcStr, dstStr, weightStr;

        if (!std::getline(iss, srcStr, ',')) continue;
        if (!std::getline(iss, dstStr, ',')) continue;
        if (!std::getline(iss, weightStr, ',')) continue;

        long long src = std::stoll(srcStr);
        long long dst = std::stoll(dstStr);
        double weight = std::stod(weightStr);

        edges.emplace_back(src, dst, weight);
    }

    file.close();

    if (edges.empty()) {
        throw std::runtime_error("No valid edges found in file: " + filename);
    }

    // Remap OSM IDs to 0-based indices
    std::unordered_map<long long, int> nodeMap;
    int nextId = 0;

    // Count total vertices
    for (const auto& [src, dst, _] : edges) {
        if (nodeMap.find(src) == nodeMap.end()) nodeMap[src] = nextId++;
        if (nodeMap.find(dst) == nodeMap.end()) nodeMap[dst] = nextId++;
    }

    int V = nextId; // total vertices
    auto graph = std::make_shared<IDirectedgraph>(V, directed);

    // Add edges with remapped indices
    for (const auto& [src, dst, weight] : edges) {
        int u = nodeMap[src];
        int v = nodeMap[dst];
        graph->addEdge(std::make_shared<IEdge>(u, v, weight));
    }

    std::cout << "Graph loaded successfully: " << V << " vertices, " << edges.size() << " edges\n";
    return graph;
}

std::string IDirectedgraph::toString() const {
        std::ostringstream oss;
        oss << V_ << " vertices, " << E_ << " edges\n";

        for (int v = 0; v < V_; v++) {
            oss << v << ": ";
            for (const auto& e : adj_[v]) {
                oss << e->toString() << "  ";
            }
            oss << "\n";
        }

        return oss.str();
    }

void IDirectedgraph::validateVertex(int v) const {
    if (v < 0 || v >= V_) {
        throw std::out_of_range(
            "vertex " + std::to_string(v) +
            " is should be between 0 and " + std::to_string(V_ - 1)
        );
    }
}


