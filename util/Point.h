#pragma once

class Point {
private:
    int x;
    int y;

public:
    Point() : x(0), y(0) {}
    Point(int _x, int _y) : x(_x), y(_y) {}

    Point move(int delta_x, int delta_y) const {
        return Point(this->x + delta_x, this->y + delta_y);
    }

    int get_x() const {
        return x;
    }

    int get_y() const {
        return y;
    }
};