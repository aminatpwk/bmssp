import random
import argparse
import os
from typing import List, Tuple

def generate_graph(
    n_nodes: int,
    edge_density: float = 0.4,
    min_weight: float = 1.0,
    max_weight: float = 20.0,
    allow_negative: bool = False,
    ensure_acyclic: bool = False,
    ensure_connected: bool = True,
    seed: int | None = None
) -> List[Tuple[int, int, float]]:
    if seed is not None:
        random.seed(seed)

    edges: List[Tuple[int, int, float]] = []
    possible = [(i, j) for i in range(n_nodes) for j in range(n_nodes) if i != j]

    if ensure_acyclic:
        perm = list(range(n_nodes))
        random.shuffle(perm)
        rank = {node: idx for idx, node in enumerate(perm)}
        candidates = [(u, v) for u, v in possible if rank[u] < rank[v]]
    else:
        candidates = possible

    for u, v in candidates:
        if random.random() < edge_density:
            sign = -1 if allow_negative and random.random() < 0.3 else 1
            w = sign * random.uniform(min_weight, max_weight)
            edges.append((u, v, w))

    if ensure_connected and n_nodes > 1 and edges:
        from collections import defaultdict
        out_neighbors = defaultdict(set)
        in_neighbors = defaultdict(set)
        for u, v, _ in edges:
            out_neighbors[u].add(v)
            in_neighbors[v].add(u)

        visited = set()

        def dfs(node, neighbors_dict):
            stack = [node]
            while stack:
                cur = stack.pop()
                if cur not in visited:
                    visited.add(cur)
                    stack.extend(neighbors_dict[cur] - visited)

        dfs(0, out_neighbors)
        dfs(0, in_neighbors)
        reachable = visited

        if len(reachable) < n_nodes:
            missing = set(range(n_nodes)) - reachable
            for node in missing:
                src = random.choice(list(reachable))
                w = random.uniform(min_weight, max_weight)
                edges.append((src, node, w))
                reachable.add(node)
    return edges


def save_in_txt_format(
    edges: List[Tuple[int, int, float]],
    filename: str,
    n_nodes: int
):
    os.makedirs(os.path.dirname(filename), exist_ok=True) if os.path.dirname(filename) else None
    with open(filename, 'w') as f:
        f.write(f"{n_nodes}\n")
        f.write(f"{len(edges)}\n")
        for u, v, w in edges:
            f.write(f"{u} {v} {w:.2f}\n")


def main():
    parser = argparse.ArgumentParser(
        description="Generate random weighted digraph in format (N, M, u v w lines)"
    )
    parser.add_argument("-n", "--nodes", type=int, default=20, help="Number of nodes (default: 20)")
    parser.add_argument("-d", "--density", type=float, default=0.5,
                        help="Approximate edge density (0-1)")
    parser.add_argument("--min-w", type=float, default=1.0, help="Minimum absolute weight")
    parser.add_argument("--max-w", type=float, default=20.0, help="Maximum absolute weight")
    parser.add_argument("--negative", action="store_true", help="Allow negative weights")
    parser.add_argument("--dag", action="store_true", help="Force DAG (acyclic)")
    parser.add_argument("--no-connected", action="store_true", help="Do not enforce connectivity")
    parser.add_argument("-o", "--output", type=str, default="dense.txt", help="Output filename")
    parser.add_argument("--seed", type=int, default=None, help="Random seed for reproducibility")

    args = parser.parse_args()

    edges = generate_graph(
        n_nodes=args.nodes,
        edge_density=args.density,
        min_weight=args.min_w,
        max_weight=args.max_w,
        allow_negative=args.negative,
        ensure_acyclic=args.dag,
        ensure_connected=not args.no_connected,
        seed=args.seed
    )

    save_in_txt_format(edges, args.output, args.nodes)

if __name__ == "__main__":
    main()