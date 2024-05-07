//
// Created by timo.homburg on 06.05.2024.
//

#ifndef GIGAMESH_COLORTEMPLATE_H
#define GIGAMESH_COLORTEMPLATE_H


#include <QColor>
#include <QMap>

class ColorTemplate {
    QColor bordercolor;
    QColor fillcolor;
    QString templatename;
    QString templatedescription;
    QMap<QString,QMap<QString,QColor>> annovalueToColor;

    ColorTemplate(QString templatename,QColor fillcolor,QColor bordercolor);
    QString getTemplateName();
    QString getTemplateDescription();
    QColor getFillColor();
    QColor getBorderColor();
    void setBorderColor(QColor bordercolor);
    void setFillColor(QColor fillcolor);
    bool addAnnoValueColor(QColor color,QString property,QString value="");
    bool removeAnnoValueColor(QColor color,QString property,QString value="");
    bool removeAllAnnoValueColors();
};


#endif //GIGAMESH_COLORTEMPLATE_H
