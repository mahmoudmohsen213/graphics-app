#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <stack>
#include "Shape.h"
#include "ShapeFactory.h"
using namespace std;

class Ellipse : public Shape
{
    public:
        Ellipse();
        virtual string GetName();
        virtual vector<string> GetMethodsName();
        virtual unsigned int GetID();
        virtual void Draw(HDC &hdc);
        virtual Shape* Clone();
    private:
        class EllipseAgent {
            public:
                EllipseAgent() { ShapeFactory::RegisterShape(333,EllipseAgent::CreateShape); }
                static Shape* CreateShape() { Shape *p = new Ellipse(); return p; }
        };

        static EllipseAgent myAgent;

        virtual void DrawEllipseCrt(HDC &hdc);
        virtual void DrawEllipseMPA(HDC &hdc);
        virtual void FillEllipse(HDC &hdc);
};


Ellipse::EllipseAgent Ellipse::myAgent;

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

Ellipse::Ellipse(){
    drawAlg = 1;
    fillFlag = false;
    outColorRGB = 0;
    inColorRGB = ((255<<16)|(255<<8)|(255));
}

//Override
string Ellipse::GetName(){
    cout<<"Ellipse::GetName"<<endl;
    return "Ellipse";
}

vector<string> Ellipse::GetMethodsName(){
    cout<<"Ellipse::GetMethodsName"<<endl;
    vector<string> s(2);
    s[0]="Crt";
    s[1]="MPA";

    return s;
}

unsigned int Ellipse::GetID(){
    cout<<"Ellipse::GetID"<<endl;
    return 333;
}

void Ellipse::Draw(HDC &hdc){
    cout<<"Ellipse::Draw"<<endl;
    if((startPoint.x == endPoint.x)&&(startPoint.y == endPoint.y)) return;

    if(drawAlg == 0) this->DrawEllipseCrt(hdc);
    else if(drawAlg == 1) this->DrawEllipseMPA(hdc);
}

Shape* Ellipse::Clone(){
    Shape* clonedShape = new Ellipse();

    clonedShape->startPoint   = this->startPoint;
    clonedShape->endPoint     = this->endPoint;
    clonedShape->drawAlg      = this->drawAlg;
    clonedShape->fillFlag     = this->fillFlag;
    clonedShape->outColorRGB  = this->outColorRGB;
    clonedShape->inColorRGB   = this->inColorRGB;

    return clonedShape;
}

/////////////////////////////////////////////////////////////////////

void Ellipse::DrawEllipseCrt(HDC &hdc){
    cout<<"Ellipse::DrawEllipseCrt"<<endl;
    int x1=min(startPoint.x,endPoint.x);
    int y1=min(startPoint.y,endPoint.y);
    int x2=max(startPoint.x,endPoint.x);
    int y2=max(startPoint.y,endPoint.y);

    double a=(((double)(x2-x1))/2);
    double b=(((double)(y2-y1))/2);
    int shiftx=round(((double)(x2+x1))/2);
    int shifty=round(((double)(y2+y1))/2);
    if(a>b){
        for(int x=0;x<=round(a);x++){
            int y = round(sqrt((((a*a*b*b)-((double)(b*b*x*x)))/(a*a))));
            SetPixel(hdc, x+shiftx , y+shifty,((COLORREF)outColorRGB));
            SetPixel(hdc,-x+shiftx , y+shifty,((COLORREF)outColorRGB));
            SetPixel(hdc, x+shiftx ,-y+shifty,((COLORREF)outColorRGB));
            SetPixel(hdc,-x+shiftx ,-y+shifty,((COLORREF)outColorRGB));
        }
    }
    else{
        for(int y=0;y<=round(b);y++){
            int x = round(sqrt((((a*a*b*b)-((double)(a*a*y*y)))/(b*b))));
            SetPixel(hdc, x+shiftx , y+shifty,((COLORREF)outColorRGB));
            SetPixel(hdc,-x+shiftx , y+shifty,((COLORREF)outColorRGB));
            SetPixel(hdc, x+shiftx ,-y+shifty,((COLORREF)outColorRGB));
            SetPixel(hdc,-x+shiftx ,-y+shifty,((COLORREF)outColorRGB));
        }
    }

    if(fillFlag) FillEllipse(hdc);
}

void Ellipse::DrawEllipseMPA(HDC &hdc){
    cout<<"Ellipse::DrawEllipseMPA"<<endl;

    int x1=min(startPoint.x,endPoint.x);
    int y1=min(startPoint.y,endPoint.y);
    int x2=max(startPoint.x,endPoint.x);
    int y2=max(startPoint.y,endPoint.y);

    double a=(((double)(x2-x1))/2), sqA=a*a;
    double b=(((double)(y2-y1))/2), sqB=b*b;
    int shiftx=round(((double)(x2+x1))/2);
    int shifty=round(((double)(y2+y1))/2);

    int x=0, y=round(b);
    double d=(b*b)-(a*a*b)+(a*a*0.25);
    double dE     =(3*b*b);
    double dSE    =(3*b*b)-(2*a*a*b)+(8*a*a*0.25);
    double ddE_E  =2*b*b;
    double ddE_SE =2*b*b;
    double ddSE_E =2*b*b;
    double ddSE_SE=(2*b*b)+(8*a*a*0.25);

    while ((sqA*((double)(y+1)))>=(sqB*((double)x))){
        SetPixel(hdc, x+shiftx , y+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc,-x+shiftx , y+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc, x+shiftx ,-y+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc,-x+shiftx ,-y+shifty,((COLORREF)outColorRGB));
        if(d<0){ d+=dE; dE+=ddE_E; dSE+=ddE_SE; x++; }
        else { d+=dSE; dE+=ddSE_E; dSE+=ddSE_SE; x++; y--; }
    }

    swap(a,b); swap(sqA,sqB);
    x=0, y=round(b);
    d=(b*b)-(a*a*b)+(a*a*0.25);
    dE     =(3*b*b);
    dSE    =(3*b*b)-(2*a*a*b)+(8*a*a*0.25);
    ddE_E  =2*b*b;
    ddE_SE =2*b*b;
    ddSE_E =2*b*b;
    ddSE_SE=(2*b*b)+(8*a*a*0.25);
    while ((sqA*((double)(y+1)))>=(sqB*((double)x))){
        SetPixel(hdc, y+shiftx , x+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc,-y+shiftx , x+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc, y+shiftx ,-x+shifty,((COLORREF)outColorRGB));
        SetPixel(hdc,-y+shiftx ,-x+shifty,((COLORREF)outColorRGB));
        if(d<0){ d+=dE; dE+=ddE_E; dSE+=ddE_SE; x++; }
        else { d+=dSE; dE+=ddSE_E; dSE+=ddSE_SE; x++; y--; }
    }

    if(fillFlag) FillEllipse(hdc);
}

void Ellipse::FillEllipse(HDC &hdc){
    cout<<"Ellipse::FillEllipse"<<endl;
}

/*
void Ellipse::FillEllipse(HDC &hdc){
    cout<<"Ellipse::FillEllipse"<<endl;

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

#endif // ELLIPSE_H
