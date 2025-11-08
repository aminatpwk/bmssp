#include <limits>
#include <map>
#include <vector>
#include <utility>
#ifndef ADAPTIVEDATASTRUCTURE_H
#define ADAPTIVEDATASTRUCTURE_H

const double INF = std::numeric_limits<double>::infinity();

struct Block {
    std::pmr::map<int, double> elements;
    double upperBound;
    Block* next;
    Block(double ub = INF) : upperBound(ub), next(nullptr) {}
};

class AdaptiveDataStructure {
public:
    AdaptiveDataStructure(int m, double b, int nv);
    ~AdaptiveDataStructure();
    void insert(int key, double value);
    void batchPrepend(const std::vector<std::pair<int, double>>& L);
    std::pair<std::vector<int>, double> pull();
    bool empty() const;

private:
    Block* D0_head;
    Block* D1_head;
    int M;
    double B;
    std::pmr::vector<double> bestVal;
    void splitBlock(Block* block, Block* prev);
    void removeKey(int key);

};
#endif //ADAPTIVEDATASTRUCTURE_H
