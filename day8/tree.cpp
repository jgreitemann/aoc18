#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

struct node {
    std::vector<node> children;
    std::vector<int> metadata;

    int sum() const {
        return std::accumulate(metadata.begin(), metadata.end(), 0)
            + std::accumulate(children.begin(), children.end(), 0,
                              [] (int acc, node const& n) {
                                  return acc + n.sum();
                              });
    }

    int value() const {
        if (children.empty())
            return std::accumulate(metadata.begin(), metadata.end(), 0);
        else
            return std::accumulate(metadata.begin(), metadata.end(), 0,
                                   [&] (int acc, int m) {
                                       if (m > 0 && m <= children.size())
                                           acc += children[m - 1].value();
                                       return acc;
                                   });
    }
};

std::istream & operator>>(std::istream & is, node & n) {
    size_t n_children, n_metadata;
    is >> n_children >> n_metadata;
    n.children.resize(n_children);
    for (auto & c : n.children)
        is >> c;
    n.metadata.resize(n_metadata);
    for (int & i : n.metadata)
        is >> i;
    return is;
}

int main() {
    std::ifstream is{"input"};
    node root;
    is >> root;
    std::cout << root.sum() << std::endl;
    std::cout << root.value() << std::endl;
}
