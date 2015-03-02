#ifndef FRAMEMANAGER_H
#define FRAMEMANAGER_H

#include <iostream>
#include <windows.h>
#include "Shape.h"
using namespace std;

class DrawManager
{
    public:
        DrawManager(HWND* _windowHandle, Shape* _shape);
        ~DrawManager();

        void SetShape(Shape* _shape);
        void SetStartPoint(const Point &p);
        void SetEndPoint(const Point &p);
        void SetFillFlag(const bool &fillFlag);
        void SetDrawAlg(const int &t);
        void SetOutColor(const int &rgbCurrent);
        void SetInColor(const int &rgbCurrent);
        Shape* GetShape();
        int GetOutColor();
        int GetInColor();
        bool GetFillFlag();

        void DrawReady();
        void Draw();
    private:
        Shape* shape;

        HWND* windowHandle;
        HDC hdc;
        HDC memoHdc;
        HDC bufferHdc;
        HBITMAP  memoMap;
        HBITMAP  bufferMap;
        RECT windowRect;
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

DrawManager::DrawManager(HWND* _windowHandle, Shape* _shape){
    windowHandle = _windowHandle;
    shape = _shape;
}

DrawManager::~DrawManager(){
    windowHandle = NULL;
    delete shape;
}

void DrawManager::SetShape(Shape* _shape){
    cout<<"DrawManager::SetShape"<<endl;
    _shape->outColorRGB = shape->outColorRGB;
    _shape->inColorRGB = shape->inColorRGB;
    delete shape;
    shape = _shape;
}

void DrawManager::SetStartPoint(const Point &p){
    cout<<"DrawManager::SetStartPoint"<<endl;
    shape->startPoint = p;
}

void DrawManager::SetEndPoint(const Point &p){
    cout<<"DrawManager::SetEndPoint"<<endl;
    shape->endPoint = p;
}

void DrawManager::SetFillFlag(const bool &_fillFlag){
    cout<<"DrawManager::SetFillFlag"<<endl;
    shape->fillFlag = _fillFlag;
}

void DrawManager::SetDrawAlg(const int &t){
    cout<<"DrawManager::SetDrawAlg"<<endl;
    shape->drawAlg = t;
}

void DrawManager::SetOutColor(const int &rgbCurrent){
    cout<<"DrawManager::SetOutColor"<<endl;
    shape->outColorRGB = rgbCurrent;
}
void DrawManager::SetInColor(const int &rgbCurrent){
    cout<<"DrawManager::SetInColor"<<endl;
    shape->inColorRGB = rgbCurrent;
}

Shape* DrawManager::GetShape(){
    cout<<"DrawManager::GetShape"<<endl;
    return shape;
}

int DrawManager::GetOutColor(){
    cout<<"DrawManager::GetOutColor"<<endl;
    return shape->outColorRGB;
}

int DrawManager::GetInColor(){
    cout<<"DrawManager::GetInColor"<<endl;
    return shape->inColorRGB;
}

bool DrawManager::GetFillFlag(){
    cout<<"DrawManager::GetFillFlag"<<endl;
    return shape->fillFlag;
}

void DrawManager::DrawReady(){
    cout<<"DrawManager::DrawReady"<<endl;
    DeleteObject(memoMap);
    DeleteDC(memoHdc);

    GetClientRect(*windowHandle, &windowRect);
    int left = windowRect.left;
    int top = windowRect.top;
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;

    hdc = GetDC(*windowHandle);
    memoHdc = CreateCompatibleDC(hdc);
    memoMap = CreateCompatibleBitmap(hdc,width,height);
    SelectObject(memoHdc, memoMap);
    BitBlt(memoHdc,left,top,width,height,hdc,left,top,SRCCOPY);
    ReleaseDC(*windowHandle,hdc);
}

void DrawManager::Draw(){
    cout<<"DrawManager::Draw"<<endl;
    int left = windowRect.left;
    int top = windowRect.top;
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;

    hdc = GetDC(*windowHandle);
    bufferHdc = CreateCompatibleDC(hdc);
    bufferMap = CreateCompatibleBitmap(hdc,width,height);
    SelectObject(bufferHdc, bufferMap);
    BitBlt(bufferHdc,left,top,width,height,memoHdc,left,top,SRCCOPY);
    shape->Draw(bufferHdc);
    BitBlt(hdc,left,top,width,height,bufferHdc,left,top,SRCCOPY);
    DeleteObject(bufferMap);
    DeleteDC(bufferHdc);
    ReleaseDC(*windowHandle,hdc);
}

#endif // FRAMEMANAGER_H
