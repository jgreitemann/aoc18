#include <deque>

template <typename T>
auto rotate(std::deque<T> & d,
            typename std::deque<T>::difference_type amount = 1)
{
    while (amount > 0) {
        d.push_back(d.front());
        d.pop_front();
        --amount;
    }
    while (amount < 0) {
        d.push_front(d.back());
        d.pop_back();
        ++amount;
    }
    return d.begin();
}
