#include <algorithm>
#include <iostream>
#include <iterator>
#include <deque>

int main() {
    const std::deque stop = {1, 7, 0, 6, 4, 1};

    std::deque book = {3, 7};
    size_t first = 0, second = 1;
    while (true) {
        int sum = book[first] + book[second];
        if (sum >= 10)
            book.push_back(1);
        if (book.size() >= stop.size()
            && std::equal(stop.rbegin(), stop.rend(), book.rbegin())) {
            std::cout << book.size() - stop.size() << '\n';
            return 0;
        }
        book.push_back(sum % 10);
        char c = *(stop.rbegin());
        if (book.size() >= stop.size()
            && std::equal(stop.rbegin(), stop.rend(), book.rbegin())) {
            std::cout << book.size() - stop.size() << '\n';
            return 0;
        }
        first = (first + book[first] + 1) % book.size();
        second = (second + book[second] + 1) % book.size();
    }
}
