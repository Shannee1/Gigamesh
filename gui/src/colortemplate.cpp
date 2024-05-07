//
// Created by timo.homburg on 06.05.2024.
//

#include "colortemplate.h"

ColorTemplate::ColorTemplate(QString templatename,QColor fillcolor, QColor bordercolor) {
    this->templatename=templatename;
    this->fillcolor=fillcolor;
    this->bordercolor=bordercolor;
    this->annovalueToColor=QMap<QString,QMap<QString,QColor>>();
}

QString ColorTemplate::getTemplateName(){
    return this->templatename;
}

QString ColorTemplate::getTemplateDescription(){
    return this->templatedescription;
}

QColor ColorTemplate::getFillColor(){
    return this->fillcolor;
}
QColor ColorTemplate::getBorderColor(){
    return this->bordercolor;
}

void ColorTemplate::setBorderColor(QColor bordercolor){
    this->bordercolor=bordercolor;
}

void ColorTemplate::setFillColor(QColor fillcolor){
    this->fillcolor=fillcolor;
}


bool ColorTemplate::addAnnoValueColor(QColor color,QString property,QString value){
    if(!(this->annovalueToColor.contains(property))){
        this->annovalueToColor.insert(property,QMap<QString,QColor>());
    }
    this->annovalueToColor.find(property)->insert(value,color);
}

bool ColorTemplate::removeAnnoValueColor(QColor color,QString property,QString value){
    if((this->annovalueToColor.contains(property)) && this->annovalueToColor.find(property)->contains(value)){
        this->annovalueToColor.find(property)->remove(value);
    }
}

bool ColorTemplate::removeAllAnnoValueColors(){
    this->annovalueToColor.clear();
}