//
// Created by timo.homburg on 14.05.2024.
//

#ifndef GIGAMESH_QGMDIALOGCOLORANNOTATION_H
#define GIGAMESH_QGMDIALOGCOLORANNOTATION_H


#include <QtGui>
#include <QDialog>
#include "QGMMacros.h"
#include "meshwidget.h"

class QGMDialogColorAnnotation : public QDialog {
Q_OBJECT
public:
    explicit QGMDialogColorAnnotation(MeshWidget* themesh);
    QComboBox* attCBox;
    MeshWidget* themesh;
    public slots:
    void colorAnnotationsByAttribute();

};


#endif //GIGAMESH_QGMDIALOGCOLORANNOTATION_H
