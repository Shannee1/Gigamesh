//
// Created by timo.homburg on 01.05.2024.
//

#ifndef GIGAMESH_QGMDIALOGEXPORTANNOTATIONS_H
#define GIGAMESH_QGMDIALOGEXPORTANNOTATIONS_H

#include <QtGui>
#include <QDialog>
#include <qstring.h>
#include <iostream>
#include <list>
#include <QDialog>
#include "ui_dialogExportAnnotations.h"
#include "annotation.h"

class QGMDialogExportAnnotations : public QDialog {
Q_OBJECT
    std::list<Annotation*> annotations;
    QComboBox* annostylecbox;
    Mesh* themesh;
    QCheckBox* onlyBorderCheckBox;
public:
    QGMDialogExportAnnotations(std::list<Annotation*> annotations,Mesh* themesh,QWidget *parent = 0);
public slots:
    void exportAnnotations();

};


#endif //GIGAMESH_QGMDIALOGEXPORTANNOTATIONS_H
