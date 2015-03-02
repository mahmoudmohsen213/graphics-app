#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <iostream>
#include <windows.h>
#include <list>
#include "Shape.h"
using namespace std;

class HistoryManager
{
    public:
        HistoryManager(HWND* _windowHandle);
        ~HistoryManager();

        void UndoAction();
        void RedoAction();
        void AddAction(Shape* s);
        void Restore();
        void Clear();
        void DeleteHistory();
        void Reset();
    private:
        list<Shape*> memo;
        list<Shape*>::iterator currentIterator;
        int currentIndex;

        int screenWidth;
        int screenHeight;

        HWND* windowHandle;
        HDC hdc;
        HDC defaultHdc;
        HDC orgHdc;
        HDC bufferHdc;
        HBITMAP  defaultMap;
        HBITMAP  orgMap;
        HBITMAP  bufferMap;
        RECT windowRect;
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

HistoryManager::HistoryManager(HWND* _windowHandle){
    windowHandle=_windowHandle;
    currentIndex=-1; currentIterator = memo.begin(); currentIterator--;

    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &windowRect);
    int left = 0;
    int top = 0;
    screenWidth = windowRect.right - windowRect.left;
    screenHeight = windowRect.bottom - windowRect.top;

    hdc = GetDC(*windowHandle);
    orgHdc = CreateCompatibleDC(hdc);
    orgMap = CreateCompatibleBitmap(hdc,screenWidth,screenHeight);
    defaultHdc = CreateCompatibleDC(hdc);
    defaultMap = CreateCompatibleBitmap(hdc,screenWidth,screenHeight);
    SelectObject(orgHdc, orgMap);
    SelectObject(defaultHdc, defaultMap);

    for(int i=0;i<screenWidth;i++) for(int j=0;j<screenHeight;j++) SetPixel(defaultHdc,i,j,16777215);
    BitBlt(orgHdc,left,top,screenWidth,screenHeight,defaultHdc,left,top,SRCCOPY);
    BitBlt(hdc,left,top,screenWidth,screenHeight,defaultHdc,left,top,SRCCOPY);
    ReleaseDC(*windowHandle,hdc);
}

HistoryManager::~HistoryManager(){
    windowHandle = NULL;
    DeleteObject(defaultMap);
    DeleteDC(defaultHdc);
    DeleteObject(orgMap);
    DeleteDC(orgHdc);
    DeleteObject(bufferMap);
    DeleteDC(bufferHdc);

    for(currentIterator=memo.begin();currentIterator!=memo.end();currentIterator++) delete (*currentIterator);
    memo.clear();
    currentIndex=-1; currentIterator = memo.begin(); currentIterator--;
}

void HistoryManager::UndoAction(){
    cout<<"HistoryManager::UndoAction"<<endl;
    if(currentIndex == -1 ) return;

    GetClientRect(*windowHandle, &windowRect);
    int left = windowRect.left;
    int top = windowRect.top;
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;

    hdc = GetDC(*windowHandle);
    bufferHdc = CreateCompatibleDC(hdc);
    bufferMap = CreateCompatibleBitmap(hdc,width,height);
    SelectObject(bufferHdc, bufferMap);
    BitBlt(bufferHdc,left,top,width,height,orgHdc,left,top,SRCCOPY);

    list<Shape*>::iterator temp1=memo.begin();
    for(int i=0;i<currentIndex;i++,temp1++) (*temp1)->Draw(bufferHdc);
    currentIndex--; currentIterator--;

    BitBlt(hdc,left,top,width,height,bufferHdc,left,top,SRCCOPY);
    DeleteObject(bufferMap);
    DeleteDC(bufferHdc);
    ReleaseDC(*windowHandle,hdc);
}

void HistoryManager::RedoAction(){
    cout<<"HistoryManager::RedoAction"<<endl;
    if((currentIndex+1) == ((int)(memo.size()))) return;

    hdc = GetDC(*windowHandle);
    currentIndex++; currentIterator++;
    (*currentIterator)->Draw(hdc);
    ReleaseDC(*windowHandle,hdc);
}

void HistoryManager::AddAction(Shape* s){
    cout<<"HistoryManager::AddAction"<<endl;

    list<Shape*>::iterator temp1=memo.end(); temp1--;
    for(int i=(((int)(memo.size()))-1);i>currentIndex;i--) {
        delete *temp1; memo.erase(temp1); temp1=memo.end(); temp1--;
    }
    memo.push_back(s);
    currentIndex=memo.size()-1;
    currentIterator=memo.end(); currentIterator--;
}

void HistoryManager::Restore(){
    cout<<"HistoryManager::Restore"<<endl;
    if(((int)(memo.size())) == 0) return;

    hdc = GetDC(*windowHandle);
    list<Shape*>::iterator temp1=memo.begin();
    for(int i=0;i<=currentIndex;i++,temp1++) (*temp1)->Draw(hdc);
    ReleaseDC(*windowHandle,hdc);
}

void HistoryManager::Clear(){
    cout<<"HistoryManager::Clear"<<endl;
    GetClientRect(*windowHandle, &windowRect);
    int left = windowRect.left;
    int top = windowRect.top;
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;

    hdc = GetDC(*windowHandle);
    BitBlt(hdc,left,top,width,height,orgHdc,left,top,SRCCOPY);
    ReleaseDC(*windowHandle,hdc);

    currentIndex=-1; currentIterator = memo.begin(); currentIterator--;
}

void HistoryManager::DeleteHistory(){
    cout<<"HistoryManager::DeleteHistory"<<endl;

    GetClientRect(*windowHandle, &windowRect);
    int left = windowRect.left;
    int top = windowRect.top;
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;

    hdc = GetDC(*windowHandle);
    BitBlt(orgHdc,left,top,width,height,hdc,left,top,SRCCOPY);
    ReleaseDC(*windowHandle,hdc);


    if(((int)(memo.size())) == 0) return;

    for(currentIterator=memo.begin();currentIterator!=memo.end();currentIterator++)
        delete (*currentIterator);
    memo.clear();
    currentIndex=-1;
    currentIterator = memo.begin();
    currentIterator--;
}

void HistoryManager::Reset(){
    cout<<"HistoryManager::Reset"<<endl;

    BitBlt(orgHdc,0,0,screenWidth,screenHeight,defaultHdc,0,0,SRCCOPY);

    GetClientRect(*windowHandle, &windowRect);
    int left = windowRect.left;
    int top = windowRect.top;
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;

    hdc = GetDC(*windowHandle);
    BitBlt(hdc,left,top,width,height,defaultHdc,left,top,SRCCOPY);
    ReleaseDC(*windowHandle,hdc);

    if(((int)(memo.size())) == 0) return;

    for(currentIterator=memo.begin();currentIterator!=memo.end();currentIterator++)
        delete (*currentIterator);
    memo.clear();
    currentIndex=-1;
    currentIterator = memo.begin();
    currentIterator--;
}

#endif // HISTORYMANAGER_H
