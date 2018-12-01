#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>

template <typename T>
void rotate(std::deque<T> & d) {
    d.push_back(d.front());
    d.pop_front();
}

int main() {
    std::ifstream is("input");
    std::deque<int> d;
    std::copy(std::istream_iterator<int>{is},
              std::istream_iterator<int>{},
              std::back_inserter(d));
    int total = 0;
    std::set<int> s;
    while (s.find(total) == s.end()) {
        s.insert(total);
        total += d.front();
        rotate(d);
    }
    std::cout << total << std::endl;
}
