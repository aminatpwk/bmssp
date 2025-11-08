#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <chrono>
#include "graphs/Directedgraph.h"
#include "src/algorithms/bmssp/bmssp.h"

using namespace std;

void print_shortest_path(int source_vertex, int destination_vertex,
                         const vector<int>& pred, const vector<double>& dist) {
    const double INF = std::numeric_limits<double>::infinity();

    if (dist[destination_vertex] == INF) {
        cout << "Unreachable (Distance = INF)";
        return;
    }

    cout << "Distance = " << dist[destination_vertex] << ", Path: ";

    vector<int> path;
    for (int v = destination_vertex; v != -1; v = pred[v]) {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());

    for (size_t i = 0; i < path.size(); ++i) {
        cout << path[i];
        if (i + 1 < path.size()) cout << " -> ";
    }
}

int main() {
    const std::string filename = "../sparse.txt";
    const int source_vertex = 0;

    try {
        cout << "Loading graph from file: " << filename << "..." << endl;
        auto G = IDirectedgraph::loadFromFile(filename, true);

        cout << "Graph loaded successfully. " << G->V() << " vertices, " << G->E() << " edges." << endl;

        if (source_vertex < 0 || source_vertex >= G->V()) {
            cerr << "Error: Source vertex out of bounds." << endl;
            return 1;
        }

        cout << "\nRunning BMSSP algorithm starting from vertex " << source_vertex << "..." << endl;
        auto start_bmssp = chrono::high_resolution_clock::now();
        bmssp bmssp(*G);
        bmssp.solve(source_vertex);
        auto end_bmssp = chrono::high_resolution_clock::now();
        auto bmssp_duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end_bmssp - start_bmssp);

        cout << "BMSSP completed in " << bmssp_duration.count() << " ms" << endl;
        auto dist = bmssp.getDistances();
        auto pred = bmssp.getPredecessors();

        int reachable = 0;
        int at_zero = 0;
        int at_inf = 0;
        const double INF = std::numeric_limits<double>::infinity();

        for (int v = 0; v < G->V(); v++) {
            if (dist[v] < INF) reachable++;
            if (dist[v] == 0) at_zero++;
            if (dist[v] == INF) at_inf++;
        }

        cout << "\n--- Shortest Paths from Source Vertex " << source_vertex << " ---" << endl;
        for (int v = 0; v < G->V(); v++) {
            cout << "Vertex " << v << ": ";
            print_shortest_path(source_vertex, v, pred, dist);
            cout << endl;
        }

    } catch (const std::exception& e) {
        cerr << "\nAn error occurred: " << e.what() << endl;
        return 1;
    }

    return 0;
}