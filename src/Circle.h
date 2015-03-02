#ifndef CIRCLE_H
#define CIRCLE_H

#include <stack>
#include "Shape.h"
#include "ShapeFactory.h"
using namespace std;

class Circle : public Shape
{
    public:
        Circle();
        virtual string GetName();
        virtual vector<string> GetMethodsName();
        virtual unsigned int GetID();
        virtual void Draw(HDC &hdc);
        virtual Shape* Clone();
    private:
        class CircleAgent {
            public:
                CircleAgent() { ShapeFactory::RegisterShape(222,CircleAgent::CreateShape); }
                static Shape* CreateShape() { Shape *p = new Circle(); return p; }
        };

        static CircleAgent myAgent;

        virtual void DrawCircleCrt(HDC &hdc);
        virtual void DrawCircleMPA(HDC &hdc);
        virtual void FillCircle(HDC &hdc);
};


Circle::CircleAgent Circle::myAgent;

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

Circle::Circle(){
    drawAlg = 1;
    fillFlag = false;
    outColorRGB = 0;
    inColorRGB = ((255<<16)|(255<<8)|(255));
}

//Override
string Circle::GetName(){
    cout<<"Circle::GetName"<<endl;
    return "Circle";
}

vector<string> Circle::GetMethodsName(){
    cout<<"Circle::GetMethodsName"<<endl;
    vector<string> s(2);
    s[0]="Crt";
    s[1]="MPA";

    return s;
}

unsigned int Circle::GetID(){
    cout<<"Circle::GetID"<<endl;
    return 222;
}

void Circle::Draw(HDC &hdc){
    cout<<"Circle::Draw"<<endl;
    if((startPoint.x == endPoint.x)&&(startPoint.y == endPoint.y)) return;

    if(drawAlg == 0) this->DrawCircleCrt(hdc);
    else if(drawAlg == 1) this->DrawCircleMPA(hdc);
}

Shape* Circle::Clone(){
    Shape* clonedShape = new Circle();

    clonedShape->startPoint   = this->startPoint;
    clonedShape->endPoint     = this->endPoint;
    clonedShape->drawAlg      = this->drawAlg;
    clonedShape->fillFlag     = this->fillFlag;
    clonedShape->outColorRGB  = this->outColorRGB;
    clonedShape->inColorRGB   = this->inColorRGB;

    return clonedShape;
}

/////////////////////////////////////////////////////////////////////

void Circle::DrawCircleCrt(HDC &hdc){
    cout<<"Circle::DrawCircleCrt"<<endl;
    int x1=min(startPoint.x,endPoint.x);
    int y1=min(startPoint.y,endPoint.y);
    int x2=max(startPoint.x,endPoint.x);
    int y2=max(startPoint.y,endPoint.y);

    double r=(((double)(min(x2-x1,y2-y1)))/2);
    int shiftx=round(((double)(x2+x1))/2);
    int shifty=round(((double)(y2+y1))/2);
    for(int x=0;x<=round(r);x++){
        int y = round(sqrt((r*r)-((double)(x*x))));
        SetPixel(hdc, x+shiftx , y+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc,-x+shiftx , y+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc, x+shiftx ,-y+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc,-x+shiftx ,-y+shifty,((COLORREF)outColorRGB));
    }

    if(fillFlag) FillCircle(hdc);
}

void Circle::DrawCircleMPA(HDC &hdc){
    cout<<"Circle::DrawCircleMPA"<<endl;
    int x1=min(startPoint.x,endPoint.x);
    int y1=min(startPoint.y,endPoint.y);
    int x2=max(startPoint.x,endPoint.x);
    int y2=max(startPoint.y,endPoint.y);

    int r=round(((double)(min(x2-x1,y2-y1)))/2);
    int shiftx=round(((double)(x2+x1))/2);
    int shifty=round(((double)(y2+y1))/2);

    int x=0, y=r;
    int d=1-r;
    int dE     =((2*x)+3);
    int dSE    =((2*x)-(2*y)+5);
    int ddE_E  =2;
    int ddE_SE =2;
    int ddSE_E =2;
    int ddSE_SE=4;

    while(y>=x){
        SetPixel(hdc, x+shiftx , y+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc,-x+shiftx , y+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc, x+shiftx ,-y+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc,-x+shiftx ,-y+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc, y+shiftx , x+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc,-y+shiftx , x+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc, y+shiftx ,-x+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc,-y+shiftx ,-x+shifty,((COLORREF)outColorRGB));
        if(d<0){ d+=dE; dE+=ddE_E; dSE+=ddE_SE; x++; }
        else { d+=dSE; dE+=ddSE_E; dSE+=ddSE_SE; x++; y--; }
    }

    if(fillFlag) FillCircle(hdc);
}

void Circle::FillCircle(HDC &hdc){
    cout<<"Circle::FillCircle"<<endl;
}



/*
void Circle::FillCircle(HDC &hdc){
    cout<<"Circle::FillCircle"<<endl;

    int x = (abs(startPoint.x+endPoint.x))/2;
    int y = (abs(startPoint.y+endPoint.y))/2;
    int oldColorRGB = GetPixel(hdc,x,y);
    int tempColorRGB;
    stack<int> xStack, yStack;
    xStack.push(x);
    yStack.push(y);

    while(!(xStack.empty())){
        x=xStack.top(); xStack.pop();
        y=yStack.top(); yStack.pop();
        SetPixel(hdc,x,y,((COLORREF)inColorRGB));
        tempColorRGB = GetPixel(hdc,x+1,y);
        if(tempColorRGB == oldColorRGB) { xStack.push(x+1); yStack.push(y); }
        tempColorRGB = GetPixel(hdc,x-1,y);
        if(tempColorRGB == oldColorRGB) { xStack.push(x-1); yStack.push(y); }
        tempColorRGB = GetPixel(hdc,x,y+1);
        if(tempColorRGB == oldColorRGB) { xStack.push(x); yStack.push(y+1); }
        tempColorRGB = GetPixel(hdc,x,y-1);
        if(tempColorRGB == oldColorRGB) { xStack.push(x); yStack.push(y-1); }
    }
}
*/

#endif // CIRCLE_H
