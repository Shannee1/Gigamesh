//
// Created by timo.homburg on 06.05.2024.
//

#ifndef GIGAMESH_QGMCOLORTEMPLATEDIALOG_H
#define GIGAMESH_QGMCOLORTEMPLATEDIALOG_H


#include "colortemplate.h"

class QGMColorTemplateDialog {
    QGMColorTemplateDialog(QJsonArray colortemplates);
    std::list<ColorTemplate> colortemplates;
};


#endif //GIGAMESH_QGMCOLORTEMPLATEDIALOG_H
