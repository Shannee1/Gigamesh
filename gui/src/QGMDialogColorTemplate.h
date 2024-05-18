//
// Created by timo.homburg on 06.05.2024.
//

#ifndef GIGAMESH_QGMDIALOGCOLORTEMPLATE_H
#define GIGAMESH_QGMDIALOGCOLORTEMPLATE_H


#include "colortemplate.h"

class QGMDialogColorTemplate {
    QGMDialogColorTemplate(QJsonArray colortemplates);
    std::list<ColorTemplate> colortemplates;
};


#endif //GIGAMESH_QGMDIALOGCOLORTEMPLATE_H
