#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <algorithm>

// Merges overlapping or adjacent intervals
inline std::vector<std::pair<unsigned long long, unsigned long long>>
unify_intervals(std::vector<std::pair<unsigned long long, unsigned long long>>& intervals)
{
    if (intervals.empty()) {
        return {};
    }
    std::sort(intervals.begin(), intervals.end(),
        [](auto &a, auto &b){ return a.first < b.first; });

    std::vector<std::pair<unsigned long long, unsigned long long>> merged;
    merged.reserve(intervals.size());

    auto current_start = intervals[0].first;
    auto current_end   = intervals[0].second;

    for (size_t i = 1; i < intervals.size(); ++i) {
        auto s = intervals[i].first;
        auto e = intervals[i].second;
        if (s <= current_end) {
            // Extend
            if (e > current_end) {
                current_end = e;
            }
        } else {
            merged.push_back({current_start, current_end});
            current_start = s;
            current_end   = e;
        }
    }
    merged.push_back({current_start, current_end});
    return merged;
}

// Returns sum of (end - start) for each interval
inline unsigned long long
intervals_length(const std::vector<std::pair<unsigned long long, unsigned long long>>& intervals)
{
    unsigned long long total = 0ULL;
    for (auto &iv : intervals) {
        total += (iv.second - iv.first);
    }
    return total;
}

#endif // FUNCTIONS_H

