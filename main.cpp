#include <iostream>
#include "graphs/Directedgraph.h"

using namespace std;

int main() {
    cout << "=== Manual Graph Test ===\n\n";

    IDirectedgraph G(5, true);

    cout << "Empty graph created:\n";
    cout << "  Vertices: " << G.V() << "\n";
    cout << "  Edges: " << G.E() << "\n\n";

    cout << "Adding edges...\n";
    G.addEdge(make_shared<IEdge>(0, 1, 4.0));
    G.addEdge(make_shared<IEdge>(0, 2, 2.0));
    G.addEdge(make_shared<IEdge>(1, 2, 1.0));
    G.addEdge(make_shared<IEdge>(1, 3, 5.0));
    G.addEdge(make_shared<IEdge>(2, 3, 8.0));

    // Print the graph
    cout << "\n" << G.toString() << "\n";

    // Check degrees manually
    cout << "Degree information:\n";
    for (int v = 0; v < G.V(); v++) {
        cout << "  Vertex " << v << ": "
             << "out=" << G.outdegree(v) << ", "
             << "in=" << G.indegree(v) << "\n";
    }

    return 0;
}