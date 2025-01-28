#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <set>
#include <cctype>
#include <cmath>

struct ChargerInfo {
    // The overall coverage range for this charger:
    // from min_start to max_end (inclusive of gaps in between).
    unsigned long long min_start;
    unsigned long long max_end;
    // A list of intervals where this charger is "up".
    // We will merge these intervals later.
    std::vector<std::pair<unsigned long long, unsigned long long>> up_intervals;
    // Indicator whether min_start and max_end have been initialized
    bool initialized = false;
};

// Merge overlapping or contiguous intervals in a sorted list of intervals.
std::vector<std::pair<unsigned long long, unsigned long long>>
unify_intervals(std::vector<std::pair<unsigned long long, unsigned long long>>& intervals)
{
    if (intervals.empty()) {
        return {};
    }
    // Sort intervals by start time
    std::sort(intervals.begin(), intervals.end(),
              [](auto &a, auto &b) {
                  return a.first < b.first;
              });

    std::vector<std::pair<unsigned long long, unsigned long long>> merged;
    merged.reserve(intervals.size());

    auto current_start = intervals[0].first;
    auto current_end   = intervals[0].second;

    for (size_t i = 1; i < intervals.size(); ++i) {
        unsigned long long s = intervals[i].first;
        unsigned long long e = intervals[i].second;
        if (s <= current_end) {
            // Overlap or contiguous; extend current_end if needed
            if (e > current_end) {
                current_end = e;
            }
        } else {
            // No overlap; push the current interval, then reset
            merged.emplace_back(current_start, current_end);
            current_start = s;
            current_end   = e;
        }
    }
    // Push the last interval
    merged.emplace_back(current_start, current_end);

    return merged;
}

// Sum the lengths of a set of intervals
unsigned long long intervals_length(const std::vector<std::pair<unsigned long long,
                                                unsigned long long>>& intervals)
{
    unsigned long long total = 0;
    for (auto &iv : intervals) {
        total += (iv.second - iv.first);
    }
    return total;
}

// Trim leading/trailing whitespace
static inline std::string trim(const std::string &s)
{
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }
    return s.substr(start, end - start);
}

// Checks if a string is all digits (unsigned)
bool is_all_digits(const std::string &str)
{
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[])
{
    // Must receive exactly one argument (the input file path)
    if (argc != 2) {
        std::cout << "ERROR" << std::endl;
        return 0;
    }
    std::string input_path = argv[1];

    std::ifstream fin(input_path);
    if (!fin.is_open()) {
        std::cout << "ERROR" << std::endl;
        return 0;
    }

    // Read all non-empty lines
    std::vector<std::string> lines;
    {
        std::string line;
        while (std::getline(fin, line)) {
            line = trim(line);
            if (!line.empty()) {
                lines.push_back(line);
            }
        }
    }
    fin.close();

    // We need to find two sections:
    // [Stations]
    // [Charger Availability Reports]

    // Data structures
    std::unordered_map<unsigned long long, unsigned long long> charger_to_station; // charger_id -> station_id
    std::set<unsigned long long> station_ids;

    std::unordered_map<unsigned long long, ChargerInfo> charger_data; // charger_id -> ChargerInfo

    // We'll parse lines to find "[Stations]" and "[Charger Availability Reports]"
    const std::string STATIONS_HEADER = "[Stations]";
    const std::string REPORTS_HEADER  = "[Charger Availability Reports]";

    size_t idx = 0;
    const size_t n = lines.size();

    // 1) Find [Stations]
    while (idx < n && lines[idx] != STATIONS_HEADER) {
        idx++;
    }
    if (idx >= n) {
        // no stations header found
        std::cout << "ERROR" << std::endl;
        return 0;
    }
    idx++; // skip the line "[Stations]"

    // 2) Read station lines until [Charger Availability Reports]
    while (idx < n && lines[idx] != REPORTS_HEADER) {
        // parse station + chargers
        std::istringstream iss(lines[idx]);
        std::vector<std::string> tokens;
        {
            std::string t;
            while (iss >> t) {
                tokens.push_back(t);
            }
        }
        if (tokens.size() < 2) {
            // Must have at least 1 station ID + 1 charger ID
            std::cout << "ERROR" << std::endl;
            return 0;
        }

        // First token is station_id
        if (!is_all_digits(tokens[0])) {
            std::cout << "ERROR" << std::endl;
            return 0;
        }
        unsigned long long station_id = std::stoull(tokens[0]);
        station_ids.insert(station_id);

        // The rest are charger IDs
        for (size_t i = 1; i < tokens.size(); ++i) {
            if (!is_all_digits(tokens[i])) {
                std::cout << "ERROR" << std::endl;
                return 0;
            }
            unsigned long long cid = std::stoull(tokens[i]);

            // If we already have this charger mapped to a station, it's an error
            if (charger_to_station.find(cid) != charger_to_station.end()) {
                // Charger claimed by two different stations => error
                std::cout << "ERROR" << std::endl;
                return 0;
            }
            charger_to_station[cid] = station_id;
        }
        idx++;
    }

    // Check if we have the [Charger Availability Reports] header
    if (idx >= n || lines[idx] != REPORTS_HEADER) {
        // no reports header found
        std::cout << "ERROR" << std::endl;
        return 0;
    }
    idx++; // skip the "[Charger Availability Reports]" line

    // 3) Parse each report line
    while (idx < n) {
        std::istringstream iss(lines[idx]);
        std::vector<std::string> tokens;
        {
            std::string t;
            while (iss >> t) {
                tokens.push_back(t);
            }
        }
        if (tokens.size() != 4) {
            // Must be exactly 4 tokens: cid, start, end, up(true/false)
            std::cout << "ERROR" << std::endl;
            return 0;
        }
        // <Charger ID> <start time> <end time> <up>
        if (!is_all_digits(tokens[0]) ||
            !is_all_digits(tokens[1]) ||
            !is_all_digits(tokens[2]) ||
            (tokens[3] != "true" && tokens[3] != "false"))
        {
            std::cout << "ERROR" << std::endl;
            return 0;
        }

        unsigned long long cid   = std::stoull(tokens[0]);
        unsigned long long start = std::stoull(tokens[1]);
        unsigned long long end   = std::stoull(tokens[2]);
        bool up_val = (tokens[3] == "true");

        // check if charger ID is known from [Stations]
        if (charger_to_station.find(cid) == charger_to_station.end()) {
            // unknown charger => error
            std::cout << "ERROR" << std::endl;
            return 0;
        }

        // update charger data
        auto &info = charger_data[cid];
        if (!info.initialized) {
            info.min_start = start;
            info.max_end   = end;
            info.initialized = true;
        } else {
            if (start < info.min_start) {
                info.min_start = start;
            }
            if (end > info.max_end) {
                info.max_end = end;
            }
        }
        if (up_val) {
            info.up_intervals.push_back({start, end});
        }

        idx++;
    }

    // 4) For each charger, unify its up_intervals
    for (auto &kv : charger_data) {
        auto &info = kv.second;
        auto merged = unify_intervals(info.up_intervals);
        info.up_intervals = merged;
    }

    // We now want to compute station coverage and station up intervals
    // station_coverage[s] = list of intervals
    // station_up[s]       = list of intervals
    std::map<unsigned long long, std::vector<std::pair<unsigned long long, unsigned long long>>> station_coverage;
    std::map<unsigned long long, std::vector<std::pair<unsigned long long, unsigned long long>>> station_up;

    // Initialize these maps with empty vectors for each known station
    for (auto sid : station_ids) {
        station_coverage[sid] = {};
        station_up[sid]       = {};
    }

    // Fill station coverage and up intervals from each charger
    // The coverage for each charger is the single big interval [min_start, max_end]
    for (auto &kv : charger_data) {
        unsigned long long cid = kv.first;
        ChargerInfo &info = kv.second;
        unsigned long long s_id = charger_to_station[cid];

        // coverage
        station_coverage[s_id].push_back({info.min_start, info.max_end});
        // up intervals
        for (auto &iv : info.up_intervals) {
            station_up[s_id].push_back(iv);
        }
    }

    // Merge coverage intervals and up intervals per station
    // Then compute final uptime
    std::vector<std::pair<unsigned long long, unsigned long long>> results;
    results.reserve(station_ids.size());

    for (auto sid : station_ids) {
        // unify coverage
        auto coverage_merged = unify_intervals(station_coverage[sid]);
        auto up_merged       = unify_intervals(station_up[sid]);

        unsigned long long coverage_len = intervals_length(coverage_merged);
        unsigned long long up_len       = intervals_length(up_merged);

        unsigned long long uptime = 0ULL;
        if (coverage_len > 0ULL) {
            // integer floor of the ratio * 100
            // e.g. up_len = 150, coverage_len = 200 => 150*100/200 = 75
            uptime = (up_len * 100ULL) / coverage_len;
        }

        results.push_back({sid, uptime});
    }

    // Print results in ascending station order
    // We already traversed station_ids in ascending order since it's a std::set
    for (auto &res : results) {
        std::cout << res.first << " " << res.second << std::endl;
    }

    return 0;
}
