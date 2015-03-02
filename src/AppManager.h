#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <windows.h>
#include <fstream>
#include <vector>
#include <string>
#include "HistoryManager.h"
#include "DrawManager.h"
#include "ShapeFactory.h"
#include "Shape.h"
#include "resource.h"
using namespace std;

class AppManager
{
    public:
        AppManager() { appInitialized = false; }
        ~AppManager();

        void InitializeComponents(HWND* _windowHandle);
        void WM_COMMAND_MessageHandler(const WPARAM &wParam, const LPARAM &lParam);
        void WM_LBUTTONDOWN_MessageHandler(const WPARAM &wParam, const LPARAM &lParam);
        void WM_MOUSEMOVE_MessageHandler(const WPARAM &wParam, const LPARAM &lParam);
        void WM_LBUTTONUP_MessageHandler(const WPARAM &wParam, const LPARAM &lParam);
        void WM_SIZE_MessageHandler(const WPARAM &wParam, const LPARAM &lParam);
    private:
        bool appInitialized;

        HMENU menubarHandle;
        HMENU fileMenuHandle;
        HMENU editMenuHandle;
        HMENU drawMenuHandle;
        HMENU algMenuHandle;
        HMENU colorMenuHandle;
        int algMenuSize;

        HWND* windowHandle;
        HistoryManager* historyManager;
        DrawManager* drawManager;

        int DisplayColorPaletteDialog(int rgbCurrent);
        string DisplaySaveFileDialog();
        string DisplayOpenFileDialog();
        void SaveFile();
        void LoadFile();
        void SetOutColor();
        void SetInColor();
        void SetFillFlag();
        void SetShape(int temp);
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

AppManager::~AppManager(){
    windowHandle = NULL;
    delete drawManager;
    delete historyManager;
    appInitialized = false;
}

void AppManager::InitializeComponents(HWND* _windowHandle){
    cout<<"AppManager::InitializeComponents"<<endl;

    windowHandle = _windowHandle;

    menubarHandle = CreateMenu();
    fileMenuHandle = CreateMenu();
    editMenuHandle = CreateMenu();
    colorMenuHandle = CreateMenu();
    drawMenuHandle = CreateMenu();
    algMenuHandle = CreateMenu();

    AppendMenu(menubarHandle, MF_POPUP, (UINT_PTR)fileMenuHandle,  "File");
    AppendMenu(menubarHandle, MF_POPUP, (UINT_PTR)editMenuHandle,  "Edit");
    AppendMenu(menubarHandle, MF_POPUP, (UINT_PTR)drawMenuHandle,  "Draw");
    AppendMenu(menubarHandle, MF_POPUP, (UINT_PTR)algMenuHandle,   "Alg");
    AppendMenu(menubarHandle, MF_POPUP, (UINT_PTR)colorMenuHandle, "Color");

    AppendMenu(fileMenuHandle, MF_STRING, ID_New,  "New");
    AppendMenu(fileMenuHandle, MF_STRING, ID_Save, "Save");
    AppendMenu(fileMenuHandle, MF_STRING, ID_Open, "Open");
    AppendMenu(fileMenuHandle, MF_STRING, ID_Exit, "Exit");

    AppendMenu(editMenuHandle, MF_STRING, ID_Undo,  "Undo");
    AppendMenu(editMenuHandle, MF_STRING, ID_Redo,  "Redo");
    AppendMenu(editMenuHandle, MF_STRING, ID_Clear, "Clear");

    AppendMenu(colorMenuHandle, MF_STRING, ID_OutColor, "Outline");
    AppendMenu(colorMenuHandle, MF_STRING, ID_InColor,  "Fill");
    AppendMenu(colorMenuHandle, MF_STRING, ID_FillToggle, "Toggle Filling");

    vector<Shape*> shapes = ShapeFactory::GetShapes();
    int siz = (int)(shapes.size());
    for(int i=0;i<siz;i++){
        AppendMenu(drawMenuHandle, MF_STRING, shapes[i]->GetID(), shapes[i]->GetName().c_str());
    }

    vector<string> methodsName = shapes[0]->GetMethodsName();
    siz = (int)(methodsName.size()); algMenuSize = siz;
    for(int i=0;i<siz;i++){
        AppendMenu(algMenuHandle, MF_STRING, i+1, methodsName[i].c_str());
    }

    SetMenu(*windowHandle,menubarHandle);

    historyManager = new HistoryManager(windowHandle);
    drawManager = new DrawManager(windowHandle, shapes[0]);
    siz = (int)(shapes.size());
    for(int i=1;i<siz;i++){
        delete shapes[i];
    }

    appInitialized = true;
}

void AppManager::WM_COMMAND_MessageHandler(const WPARAM &wParam, const LPARAM &lParam){
    if(!appInitialized) return;
    cout<<"AppManager::WM_COMMAND_MessageHandler  "<<LOWORD(wParam)<<endl;
    int temp = LOWORD(wParam);
    if(temp == ID_Exit) exit(0);
    else if(temp == ID_New) historyManager->Reset();
    else if(temp == ID_Save) SaveFile();
    else if(temp == ID_Open) LoadFile();
    else if(temp == ID_Undo) historyManager->UndoAction();
    else if(temp == ID_Redo) historyManager->RedoAction();
    else if(temp == ID_Clear) historyManager->Clear();
    else if(temp == ID_OutColor) SetOutColor();
    else if(temp == ID_InColor) SetInColor();
    else if(temp == ID_FillToggle) SetFillFlag();
    else if((1 <= temp)&&(temp <= algMenuSize)) drawManager->SetDrawAlg(temp-1);
    else SetShape(temp);
}

void AppManager::WM_LBUTTONDOWN_MessageHandler(const WPARAM &wParam, const LPARAM &lParam){
    if(!appInitialized) return;
    cout<<"AppManager::WM_LBUTTONDOWN_MessageHandler"<<endl;
    drawManager->SetStartPoint(Point(LOWORD(lParam),HIWORD(lParam)));
    drawManager->SetEndPoint(Point(LOWORD(lParam),HIWORD(lParam)));
    drawManager->DrawReady();
}

void AppManager::WM_MOUSEMOVE_MessageHandler(const WPARAM &wParam, const LPARAM &lParam){
    if(!appInitialized) return;
    if(wParam == MK_LBUTTON){
        cout<<"AppManager::WM_MOUSEMOVE_MessageHandler"<<endl;
        drawManager->SetEndPoint(Point(LOWORD(lParam),HIWORD(lParam)));
        drawManager->Draw();
    }
}

void AppManager::WM_LBUTTONUP_MessageHandler(const WPARAM &wParam, const LPARAM &lParam){
    if(!appInitialized) return;
    cout<<"AppManager::WM_LBUTTONUP_MessageHandler"<<endl;

    Shape* currentShape = drawManager->GetShape();
    Shape* clonedShape = currentShape->Clone();
    historyManager->AddAction(clonedShape);
}

void AppManager::WM_SIZE_MessageHandler(const WPARAM &wParam, const LPARAM &lParam){
    if(!appInitialized) return;
    cout<<"AppManager::WM_SIZE_MessageHandler"<<endl;
    historyManager->Restore();
}

/////////////////////////////////////////////////////////////////////

string AppManager::DisplaySaveFileDialog(){
    if(!appInitialized) return NULL;
    cout<<"AppManager::DisplaySaveFileDialog"<<endl;

    OPENFILENAME saveFileDialog;    // common dialog box structure
    char szFile[260];               // buffer for file name
    string fileName = "";

    // Initialize OPENFILENAME
    ZeroMemory(&saveFileDialog, sizeof(saveFileDialog));
    saveFileDialog.lStructSize     = sizeof(saveFileDialog);
    saveFileDialog.hwndOwner       = *windowHandle;
    saveFileDialog.lpstrTitle      = "Save";
    saveFileDialog.lpstrFile       = szFile;
    saveFileDialog.lpstrFile[0]    = '\0';
    saveFileDialog.nMaxFile        = sizeof(szFile);
    saveFileDialog.lpstrFilter     = "All\0*.*\0Text\0*.TXT\0";
    saveFileDialog.nFilterIndex    = 1;
    saveFileDialog.lpstrFileTitle  = NULL;
    saveFileDialog.nMaxFileTitle   = 0;
    saveFileDialog.lpstrInitialDir = NULL;
    saveFileDialog.Flags = false;

    // Display the Open dialog box.
    if(GetOpenFileName(&saveFileDialog)==TRUE) fileName = saveFileDialog.lpstrFile;
    return fileName;
}

string AppManager::DisplayOpenFileDialog(){
    if(!appInitialized) return NULL;
    cout<<"AppManager::DisplayOpenFileDialog"<<endl;

    OPENFILENAME openFileDialog;    // common dialog box structure
    char szFile[260];               // buffer for file name
    string fileName = "";

    // Initialize OPENFILENAME
    ZeroMemory(&openFileDialog, sizeof(openFileDialog));
    openFileDialog.lStructSize     = sizeof(openFileDialog);
    openFileDialog.hwndOwner       = *windowHandle;
    openFileDialog.lpstrFile       = szFile;
    openFileDialog.lpstrFile[0]    = '\0';
    openFileDialog.nMaxFile        = sizeof(szFile);
    openFileDialog.lpstrFilter     = "All\0*.*\0Text\0*.TXT\0";
    openFileDialog.nFilterIndex    = 1;
    openFileDialog.lpstrFileTitle  = NULL;
    openFileDialog.nMaxFileTitle   = 0;
    openFileDialog.lpstrInitialDir = NULL;
    openFileDialog.Flags = (OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST);

    // Display the Open dialog box.
    if(GetOpenFileName(&openFileDialog)==TRUE) fileName = openFileDialog.lpstrFile;
    return fileName;
}

int AppManager::DisplayColorPaletteDialog(int rgbCurrent){
    if(!appInitialized) return 0;
    cout<<"AppManager::DisplayColorPalette"<<endl;

    CHOOSECOLOR colorPaletteDialog; // common dialog box structure
    static COLORREF acrCustClr[16]; // array of custom colors

    ZeroMemory(&colorPaletteDialog, sizeof(colorPaletteDialog));
    colorPaletteDialog.lStructSize  = sizeof(colorPaletteDialog);
    colorPaletteDialog.hwndOwner    = *windowHandle;
    colorPaletteDialog.lpCustColors = (LPDWORD) acrCustClr;
    colorPaletteDialog.rgbResult    = rgbCurrent;
    colorPaletteDialog.Flags        = (CC_FULLOPEN | CC_RGBINIT);

    if(ChooseColor(&colorPaletteDialog)==TRUE) rgbCurrent = ((int)(colorPaletteDialog.rgbResult));
    return rgbCurrent;
}

void AppManager::SaveFile(){
    if(!appInitialized) return;
    cout<<"AppManager::SaveFile"<<endl;

    string fileName = DisplaySaveFileDialog();
    if(fileName=="") return;
    int index = 0; char* buffer = new char[8388608]; // 8 Mega Bytes

    RECT windowRect; GetClientRect(*windowHandle, &windowRect);
    int left = windowRect.left;
    int top = windowRect.top;
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    int pixel;

    memcpy(buffer,((const char*)(&left)),sizeof(left)); index+=sizeof(left);
    memcpy(buffer+index,((const char*)(&top)),sizeof(top)); index+=sizeof(top);
    memcpy(buffer+index,((const char*)(&width)),sizeof(width)); index+=sizeof(width);
    memcpy(buffer+index,((const char*)(&height)),sizeof(height)); index+=sizeof(height);

    HDC hdc = GetDC(*windowHandle);
    HDC memoHdc = CreateCompatibleDC(hdc);
    HBITMAP memoMap = CreateCompatibleBitmap(hdc,width,height);
    SelectObject(memoHdc, memoMap);
    BitBlt(memoHdc,left,top,width,height,hdc,left,top,SRCCOPY);
    ReleaseDC(*windowHandle,hdc);

    for(int i=0;i<width;i++){
        for(int j=0;j<height;j++){
            pixel=((int)(GetPixel(memoHdc,i,j)));
            memcpy(buffer+index,((const char*)(&pixel)),sizeof(pixel));
            index+=sizeof(pixel);
        }
    }

    ofstream outFile(fileName.c_str(),ios::binary);
    outFile.write(((const char*)(&index)),sizeof(index));
    outFile.write(buffer,index);
    outFile.close();
    delete[] buffer;
}

void AppManager::LoadFile(){
    if(!appInitialized) return;
    cout<<"AppManager::LoadFile"<<endl;

    string fileName = DisplayOpenFileDialog();
    if(fileName=="") return;

    int index = 0, siz; char* buffer = new char[8388608]; // 8 Mega Bytes
    ifstream inFile(fileName.c_str(),ios::binary);
    inFile.read(((char*)(&siz)),sizeof(siz));
    inFile.read(buffer,siz);
    inFile.close();

    int left, top, width, height, pixel;
    memcpy(((char*)(&left)),buffer,sizeof(left)); index+=sizeof(left);
    memcpy(((char*)(&top)),buffer+index,sizeof(top)); index+=sizeof(top);
    memcpy(((char*)(&width)),buffer+index,sizeof(width)); index+=sizeof(width);
    memcpy(((char*)(&height)),buffer+index,sizeof(height)); index+=sizeof(height);
    HDC hdc = GetDC(*windowHandle);
    for(int i=0;i<width;i++){
        for(int j=0;j<height;j++){
            memcpy(((char*)(&pixel)),buffer+index,sizeof(pixel)); index+=sizeof(pixel);
            SetPixel(hdc,i,j,pixel);
        }
    }

    delete[] buffer;
    ReleaseDC(*windowHandle,hdc);
    historyManager->DeleteHistory();
}

void AppManager::SetOutColor(){
    if(!appInitialized) return;
    cout<<"AppManager::SetOutColor"<<endl;

    int rgbCurrent = drawManager->GetOutColor();
    rgbCurrent = DisplayColorPaletteDialog(rgbCurrent);
    drawManager->SetOutColor(rgbCurrent);
}

void AppManager::SetInColor(){
    if(!appInitialized) return;
    cout<<"AppManager::SetInColor"<<endl;

    int rgbCurrent = drawManager->GetInColor();
    rgbCurrent = DisplayColorPaletteDialog(rgbCurrent);
    drawManager->SetInColor(rgbCurrent);
}

void AppManager::SetFillFlag(){
    if(!appInitialized) return;
    cout<<"AppManager::SetFillFlag"<<endl;

    bool fillFlag = !(drawManager->GetFillFlag());
    drawManager->SetFillFlag(fillFlag);
    CheckMenuItem(colorMenuHandle,ID_FillToggle,(MF_BYCOMMAND|((fillFlag)?(MF_CHECKED):(MF_UNCHECKED))));
}

void AppManager::SetShape(int temp){
    if(!appInitialized) return;
    cout<<"AppManager::SetShape"<<endl;
    for(int i=0;i<algMenuSize;i++){
        RemoveMenu(algMenuHandle, 0, MF_BYPOSITION);
    }

    Shape* shapeObject = ShapeFactory::CreateShape((unsigned int)temp);
    vector<string> methodsName = shapeObject->GetMethodsName();
    algMenuSize = (int)(methodsName.size());
    for(int i=0;i<algMenuSize;i++){
        AppendMenu(algMenuHandle, MF_STRING, i+1, methodsName[i].c_str());
    }

    drawManager->SetShape(shapeObject);
}
#endif // APPMANAGER_H
