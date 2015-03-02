#ifndef SHAPE_H
#define SHAPE_H

#include <iostream>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <math.h>
#include <string>
#include "Point.h"
using namespace std;

class Shape {
    public:
        Point startPoint;
        Point endPoint;
        bool fillFlag;
        int drawAlg;
        int outColorRGB;
        int inColorRGB;

        Shape() {}
        virtual ~Shape() {}
        virtual string GetName() = 0;
        virtual vector<string> GetMethodsName() = 0;
        virtual unsigned int GetID() = 0;
        virtual void Draw(HDC &hdc) = 0;
        virtual Shape* Clone() = 0;
};

#endif // SHAPE_H
