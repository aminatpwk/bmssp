# Empirical Benchmarking of BMSSP vs Dijkstra's Algorithm

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Paper](https://img.shields.io/badge/Paper-Abstract-brightgreen)](ABSTRACT.md)

An empirical evaluation of the Bounded Multi-Source Shortest Path (BMSSP) algorithm against classical Dijkstra's algorithm on real-world urban road networks.

## Related

This implementation accompanies this paper abstract:

> **"Practical Implications of BMSSP: Shortest Path Computation on Urban Road Networks"**  

**Abstract**: The single-source shortest path problem has a new theoretical benchmark set by the Bounded Multi-Source Shortest Path algorithm with an asymptotic complexity of O(mlog2/3n). This work presents an empirical benchmarking of BMSSP against the classical Dijkstra's algorithm across diverse graph configurations. Despite its theoretical advantage, BMSSP underperforms Dijkstra by 7-10× on most small to medium-sized graphs. Our findings underscore the critical difference between asymptotic optimality and practical, real-world algorithm performance.

## Key Findings

- **BMSSP is 7-10× slower** than Dijkstra's algorithm on real-world road networks
- **Theoretical optimality ≠ Practical performance** for current BMSSP implementation
- **Large constant factors** and **recursion overhead** dominate execution time
- **Poor cache locality** due to complex data structures and recursive nature

### Benchmark Results
| Graph | Dijkstra (ms) | BMSSP (ms) | Slowdown |
|-------|---------------|------------|----------|
| Shkodra | 2.33 | 25.276 | 10.8× |
| Tirana | 17.01 | 267.89 | 15.7× |
| Tokyo | 10.13 | 96.17 | 9.5× |

## Implementation Details

### Algorithms Implemented
- **Dijkstra's Algorithm**: Classical priority queue implementation
- **BMSSP**: Bounded Multi-Source Shortest Path based on [Duan et al., 2025]
- Both implemented from scratch in C++ following formal specifications

### Test Graphs
- **Real-world urban road networks**:
  - Shkodra, Albania (small)
  - Tirana, Albania (medium) 
  - Tokyo, Japan (dense)
- **Synthetic graphs**: For correctness validation

## Installation & Usage

### Prerequisites
- C++17 compiler
- CMake ≥ 3.15

### Build
```bash
git clone https://github.com/aminatpwk/bmssp.git
cd bmssp
mkdir build && cd build
cmake ..
make
```

### Note: 
This is a research implementation focused on empirical benchmarking rather than production use.
