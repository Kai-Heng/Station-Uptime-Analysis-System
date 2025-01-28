#include <iostream>
#include <vector>
#include <utility>
#include <cassert>
#include "functions.h"

// Suppose we've extracted unify_intervals() and intervals_length() into "functions.h"
// so that we can test them separately from the main program.

void test_unify_intervals() {
    using Interval = std::pair<unsigned long long, unsigned long long>;

    // 1) Overlapping intervals
    std::vector<Interval> intervals1 = {
        {0ULL, 50ULL}, {20ULL, 100ULL}, {100ULL, 200ULL}
    };
    // Expect merge => (0,200)
    auto merged1 = unify_intervals(intervals1);
    assert(merged1.size() == 1);
    assert(merged1[0].first == 0ULL && merged1[0].second == 200ULL);

    // 2) Adjacent intervals
    std::vector<Interval> intervals2 = {
        {0ULL, 50ULL}, {50ULL, 100ULL}, {200ULL, 300ULL}
    };
    // Expect => (0,100), (200,300)
    auto merged2 = unify_intervals(intervals2);
    assert(merged2.size() == 2);
    assert(merged2[0].first == 0ULL && merged2[0].second == 100ULL);
    assert(merged2[1].first == 200ULL && merged2[1].second == 300ULL);

    // 3) No intervals
    std::vector<Interval> intervals3;
    auto merged3 = unify_intervals(intervals3);
    assert(merged3.empty());

    std::cout << "test_unify_intervals() passed." << std::endl;
}

void test_intervals_length() {
    using Interval = std::pair<unsigned long long, unsigned long long>;

    std::vector<Interval> intervals = {
        {0ULL, 50ULL}, {50ULL, 100ULL}, {200ULL, 300ULL}
    };
    // length = (50-0)+(100-50)+(300-200) = 50 + 50 + 100 = 200
    unsigned long long length = intervals_length(intervals);
    assert(length == 200ULL);

    std::cout << "test_intervals_length() passed." << std::endl;
}

int main() {
    test_unify_intervals();
    test_intervals_length();

    std::cout << "All unit tests passed!" << std::endl;
    return 0;
}

