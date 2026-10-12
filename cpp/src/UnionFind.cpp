#include "UnionFind.h"

#include <numeric>
#include <utility>

using namespace std;

UnionFind::UnionFind(size_t size) : parent_(size), rank_(size, 0), components_(size) {
    iota(parent_.begin(), parent_.end(), 0);
}

size_t UnionFind::find(size_t item) {
    if (parent_[item] != item) parent_[item] = find(parent_[item]);
    return parent_[item];
}

bool UnionFind::unite(size_t left, size_t right) {
    size_t rootLeft = find(left);
    size_t rootRight = find(right);
    if (rootLeft == rootRight) return false;
    if (rank_[rootLeft] < rank_[rootRight]) swap(rootLeft, rootRight);
    parent_[rootRight] = rootLeft;
    if (rank_[rootLeft] == rank_[rootRight]) ++rank_[rootLeft];
    --components_;
    return true;
}

size_t UnionFind::components() const { return components_; }
