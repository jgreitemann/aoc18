#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>

#include <boost/multi_array.hpp>

struct claim {
    size_t id;
    int x, y, width, height;
};

std::ostream & operator<< (std::ostream & os, claim const & c) {
    return os << c.id;
}

template <typename T>
T as(std::string const& str) {
    T i;
    std::stringstream ss;
    ss << str;
    ss >> i;
    return i;
}

int main() {
    boost::multi_array<std::int8_t, 2> fabric(boost::extents[1000][1000]);
    std::fill_n(fabric.data(), fabric.num_elements(), 0);

    std::regex claim_regex("#([0-9]+) @ ([0-9]+),([0-9]+): ([0-9]+)x([0-9]+)");

    std::ifstream is{"input"};
    is >> std::noskipws;
    std::stringstream ss;
    std::copy(std::istream_iterator<char>{is},
              std::istream_iterator<char>{},
              std::ostream_iterator<char>{ss});
    std::string s = ss.str();
    std::vector<claim> claims;
    std::transform(std::sregex_iterator(s.begin(),
                                        s.end(),
                                        claim_regex),
                   std::sregex_iterator(),
                   std::back_inserter(claims),
                   [&fabric] (std::smatch const& match) {
                       claim c {
                           as<size_t>(match[1].str()),
                           as<int>(match[2].str()),
                           as<int>(match[3].str()),
                           as<int>(match[4].str()),
                           as<int>(match[5].str())
                       };
                       for (int i = 0; i < c.width; ++i)
                           for (int j = 0; j < c.height; ++j)
                               ++fabric[c.x + i][c.y + j];
                       return c;
                   });
    std::copy_if(claims.begin(),
                 claims.end(),
                 std::ostream_iterator<claim>{std::cout, "\n"},
                 [&fabric] (claim c) {
                     for (int i = 0; i < c.width; ++i)
                         for (int j = 0; j < c.height; ++j)
                             if (fabric[c.x + i][c.y + j] > 1)
                                 return false;
                     return true;
                 });
}
