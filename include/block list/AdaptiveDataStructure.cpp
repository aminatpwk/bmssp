#include <algorithm>
#include <limits>
#include <map>
#include <vector>

const double INF = std::numeric_limits<double>::infinity();

struct Block {
    std::pmr::map<int, double> elements;
    double upperBound;
    Block* next;
    Block(double ub = INF) : upperBound(ub), next(nullptr) {}
};

class AdaptiveDataStructure {
private:
    Block* D0_head; //for batch-prepend
    Block* D1_head; //for inserted
    int M;
    double B;

public:
    AdaptiveDataStructure::AdaptiveDataStructure(int m, double b) : M(m), B(b) {
        D0_head = nullptr;
        D1_head = new Block(b);
    }

    AdaptiveDataStructure::~AdaptiveDataStructure() {
        Block* curr = D0_head;
        while (curr) {
            Block* next = curr->next;
            delete curr;
            curr = next;
        }
        curr = D1_head;
        while (curr) {
            Block* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    void AdaptiveDataStructure::insert(int key, double value) {
        if (value >= B) {
            return;
        }

        Block* current = D1_head;
        Block* previous = nullptr;
        while (current && current->upperBound < value) {
            previous = current;
            current = current->next;
        }

        if (current) {
            auto it = current->elements.find(key);
            if (it != current->elements.end()) {
                if (value < it->second) {
                    current->elements[key] = value;
                }
            }else {
                current->elements[key] = value;
                if (current->elements.size() > M) {
                    splitBlock(current, previous);
                }
            }
        }
    }

    void AdaptiveDataStructure::batchprepend(const std::vector<std::pair<int, double>>& L) {
        if (L.empty()) {
            return;
        }

        Block* newHead = nullptr;
        Block* tail = nullptr;
        std::vector<std::pair<int, double>> sorted = L;
        sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {return a.second < b.second;});

        for (size_t i = 0; i < sorted.size(); i++) {
            Block* b = new Block();
            size_t count = 0;
            while (i < sorted.size() && count < M/2) {
                b->elements[sorted[i].first] = sorted[i].second;
                i++;
                count++;
            }
            if (!b->elements.empty()) {
                b->upperBound = b->elements.rbegin()->second;
            }

            if (!newHead) {
                newHead = tail = b;
            }else {
                tail->next = b;
                tail = b;
            }
        }

        if (tail) {
            tail->next = D0_head;
            D0_head = newHead;
        }
    }

    std::pair<std::vector<int>, double> AdaptiveDataStructure::pull() {
        std::vector<std::pair<int, double>> collected;

        //ben collect nga D0
        Block* current = D0_head;
        while (current && collected.size() < M) {
            for (auto& p : current->elements) {
                collected.push_back(p);
                if (collected.size() >= M) {
                    break;
                }
            }
            current = current->next;
        }

        //collect nga D1
        current = D1_head;
        while (current && collected.size() < M) {
            for (auto& p : current->elements) {
                collected.push_back(p);
                if (collected.size() >= M) {
                    break;
                }
            }
            current = current->next;
        }

        sort(collected.begin(), collected.end(), [](const auto& a, const auto& b) {return a.second < b.second;});
        if (collected.size() > M) {
            collected.resize(M);
        }
        std::vector<int> result;
        for (auto& p : collected) {
            result.push_back(p.first);
        }

        for (int key : result) {
            removeKey(key);
        }

        double nextMin = B;
        current = D0_head;
        while (current) {
            for (auto& p : current->elements) {
                nextMin = std::min(nextMin, p.second);
            }
            current = current->next;
        }
        current = D1_head;
        while (current) {
            for (auto& p : current -> elements) {
                nextMin = std::min(nextMin, p.second);
            }
            current = current->next;
        }

        return {result, nextMin};
    }

    bool AdaptiveDataStructure::empty() const {
        if (D0_head) {
            return false;
        }

        Block* current = D1_head;
        while (current) {
            if (!current->elements.empty()) {
                return false;
            }
            current = current->next;
        }
        return true;
    }

private:
    void AdaptiveDataStructure::splitBlock(Block* block, Block* prev) {
        if ((int)block->elements.size() <= M) return;

        std::vector<std::pair<int, double>> elems(
            block->elements.begin(),
            block->elements.end()
        );
        std::sort(elems.begin(), elems.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; }
        );

        size_t mid = elems.size() / 2;

        Block* newBlock = new Block();
        block->elements.clear();

        for (size_t i = 0; i < mid; i++) {
            block->elements[elems[i].first] = elems[i].second;
        }
        for (size_t i = mid; i < elems.size(); i++) {
            newBlock->elements[elems[i].first] = elems[i].second;
        }

        if (!block->elements.empty()) {
            block->upperBound = block->elements.rbegin()->second;
        }
        if (!newBlock->elements.empty()) {
            newBlock->upperBound = newBlock->elements.rbegin()->second;
        }

        newBlock->next = block->next;
        block->next = newBlock;
    }

    void AdaptiveDataStructure::removeKey(int key) {
        Block* curr = D0_head;
        while (curr) {
            curr->elements.erase(key);
            curr = curr->next;
        }
        curr = D1_head;
        while (curr) {
            curr->elements.erase(key);
            curr = curr->next;
        }
    }
};
