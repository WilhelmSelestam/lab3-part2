#include <iostream>
#include <format>
#include <compare>
#include <fstream>
#include <filesystem>

#include <cassert>
#include <string>
#include <fstream>
#include <algorithm>

#include <utility>

#include <iostream>
#include <vector>
#include <filesystem>

#include <limits>

// #include <../../renderingSystem/src/readfiles.cpp>
#include <find-patterns.hpp>
#include <unordered_map>

const std::filesystem::path data_dir{DATA_DIR};

constexpr int minPoints = 4;

struct Point {  // feel free to modify
public:
    Point(int x = 0, int y = 0) : x_{x}, y_{y} {}

    bool operator==(const Point& p) const = default;

    // Compare by y-coordinates and break ties by x-coordinates
    std::strong_ordering operator<=>(Point p) const {
        if (y_ < p.y_) {
            return std::strong_ordering::less;
        }
        if (y_ > p.y_) {
            return std::strong_ordering::greater;
        }
        if (x_ < p.x_) {
            return std::strong_ordering::less;
        }
        if (x_ > p.x_) {
            return std::strong_ordering::greater;
        }
        return std::strong_ordering::equivalent;
    }

    std::string toString() const { return std::format("({},{})", x_, y_); }

    long long x_;
    long long y_;
};

/* ***************************************************** */

int main() {
    std::cout << "Enter the name of input points file: ";
    std::string points_file;
    std::cin >> points_file;

    // points_file = "C:/Skola/TND004 - Datastrukturer/lab3-part2/detectionSystem/data/points5.txt";

    analyseData(points_file);
}

/* ***************************************************** */

double calculate_slope(const Point& p1, const Point& p2) {
    long long dx = p2.x_ - p1.x_;
    long long dy = p2.y_ - p1.y_;

    if (dx == 0 && dy == 0) {
        return -std::numeric_limits<double>::infinity();
    }
    if (dx == 0) {
        return std::numeric_limits<double>::infinity();
    }
    if (dy == 0) {
        return 0.0;
    }

    return static_cast<double>(dy) / static_cast<double>(dx);
}

bool are_slopes_equal(double s1, double s2) {
    if (std::isinf(s1) && std::isinf(s2)) {
        return s1 == s2;
    }
    if (std::isinf(s1) || std::isinf(s2)) {
        return false;
    }

    if (std::isnan(s1) || std::isnan(s2)) {
        return false;
    }

    constexpr double epsilon = 1e-9;
    return std::abs(s1 - s2) < epsilon;
}

//bool are_collinear(const Point& p1, const Point& p2, const Point& p3) {
//    long long left_side = (p2.y_ - p1.y_) * (p3.x_ - p1.x_);
//    long long right_side = (p3.y_ - p1.y_) * (p2.x_ - p1.x_);
//    return left_side == right_side;
//}

void findCollinearSegments(const std::vector<Point>& points,
                           std::vector<std::vector<Point>>& result_segments) {
    int n = points.size();
    if (n < minPoints) {
        return;
    }

    std::vector<Point> sorted_points = points;
    std::sort(sorted_points.begin(), sorted_points.end());

    result_segments.clear();

    for (int i = 0; i < n; ++i) {
        Point p = sorted_points[i];

        //std::unordered_map<int, std::vector<Point>> collinear_groups;

        std::vector<std::pair<Point, double>> points_with_slopes;
        points_with_slopes.reserve(n);

        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            Point q = sorted_points[j];
            double slope = calculate_slope(p, q);
            points_with_slopes.push_back({q, slope});
        }

        std::stable_sort(points_with_slopes.begin(), points_with_slopes.end(),
                         [](const auto& a, const auto& b) {
                             // Primary sort key: slope
                             if (!are_slopes_equal(a.second, b.second)) {
                                 if (a.second == -std::numeric_limits<double>::infinity())
                                     return true;
                                 if (b.second == -std::numeric_limits<double>::infinity())
                                     return false;
                                 if (a.second == std::numeric_limits<double>::infinity())
                                     return false;
                                 if (b.second == std::numeric_limits<double>::infinity())
                                     return true;

                                 return a.second < b.second;
                             }
                             return false;
                         });

        int num_others = points_with_slopes.size();

        for (int k = 0; k < num_others; /* manual increment */) {

            if (points_with_slopes[k].second == -std::numeric_limits<double>::infinity()) {
                k++;
                continue;
            }

            int group_start_index = k;
            double current_slope = points_with_slopes[k].second;
            while (k < num_others &&
                   are_slopes_equal(current_slope, points_with_slopes[k].second)) {
                k++;
            }

            int group_size = k - group_start_index;

            if (group_size >= minPoints - 1) {
                std::vector<Point> segment;
                segment.reserve(group_size + 1);
                segment.push_back(p);

                for (int m = group_start_index; m < k; ++m) {
                    segment.push_back(points_with_slopes[m].first);
                }
                std::sort(segment.begin(), segment.end());
                if (!segment.empty() && segment.front() == p) {
                    result_segments.push_back(segment);
                }
            }
        }
    }
}

void analyseData(const std::filesystem::path& pointsFile, const std::filesystem::path& segmentsFile,
                 const std::filesystem::path& outputFileName) {
    /*
     * Add code here
     * Feel free to modify the function signature
     * Break your code into small functions
     */

    int n_points{0};
    std::ifstream is(pointsFile);
    is >> n_points;  // read number of particles

    std::vector<Point> points;
    points.reserve(n_points);

    for (int i = 0; i < n_points; ++i) {
        Point p;
        is >> p.x_ >> p.y_;
        points.push_back(p);
        // std::cout << p.x_ + "\n";
    }

    std::vector<std::vector<Point>> result_segments;
    findCollinearSegments(points, result_segments);

    std::ofstream WriteFile(outputFileName);
    std::ofstream WriteFile2(segmentsFile);

    for (int i = 0; i < result_segments.size(); i++) {
        // std::cout << "hej\n";
        WriteFile2 << result_segments[i][0].x_ << " " << result_segments[i][0].y_ << " "
                  << result_segments[i][result_segments[i].size() - 1].x_ << " "
                  << result_segments[i][result_segments[i].size() - 1].y_ << "\n";

        for (int j = 0; j < result_segments[i].size(); j++) {
            WriteFile << result_segments[i][j].toString();
            if (j != result_segments[i].size() - 1) {
                WriteFile << "->";
            }
        }
        WriteFile << "\n";
    }

    WriteFile.close();
    WriteFile2.close();
}

void analyseData(const std::string& name) {
    std::filesystem::path points_name = name;
    std::filesystem::path segments_name = "segments-" + name;

    analyseData(data_dir / points_name, data_dir / "output" / segments_name,
                data_dir / "output" / points_name);
}
