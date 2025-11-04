#include "graphs/Directedgraph.h"

#include <fstream>
#include <sstream>

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

static std::shared_ptr<IDirectedgraph> loadFromFile(const std::string& filename, bool directed = true) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        std::string line;

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == 'c' || line[0] == '#') {
                continue;
            }
            break;
        }

        std::istringstream iss(line);
        std::string token;
        iss >> token;

        int V, E;

        if (token == "p") {
            std::string sp;
            iss >> sp >> V >> E;
        } else {
            V = std::stoi(token);
            iss >> E;
        }

        auto graph = std::make_shared<IDirectedgraph>(V, directed);

        int edgesRead = 0;
        while (std::getline(file, line) && edgesRead < E) {
            if (line.empty() || line[0] == 'c' || line[0] == '#') {
                continue;
            }

            std::istringstream edgeStream(line);
            std::string edgeType;
            edgeStream >> edgeType;

            int v, w;
            double weight;

            if (edgeType == "a") {
                edgeStream >> v >> w >> weight;
                v--; w--;
            } else {
                v = std::stoi(edgeType);
                edgeStream >> w >> weight;
            }

            graph->addEdge(std::make_shared<IEdge>(v, w, weight));
            edgesRead++;
        }

        file.close();
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


