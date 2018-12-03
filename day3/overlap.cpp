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
    int x, y, width, height;
};

int as_int(std::string const& str) {
    int i;
    std::stringstream ss;
    ss << str;
    ss >> i;
    return i;
}

int main() {
    boost::multi_array<std::int8_t, 2> fabric(boost::extents[1000][1000]);
    std::fill_n(fabric.data(), fabric.num_elements(), 0);

    std::regex claim_regex("#[0-9]+ @ ([0-9]+),([0-9]+): ([0-9]+)x([0-9]+)");

    std::ifstream is{"input"};
    is >> std::noskipws;
    std::stringstream ss;
    std::copy(std::istream_iterator<char>{is},
              std::istream_iterator<char>{},
              std::ostream_iterator<char>{ss});
    std::string s = ss.str();
    std::for_each(std::sregex_iterator(s.begin(),
                                       s.end(),
                                       claim_regex),
                  std::sregex_iterator(),
                  [&fabric] (std::smatch const& match) {
                      claim c {
                          as_int(match[1].str()),
                          as_int(match[2].str()),
                          as_int(match[3].str()),
                          as_int(match[4].str())
                      };
                      for (int i = 0; i < c.width; ++i)
                          for (int j = 0; j < c.height; ++j)
                              ++fabric[c.x + i][c.y + j];
                  });
    std::cout << std::count_if(fabric.data(),
                               fabric.data() + fabric.num_elements(),
                               [] (std::int8_t c) { return c > 1; })
              << std::endl;
}
