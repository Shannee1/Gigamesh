//
// Created by timo.homburg on 01.05.2024.
//

#ifndef GIGAMESH_QGMDIALOGIMPORTANNOTATIONS_H
#define GIGAMESH_QGMDIALOGIMPORTANNOTATIONS_H
#include <QtGui>
#include <QDialog>
#include <QGridLayout>
#include <QListWidget>

#include "QGMMacros.h"
#include "annotation.h"
#include "meshwidget.h"
#include "ColorSelectorButton.h"

class QGMDialogImportAnnotations : public QDialog {//, private Ui::dialogImportAnnotations {
Q_OBJECT

public:
    QGMDialogImportAnnotations(MeshWidget* mMeshWidget,QWidget *parent = 0);
    ColorSelectorButton* annocolorButton;
    ColorSelectorButton* annobordercolorButton;
    QLineEdit* filepath;
    MeshWidget* meshwidget;
    QDoubleSpinBox* thicknessEdit;
    QComboBox* annostylecbox;
public slots:
    void chooseFile();
    void importAnnotations();

};

#endif //GIGAMESH_QGMDIALOGIMPORTANNOTATIONS_H
