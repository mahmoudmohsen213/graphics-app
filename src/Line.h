#ifndef LINE_H
#define LINE_H

#include "Shape.h"
#include "ShapeFactory.h"

class Line : public Shape
{
    public:
        Line();
        virtual string GetName();
        virtual vector<string> GetMethodsName();
        virtual unsigned int GetID();
        virtual void Draw(HDC &hdc);
        virtual Shape* Clone();
    private:
        class LineAgent {
            public:
                LineAgent() { ShapeFactory::RegisterShape(111,LineAgent::CreateShape); }
                static Shape* CreateShape() { Shape *p = new Line(); return p; }
        };

        static LineAgent myAgent;

        virtual void DrawLineSDDA(HDC &hdc);
        virtual void DrawLineIDDA(HDC &hdc);
        virtual void DrawLineMPA(HDC &hdc);
};

Line::LineAgent Line::myAgent;

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

Line::Line(){
    drawAlg = 2;
    fillFlag = false;
    outColorRGB = 0;
    inColorRGB = ((255<<16)|(255<<8)|(255));
}

//Override
string Line::GetName(){
    cout<<"Line::GetName"<<endl;
    return "Line";
}

vector<string> Line::GetMethodsName(){
    cout<<"Line::GetMethodsName"<<endl;
    vector<string> s(3);
    s[0]="SDDA";
    s[1]="IDDA";
    s[2]="MPA";

    return s;
}

unsigned int Line::GetID(){
    cout<<"Line::GetID"<<endl;
    return 111;
}

void Line::Draw(HDC &hdc){
    cout<<"Line::Draw"<<endl;

    if(drawAlg == 0) this->DrawLineSDDA(hdc);
    else if(drawAlg == 1) this->DrawLineIDDA(hdc);
    else if(drawAlg == 2)this->DrawLineMPA(hdc);
}

Shape* Line::Clone(){
    Shape* clonedShape = new Line();

    clonedShape->startPoint   = this->startPoint;
    clonedShape->endPoint     = this->endPoint;
    clonedShape->drawAlg      = this->drawAlg;
    clonedShape->fillFlag     = this->fillFlag;
    clonedShape->outColorRGB  = this->outColorRGB;
    clonedShape->inColorRGB   = this->inColorRGB;

    return clonedShape;
}

/////////////////////////////////////////////////////////////////////

void Line::DrawLineSDDA(HDC &hdc){
    cout<<"Line::DrawLineSDDA"<<endl;

    int x1=startPoint.x, y1=startPoint.y;
    int x2=endPoint.x, y2=endPoint.y;
    if(x1==x2) {
        if(y1>y2) { swap(x1,x2); swap(y1,y2); }
        for(int i=y1;i<=y2;i++) SetPixel(hdc,x1,i,((COLORREF)outColorRGB));
        return;
    }
    else if(y1==y2) {
        if(x1>x2) { swap(x1,x2); swap(y1,y2); }
        for(int i=x1;i<=x2;i++) SetPixel(hdc,i,y1,((COLORREF)outColorRGB));
        return;
    }
    else{
        double m=(((double)(y1-y2))/((double)(x1-x2)));
        double d=((double)y1)-(m*((double)x1));
        if((m<=1.0)&&(m>=-1.0)){
            if(x1>x2) { swap(x1,x2); swap(y1,y2); }
            for(int x=x1;x<=x2;x++) {
                int y=round((m*((double)x))+d);
                SetPixel(hdc,x,y,((COLORREF)outColorRGB));
            }
        }
        else{
            if(y1>y2) { swap(x1,x2); swap(y1,y2); }
            for(int y=y1;y<=y2;y++) {
                int x=round((((double)y)-d)/m);
                SetPixel(hdc,x,y,((COLORREF)outColorRGB));
            }
        }
    }
    return;
}

void Line::DrawLineIDDA(HDC &hdc){
    cout<<"Line::DrawLineIDDA"<<endl;

    int x1=startPoint.x, y1=startPoint.y;
    int x2=endPoint.x, y2=endPoint.y;
    if(x1==x2) {
        if(y1>y2) { swap(x1,x2); swap(y1,y2); }
        for(int i=y1;i<=y2;i++) SetPixel(hdc,x1,i,((COLORREF)outColorRGB));
        return;
    }
    else if(y1==y2) {
        if(x1>x2) { swap(x1,x2); swap(y1,y2); }
        for(int i=x1;i<=x2;i++) SetPixel(hdc,i,y1,((COLORREF)outColorRGB));
        return;
    }
    else{
        double m=(((double)(y1-y2))/((double)(x1-x2)));
        double d=((double)y1)-(m*((double)x1));
        if((m<=1.0)&&(m>=-1.0)){
            if(x1>x2) { swap(x1,x2); swap(y1,y2); }
            double y=(m*((double)x1))+d;
            for(int x=x1;x<=x2;x++) {
                SetPixel(hdc,x,round(y),((COLORREF)outColorRGB));
                y+=m;
            }
        }
        else{
            if(y1>y2) { swap(x1,x2); swap(y1,y2); }
            double x=(((double)y1)-d)/m; m=(1/m);
            for(int y=y1;y<=y2;y++) {
                SetPixel(hdc,round(x),y,((COLORREF)outColorRGB));
                x+=m;
            }
        }
    }
    return;
}

void Line::DrawLineMPA(HDC &hdc){
    cout<<"Line::DrawLineMPA"<<endl;

    int x1=startPoint.x, y1=startPoint.y;
    int x2=endPoint.x, y2=endPoint.y;
    if(x1==x2) {
        if(y1>y2) swap(y1,y2);
        for(int i=y1;i<=y2;i++) SetPixel(hdc,x1,i,((COLORREF)outColorRGB));
        return;
    }

    double m=(((double)(y1-y2))/((double)(x1-x2)));
    bool o1=((0.0<=m)&&(m<=1.0));
    bool o2=((-1.0<=m)&&(m<=0.0));
    bool o3=(1.0<=m);
    bool o4=(m<=-1.0);

    x1-=startPoint.x; y1-=startPoint.y;
    x2-=startPoint.x; y2-=startPoint.y;

    if(o2) { y2=-1*y2; }
    else if(o3) { swap(x2,y2); }
    else if(o4) { y2=-1*y2; swap(x2,y2); }

    if(x1>x2) { swap(x1,x2); swap(y1,y2); }
    int d  =(2*(y1-y2))-(x1-x2);
    int dE =(2*(y1-y2));
    int dNE=(2*(y1-y2))-(2*(x1-x2));

    while(x1<=x2){
        if(o1) SetPixel(hdc,x1+startPoint.x,y1+startPoint.y,((COLORREF)outColorRGB));
        else if(o2) SetPixel(hdc,x1+startPoint.x,-y1+startPoint.y,((COLORREF)outColorRGB));
        else if(o3) SetPixel(hdc,y1+startPoint.x,x1+startPoint.y,((COLORREF)outColorRGB));
        else if(o4) SetPixel(hdc,y1+startPoint.x,-x1+startPoint.y,((COLORREF)outColorRGB));

        if(d>=0) { d+=dE; x1++; }
        else { d+=dNE; x1++; y1++; }
    }

    return;
}

/////////////////////////////////////////////////////////////////////
/*
void Line::DrawLineMPA(HDC &hdc){
    cout<<"Line::DrawLineMPA"<<endl;
    if(startPoint.x==endPoint.x) {
        if(startPoint.y<endPoint.y) swap(startPoint,endPoint);
        for(int i=startPoint.y;i<=endPoint.y;i++) SetPixel(hdc,startPoint.x,i,((COLORREF)outColorRGB));
    }

    // the algorithm is based on the fact that in the cartesian plan, moving down DECRASE the y-value
    // but this fact is not true in  the application window, in the window moving down INCREASE the y-value
    // therefore this implementation is WRONG

    double m=(((double)(startPoint.y-endPoint.y))/((double)(startPoint.x-endPoint.x)));
    //cout<<startPoint.x<<"  "<<startPoint.y<<"  "<<endPoint.x<<"  "<<endPoint.y<<"  "<<m<<endl;
    if((0.0<=m)&&(m<=1.0)){
        if(startPoint.x>endPoint.x) swap(startPoint,endPoint);
        int d  =(2*(startPoint.y-endPoint.y))-(startPoint.x-endPoint.x);
        int dE =(2*(startPoint.y-endPoint.y));
        int dNE=(2*(startPoint.y-endPoint.y))-(2*(startPoint.x-endPoint.x));

        while(startPoint.x<=endPoint.x){
            SetPixel(hdc,startPoint.x,startPoint.y,((COLORREF)outColorRGB));
            if(d<=0) { d+=dE; startPoint.x++; }
            else { d+=dNE; startPoint.x++; startPoint.y++; }
        }
    }

    else if((-1.0<=m)&&(m<=0.0)){
        if(startPoint.x>endPoint.x) swap(startPoint,endPoint);
        int d  =(2*(startPoint.y-endPoint.y))+(startPoint.x-endPoint.x);
        int dE =(2*(startPoint.y-endPoint.y));
        int dSE=(2*(startPoint.y-endPoint.y))+(2*(startPoint.x-endPoint.x));

        while(startPoint.x<=endPoint.x){
            SetPixel(hdc,startPoint.x,startPoint.y,((COLORREF)outColorRGB));
            if(d<=0) { d+=dSE; startPoint.x++; startPoint.y--; }
            else { d+=dE; startPoint.x++; }
        }
    }

    else if(1.0<=m){
        if(startPoint.y<endPoint.y) swap(startPoint,endPoint);
        int d  =(startPoint.y-endPoint.y)-(2*(startPoint.x-endPoint.x));
        int dNE=(2*(startPoint.y-endPoint.y))-(2*(startPoint.x-endPoint.x));
        int dN =(-2*(startPoint.x-endPoint.x));

        while(startPoint.y<=endPoint.y){
            SetPixel(hdc,startPoint.x,startPoint.y,((COLORREF)outColorRGB));
            if(d<=0) { d+=dNE; startPoint.x++; startPoint.y++; }
            else { d+=dN; startPoint.y++; }
        }
    }

    else if(m<=-1.0){
        if(startPoint.y<endPoint.y) swap(startPoint,endPoint);
        int d  =(-1*(startPoint.y-endPoint.y))-(2*(startPoint.x-endPoint.x));
        int dN =(-2*(startPoint.x-endPoint.x));
        int dNW=(-2*(startPoint.y-endPoint.y))-(2*(startPoint.x-endPoint.x));

        while(startPoint.y<=endPoint.y){
            SetPixel(hdc,startPoint.x,startPoint.y,((COLORREF)outColorRGB));
            if(d<=0) { d+=dN; startPoint.y++; }
            else { d+=dNW; startPoint.x++; startPoint.y++; }
        }
    }
    return;
}
*/
/////////////////////////////////////////////////////////////////////

#endif // LINE_H
