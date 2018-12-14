#include <algorithm>
#include <iostream>
#include <iterator>
#include <deque>

int main() {
    const size_t stop = 170641;
    const size_t nseq = 10;

    std::deque<int> book = {3, 7};
    size_t first = 0, second = 1;
    while (book.size() < stop + nseq) {
        int sum = book[first] + book[second];
        if (sum >= 10)
            book.push_back(1);
        book.push_back(sum % 10);
        first = (first + book[first] + 1) % book.size();
        second = (second + book[second] + 1) % book.size();

        // for (size_t i = 0; i < book.size(); ++i) {
        //     if (i == first)
        //         std::clog << '(' << book[i] << ')';
        //     else if (i == second)
        //         std::clog << '[' << book[i] << ']';
        //     else
        //         std::clog << ' ' << book[i] << ' ';
        // }
        // std::clog << '\n';
    }
    std::copy(book.begin() + stop, book.begin() + (stop + nseq),
              std::ostream_iterator<int>{std::cout});
    std::cout << '\n';
}
