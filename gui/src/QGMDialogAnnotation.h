//
// Created by timo on 2/12/23.
//

#include <QtGui>
#include <QDialog>
#include <QGridLayout>
#include <QListWidget>

#include "QGMMacros.h"
#include "annotation.h"

#ifndef GIGAMESH_QGMANNOTATIONDIALOG_H
#define GIGAMESH_QGMANNOTATIONDIALOG_H

class QGMDialogAnnotation : public QDialog {
    Q_OBJECT

    QTabWidget* tabw;

    QJsonArray curanno;

    QListWidget* ledit;

    QList<QString> inputmap;

public:
    //QGMDialogAnnotation(QJsonObject templatejson,QWidget *parent = nullptr);
    QGMDialogAnnotation(QJsonObject templatejson, Annotation* annodata, QWidget *parent = nullptr);
    void createInputFieldByType(const QString& inputtype,int linecounter,const QString& key,QGridLayout* gridLayout,QWidget* curwidget,const QJsonObject& data,bool hasdata,QJsonArray curanno);
    void addCategoryIndependentFields(QWidget* curwidget,int linecounter,QGridLayout* gridLayout,QJsonArray curanno,QJsonArray tags);


    signals:

public slots:
    void applyChanges();
    void addTag();
    // QWidget interface
protected:

    QJsonObject saveAnnotationJSON();
};

#endif //GIGAMESH_QGMANNOTATIONDIALOG_H