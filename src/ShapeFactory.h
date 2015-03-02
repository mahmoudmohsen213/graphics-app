#ifndef SHAPEFACTORY_H
#define SHAPEFACTORY_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "Shape.h"
using namespace std;

class ShapeFactory
{
    public:
        static void RegisterShape(const unsigned int &ID, Shape* (*createShape)());
        static Shape* CreateShape(const unsigned int &ID);
        static vector<Shape*> GetShapes();

    private:
        static map<unsigned int,Shape* (*)()> shapeTable;

        ShapeFactory() {}
};

map<unsigned int,Shape* (*)()> ShapeFactory::shapeTable;

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void ShapeFactory::RegisterShape(const unsigned int &ID, Shape* (*createShape)()){
    cout<<"ShapeFactory::RegisterShape "<<ID<<endl;
    ShapeFactory::shapeTable[ID] = createShape;
    return;
}

Shape* ShapeFactory::CreateShape(const unsigned int &ID){
    cout<<"ShapeFactory::CreateShape "<<ID<<endl;
    map<unsigned int,Shape* (*)()>::iterator it = ShapeFactory::shapeTable.find(ID);
    if(it == ShapeFactory::shapeTable.end()) return NULL;
    return (it->second());
}

vector<Shape*> ShapeFactory::GetShapes(){
    cout<<"ShapeFactory::GetShapes "<<endl;
    vector<Shape*> shapes;
    map<unsigned int,Shape* (*)()>::iterator it = ShapeFactory::shapeTable.begin();
    for(;it!=ShapeFactory::shapeTable.end();it++){
        shapes.push_back(it->second());
    }
    return shapes;
}

/////////////////////////////////////////////////////////////////////

#endif // SHAPEFACTORY_H
