
#ifndef ADAPTIVEDATASTRUCTURE_H
#define ADAPTIVEDATASTRUCTURE_H

class AdaptiveDataStructure {
private:
    Block* D0_head;
    Block* D1_head;
    int M;
    double B;

    void splitBlock(Block* block, Block* prev);
    void removeKey(int key);

public:
    AdaptiveDataStructure(int m, double b);
    ~AdaptiveDataStructure();

    void insert(int key, double value);
    void batchPrepend(const std::vector<std::pair<int, double>>& L);
    std::pair<std::vector<int>, double> pull();
    bool empty() const;
};
#endif //ADAPTIVEDATASTRUCTURE_H
