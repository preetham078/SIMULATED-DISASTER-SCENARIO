#pragma once

#include <cstddef>
#include <vector>

class UnionFind {
public:
    explicit UnionFind(std::size_t size);
    std::size_t find(std::size_t item);
    bool unite(std::size_t left, std::size_t right);
    std::size_t components() const;

private:
    std::vector<std::size_t> parent_;
    std::vector<unsigned int> rank_;
    std::size_t components_;
};
