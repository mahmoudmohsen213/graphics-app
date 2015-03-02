#ifndef POINT_H
#define POINT_H


class Point {
    public:
        int x;
        int y;

        Point();
        Point(const int &_x, const int &_y);
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

Point::Point() { x=0; y=0; return; }
Point::Point(const int &_x, const int &_y) { x=_x; y=_y; return; }

#endif // POINT_H
