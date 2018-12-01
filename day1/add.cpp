#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>

int main() {
    std::ifstream is("input");
    std::cout << std::accumulate(std::istream_iterator<int>{is},
                                 std::istream_iterator<int>{}, 0)
              << std::endl;
}
