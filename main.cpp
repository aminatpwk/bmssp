#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>

// Assuming these headers exist based on your provided structure
#include "graphs/Directedgraph.h"
#include "src/algorithms/dijkstra/dijkstra.h"

using namespace std;

// Helper function to print the shortest path and distance
void print_shortest_path(int source_vertex, int destination_vertex, const dijkstra& solver) {
    // Check if the vertex is reachable (distTo is not INF)
    if (!solver.hasPathTo(destination_vertex)) {
        cout << "Unreachable (Distance = INF)";
        return;
    }

    double distance = solver.distTo(destination_vertex); // Get shortest distance
    cout << "Distance = " << distance << ", Path: ";

    if (source_vertex == destination_vertex) {
        cout << source_vertex;
        return;
    }

    // Get the path as a vector of edges
    vector<shared_ptr<IEdge>> path = solver.pathTo(destination_vertex);

    // Print the vertices in the path
    if (!path.empty()) {
        cout << path[0]->from(); // Start at the source vertex
        for (const auto& e : path) {
            cout << " -> " << e->to(); // Print the destination of each edge
        }
    }
}

int main() {
    // --- HARDCODED INPUTS ---
    const std::string filename = "../dense.txt";
    const int source_vertex = 0; // Testing starts from vertex 0
    // ------------------------

    try {
        // 1. Load the graph using the static loadFromFile method
        cout << "Loading graph from file: " << filename << "..." << endl;
        auto G = IDirectedgraph::loadFromFile(filename, true);

        cout << "Graph loaded successfully. " << G->V() << " vertices, " << G->E() << " edges." << endl;

        // 2. Validate and Run Dijkstra's algorithm
        if (source_vertex < 0 || source_vertex >= G->V()) {
            cerr << "Error: Hardcoded source vertex " << source_vertex << " is out of bounds [0, " << G->V() - 1 << "]." << endl;
            return 1;
        }

        cout << "Running Dijkstra's algorithm starting from vertex " << source_vertex << "..." << endl;
        dijkstra shortest_path_solver(G, source_vertex);

        // 3. Print results for all vertices
        cout << "\n--- Shortest Paths from Source Vertex " << source_vertex << " ---" << endl;
        for (int v = 0; v < G->V(); v++) {
            cout << "Vertex " << v << ": ";
            print_shortest_path(source_vertex, v, shortest_path_solver);
            cout << endl;
        }

    } catch (const std::exception& e) {
        cerr << "\nAn error occurred: " << e.what() << endl;
        return 1;
    }

    return 0;
}