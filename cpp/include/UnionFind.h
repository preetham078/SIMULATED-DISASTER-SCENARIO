#pragma once

#include <cstddef>
#include <vector>

using namespace std;

class UnionFind {
public:
    explicit UnionFind(size_t size);
    size_t find(size_t item);
    bool unite(size_t left, size_t right);
    size_t components() const;

private:
    vector<size_t> parent_;
    vector<unsigned int> rank_;
    size_t components_;
};
