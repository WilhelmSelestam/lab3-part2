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



//#include <../../renderingSystem/src/readfiles.cpp>
#include <find-patterns.hpp>

const std::filesystem::path data_dir{DATA_DIR};

constexpr int minPoints = 4;

struct Point { // feel free to modify
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

    //points_file = "C:/Skola/TND004 - Datastrukturer/lab3-part2/detectionSystem/data/points200.txt";

    analyseData(points_file);
}

/* ***************************************************** */


void analyseData(const std::filesystem::path& pointsFile,
                 const std::filesystem::path& segmentsFile) {
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
    }
    
    std::vector<std::vector<Point>> result;

    std::sort(points.begin(), points.end());

    for (Point p : points) {
        std::vector<std::pair<Point, double>> a;
        a.reserve(n_points - 1);
        for (Point p2 : points) {
            if (p.x_ != p2.x_) {
                double slope = (p2.y_ - p.y_) / (p2.x_ - p.x_);
                std::pair<Point, double> temp = std::make_pair(p2, slope);
                a.push_back(temp);
            }
        }
        std::stable_sort(a.begin(), a.end(), [&p]( std::pair<Point, double> a, std::pair<Point, double> b) {
            return a.second < b.second;
        });

        int n = a.size();
        int counter = 0;
        double prevSlope = 0;
        double slope = 0;
        std::vector<Point> line;

        for (int i = 0; i < n; i++) {
            int j = i+1;
            slope = a[i].second; 
            std::vector<Point> temp;
            temp.push_back(a[i].first);
            while (j < n - 1 && slope == a[j].second) {
                temp.push_back(a[j].first);
                j++;
            }
            if (temp.size() >= 3) {
                for (auto t : temp) {
                    line.push_back(t);
                }
            }

        }
        result.push_back(line);
        

    }


    std::ofstream WriteFile("test.txt");

    std::string s = "";

    for (int i = 0; i < result.size(); i++) {

        s = result[i][0].toString() + " " + result[i][result[i].size() - 1].toString() + "\n";
        WriteFile << s;

        std::cout << s;
    }
    
    WriteFile.close();

}

void analyseData(const std::string& name) {
    std::filesystem::path points_name = name;
    std::filesystem::path segments_name = "segments-" + name;

    analyseData(data_dir / points_name, data_dir / "output" / segments_name);
}
