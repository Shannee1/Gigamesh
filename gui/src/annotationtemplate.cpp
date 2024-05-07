//
// Created by timo.homburg on 06.05.2024.
//

#include <QFile>
#include "annotationtemplate.h"


void AnnotationTemplate::importTemplateJSON(QString filepath){
    /*QFile jsonfile;
    jsonfile.setFileName(filepath);
    jsonfile.open(QIODevice::ReadOnly);
    QByteArray data = jsonfile.readAll();
    qDebug()<<QJsonDocument::fromJson(data);
    QJsonDocument annoDoc;
    annoDoc=QJsonDocument::fromJson(data);
    annotemplate=annoDoc.array().at(0).toObject();*/
}

void AnnotationTemplate::exportTemplateJSON(QString filepath){
    /*for (int i = 0; i < templatejson.count(); i++) {
        QJsonObject curobj = templatejson.at(i).toObject();
        if (curobj.contains("name") && curobj.value("name").toString() == templatename) {
            curobj.remove("tags");
            curobj.insert("tags",QJsonArray());
            const QJsonArray &thetags = curobj.value("tags").toArray();
            for (int j = 0; j < this->annotagWidget->rowCount(); j++) {
                thetags.operator+(QJsonObject());
                thetags.at(j).toObject()["tag"]=this->annotagWidget->item(j, 0)->text();
                thetags.at(j).toObject()["uri"]=this->annotagWidget->item(j, 1)->text();
            }
            const QJsonObject &thefields = curobj.value("fields").toObject();
            for (int j = 0; j < this->annotemplateWidget->rowCount(); j++) {
                thefields[this->annotemplateWidget->item(j,0)->text()]=QJsonObject();
                QJsonObject curobject=thefields.value(this->annotemplateWidget->item(j,0)->text()).toObject();
                curobject.insert(QString("inputtype"),this->annotemplateWidget->item(j,1)->text());
                curobject.insert(QString("uri"),this->annotemplateWidget->item(j,2)->text());
                curobject.insert(QString("category"),this->annotemplateWidget->item(j,3)->text());
                curobject.insert(QString("datalist"),this->annotemplateWidget->item(j,4)->text());
            }
        }
    }
    QJsonDocument(templatejson)
     */
}