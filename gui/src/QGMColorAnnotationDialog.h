//
// Created by timo.homburg on 14.05.2024.
//

#ifndef GIGAMESH_QGMCOLORANNOTATIONDIALOG_H
#define GIGAMESH_QGMCOLORANNOTATIONDIALOG_H


#include <QtGui>
#include <QDialog>
#include "QGMMacros.h"
#include "meshwidget.h"

class QGMColorAnnotationDialog : public QDialog {
Q_OBJECT
public:
    explicit QGMColorAnnotationDialog(MeshWidget* themesh);
    QComboBox* attCBox;
    MeshWidget* themesh;
    public slots:
    void colorAnnotationsByAttribute();

};


#endif //GIGAMESH_QGMCOLORANNOTATIONDIALOG_H
