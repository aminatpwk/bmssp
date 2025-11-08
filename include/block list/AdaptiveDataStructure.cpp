#include "AdaptiveDataStructure.h"
#include <algorithm>
#include <map>
#include <vector>

AdaptiveDataStructure::~AdaptiveDataStructure() {
    Block* current = D0_head;
    while (current) {
        Block* next = current->next;
        delete current;
        current = next;
    }
    current = D1_head;
    while (current) {
        Block* next = current->next;
        delete current;
        current = next;
    }
}

    AdaptiveDataStructure::AdaptiveDataStructure(int m, double b, int nv) : M(m), B(b), D0_head(nullptr) {
        D1_head = new Block(b);
        bestVal.assign(nv, INF);
    }


    void AdaptiveDataStructure::insert(int key, double value) {
        if (value >= B) {
            return;
        }
        if (value >= bestVal[key]) return;
        bestVal[key] = value;

        Block* current = D1_head;
        Block* previous = nullptr;
        while (current && current->upperBound < value) {
            previous = current;
            current = current->next;
        }

        if (!current) {
            current = new Block(INF);
            if (previous) {
                previous->next = current;
            }else {
                D1_head = current;
            }
        }

        auto it = current->elements.find(key);
        if (it != current->elements.end()) {
            if (value < it->second) {
                it->second = value;
            }
        }else {
            current->elements[key] = value;
            if (static_cast<int>(current->elements.size()) > M) {
                splitBlock(current, previous);
            }
        }

        current->upperBound = current->elements.empty() ? INF : current->elements.rbegin()->second;
    }

    void AdaptiveDataStructure::batchPrepend(const std::vector<std::pair<int, double>>& L) {
        if (L.empty()) {
            return;
        }

        std::pmr::vector<std::pair<int,double>> filtered;
        filtered.reserve(L.size());
        for (auto&& p : L) {
            if (p.second < bestVal[p.first]) {
                bestVal[p.first] = p.second;
                filtered.emplace_back(p);
            }
        }
        if (filtered.empty()) return;
        std::sort(filtered.begin(), filtered.end(),
                  [](auto const& a, auto const& b){ return a.second < b.second; });

        Block* newHead = nullptr;
        Block* tail = nullptr;

        for (size_t i = 0; i < filtered.size(); ) {
            Block* b = new Block();
            size_t count = 0;
            while (i < filtered.size() && count < static_cast<size_t>(M)) {
                b->elements[filtered[i].first] = filtered[i].second;
                ++i;
                ++count;
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
        std::vector<int> S_i;
        double B_i = B;
        std::pmr::vector<std::pair<int,double>> buf;
        buf.reserve(M+8);
        size_t collected = 0;

        for (Block* b = D0_head; b && collected < static_cast<size_t>(M); b = b->next) {
            for (auto&& p : b->elements) {
                buf.emplace_back(p);
                ++collected;
                if (collected == static_cast<size_t>(M)) break;
            }
        }
        for (Block* b = D1_head; b && collected < static_cast<size_t>(M); b = b->next) {
            for (auto&& p : b->elements) {
                buf.emplace_back(p);
                ++collected;
                if (collected == static_cast<size_t>(M)) break;
            }
        }

        size_t total = 0;
        for (Block* b = D0_head; b; b = b->next) total += b->elements.size();
        for (Block* b = D1_head; b; b = b->next) total += b->elements.size();

        if (total <= static_cast<size_t>(M)) {
            for (auto&& p : buf) S_i.push_back(p.first);
            while (D0_head) { Block* nxt = D0_head->next; delete D0_head; D0_head = nxt; }
            while (D1_head && D1_head->next) { Block* nxt = D1_head->next; delete D1_head; D1_head = nxt; }
            if (D1_head) { D1_head->elements.clear(); D1_head->upperBound = B; }
            B_i = B;
            return {S_i, B_i};
        }


        std::sort(buf.begin(), buf.end(),
              [](auto const& a, auto const& b){ return a.second < b.second; });

        size_t take = std::min(buf.size(), static_cast<size_t>(M));
        for (size_t i = 0; i < take; ++i) S_i.push_back(buf[i].first);

        for (int v : S_i) removeKey(v);
        double minRem = INF;
        for (Block* b = D0_head; b; b = b->next)
            if (!b->elements.empty())
                minRem = std::min(minRem, b->elements.begin()->second);
        for (Block* b = D1_head; b; b = b->next)
            if (!b->elements.empty())
                minRem = std::min(minRem, b->elements.begin()->second);
        B_i = (minRem < INF) ? minRem : B;

        return {S_i, B_i};
    }

    bool AdaptiveDataStructure::empty() const {
        if (D0_head) return false;
        for (Block* b = D1_head; b; b = b->next)
            if (!b->elements.empty()) return false;
        return true;
    }

    void AdaptiveDataStructure::splitBlock(Block* block, Block* prev) {
        if (static_cast<int>(block->elements.size()) <= M) return;

        std::pmr::vector<std::pair<int,double>> elems(block->elements.begin(),
                                                      block->elements.end());
        std::sort(elems.begin(), elems.end(),
                  [](auto const& a, auto const& b){ return a.second < b.second; });

        size_t mid = elems.size() / 2;

        Block* newB = new Block();
        block->elements.clear();

        for (size_t i = 0; i < mid; ++i)
            block->elements[elems[i].first] = elems[i].second;
        for (size_t i = mid; i < elems.size(); ++i)
            newB->elements[elems[i].first] = elems[i].second;

        block->upperBound = block->elements.empty() ? INF : block->elements.rbegin()->second;
        newB->upperBound  = newB->elements.empty()  ? INF : newB->elements.rbegin()->second;

        newB->next = block->next;
        block->next = newB;
    }

    void AdaptiveDataStructure::removeKey(int key) {
        for (Block* b = D0_head; b; b = b->next) b->elements.erase(key);
        for (Block* b = D1_head; b; b = b->next) b->elements.erase(key);
    }

