//
// Created by timo.homburg on 06.05.2024.
//

#ifndef GIGAMESH_ANNOTATIONTEMPLATE_H
#define GIGAMESH_ANNOTATIONTEMPLATE_H


#include <QString>
#include <QJsonArray>

class AnnotationTemplate {

    QJsonArray templatejson;

    void importTemplateJSON(QString filepath);
    void exportTemplateJSON(QString filepath);
    void addTextField(QString name,QString type);





};


#endif //GIGAMESH_ANNOTATIONTEMPLATE_H
