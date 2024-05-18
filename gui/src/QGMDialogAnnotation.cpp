//
// Created by timo on 2/12/23.
//

#include "QGMDialogAnnotation.h"
#include "CompleterDelegate.h"
#include "annotation.h"
#include <iostream>
#include <ostream>
#include <QUrl>
#include <QFileDialog>
#include <QtSvg/QSvgWidget>
#include <QVBoxLayout>
#include <QtWidgets>
#include <QDialog>


QGMDialogAnnotation::QGMDialogAnnotation(QJsonObject annotemplate, Annotation* annodata, QWidget *parent) : QDialog(parent) {
    if(annotemplate.empty()){
        QFile jsonfile;
        jsonfile.setFileName("C:/Users/timo.homburg/git/GigaMesh/cmake-build-debug/annotemplates.json");
        jsonfile.open(QIODevice::ReadOnly);
        QByteArray data = jsonfile.readAll();
        qDebug()<<QJsonDocument::fromJson(data);
        QJsonDocument annoDoc;
        annoDoc=QJsonDocument::fromJson(data);
        annotemplate=annoDoc.array().at(0).toObject();
    }
    auto * gridLayout = new QGridLayout(this);
    int linecounter=0;
    auto tablabels=new QHash<QString,int>();
    auto * titlelabel=new QLabel(this);
    QString firstkey=QString::fromStdString(annodata->annotationid);
    curanno=annodata->annotationbody;
    qDebug()<<curanno << endl;
    std::cout << "Annotationbody Array Length: " << curanno.size() << endl;
    titlelabel->setText("Edit Annotation "+firstkey);
    gridLayout->addWidget(titlelabel,0,0,1,2);
    tabw=new QTabWidget(this);
    QJsonArray thetags=annotemplate.value("tags").toArray();
    gridLayout->addWidget(tabw,1,0,1,2);
    if(annotemplate.contains("fields")){
        for(const QString& key:annotemplate.value("fields").toObject().keys()) {
            QJsonObject curobj = annotemplate.value("fields").toObject().value(key).toObject();
            if (curobj.contains("category")) {
                const QJsonArray &cats = curobj.value("category").toArray();
                for (const auto & cat : cats) {
                    QString curcat = cat.toString();
                    if (!(tablabels->contains(curcat))) {
                        auto *curwidget = new QWidget();
                        curwidget->setLayout(new QGridLayout());
                        tabw->addTab(curwidget, curcat);
                        tablabels->insert(curcat, tabw->count() - 1);
                    }
                }
            }
        }
        if(tabw->count()==0) {
            auto *curwidget = new QWidget();
            tabw->addTab(curwidget, "Annotation");
        }
        for(const QString& key:annotemplate.value("fields").toObject().keys()) {
            QJsonObject curobj = annotemplate.value("fields").toObject().value(key).toObject();
            QString inputtype=curobj.value("inputtype").toString();
            if (curobj.contains("category")) {
                const QJsonArray &cats = curobj.value("category").toArray();
                for (const auto & cat : cats) {
                    QWidget *curwidget = tabw->widget(tablabels->value(cat.toString()));
                    createInputFieldByType(inputtype, linecounter, key,
                                           dynamic_cast<QGridLayout *>(curwidget->layout()), curwidget, curobj.value("data").toObject(), curobj.contains("data"),curanno);
                }
            }else{
                QWidget *curwidget = tabw->widget(0);
                createInputFieldByType(inputtype, linecounter, key, dynamic_cast<QGridLayout *>(curwidget->layout()), curwidget, curobj.value("data").toObject(), curobj.contains("data"),curanno);
            }
            linecounter++;
        }
        for(int i=0;i<tabw->count();i++){
            QWidget *curwidget = tabw->widget(i);
            auto* gridl= dynamic_cast<QGridLayout *>(curwidget->layout());
            addCategoryIndependentFields(curwidget,gridl->rowCount(),gridl,curanno,thetags);
        }
        linecounter++;
    }
    auto * okbutton=new QPushButton(this);
    okbutton->setText("Ok");
    gridLayout->addWidget(okbutton,linecounter,1);
    auto * cancelbutton=new QPushButton(this);
    cancelbutton->setText("Cancel");
    gridLayout->addWidget(cancelbutton,linecounter,0);
    connect(okbutton,SIGNAL(clicked()),
            this,SLOT(applyChanges()));
    connect(cancelbutton,SIGNAL(clicked()),
            this,SLOT(close()));
    this->setLayout(gridLayout);
    setWindowIcon( QIcon( _GIGAMESH_LOGO_ ) );
    this->show();
}

void QGMDialogAnnotation::applyChanges(){
    std::cout << "Apply Changes" << endl;
    this->saveAnnotationJSON();
    this->close();
}

QStringList getTagsFromAnnotation(const QJsonArray& curanno){
    QStringList result;
    for(auto && i : curanno){
        QJsonObject curobj=i.toObject();
        if(curobj.contains("purpose") && curobj.value("purpose")=="tagging"){
            result.append(curobj.value("value").toString());
        }
    }
    return result;
}

QHash<QString,QString> getValuesFromAnnotation(const QJsonArray& curanno){
    QHash<QString,QString> result;
    for(auto && i : curanno){
        QJsonObject curobj=i.toObject();
        if(curobj.contains("purpose") && curobj.value("purpose")!="tagging"){
            result.insert(curobj.value("purpose").toString(),curobj.value("value").toString());
        }
    }
    return result;
}

void QGMDialogAnnotation::addTag(){
    bool ok;
    QString text=QInputDialog::getText(this,"Add Tag","Tag:",QLineEdit::Normal,QDir::home().dirName(),&ok);
    if(ok && !text.isEmpty()){
        auto *it = new QListWidgetItem(text);
        it->setFlags(it->flags()| Qt::ItemIsEditable);
        this->ledit->addItem(it);
    }
}

void QGMDialogAnnotation::addCategoryIndependentFields(QWidget* curwidget, int linecounter, QGridLayout* gridLayout, QJsonArray curanno, QJsonArray tags){
    auto * label = new QLabel(curwidget);
    label->setText("Comment");
    auto * pedit=new QPlainTextEdit(curwidget);
    gridLayout->addWidget(label,linecounter,0);
    gridLayout->addWidget(pedit,linecounter,1);
    linecounter++;
    auto * label2 = new QLabel(curwidget);
    label2->setText("Tags:");
    ledit=new QListWidget(curwidget);
    QStringList assignedTags=getTagsFromAnnotation(curanno);
    if(!assignedTags.isEmpty()){
        for(const QString& tag:assignedTags){
            auto *it = new QListWidgetItem(tag);
            it->setFlags(it->flags()| Qt::ItemIsEditable);
            ledit->addItem(it);
            auto *delegate = new CompleterDelegate(curwidget,tags);
            ledit->setItemDelegate(delegate);
            ledit->setEditTriggers(QAbstractItemView::AllEditTriggers);
        }
    }else{
        auto *it = new QListWidgetItem("");
        it->setFlags(it->flags()| Qt::ItemIsEditable);
        ledit->addItem(it);
        auto *delegate = new CompleterDelegate(curwidget,tags);
        ledit->setItemDelegate(delegate);
        ledit->setEditTriggers(QAbstractItemView::AllEditTriggers);
    }
    auto * addTag=new QPushButton(curwidget);
    addTag->setText("Add Tag");
    connect(addTag, SIGNAL(clicked()), this, SLOT(addTag()));
    gridLayout->addWidget(label2,linecounter,0);
    gridLayout->addWidget(ledit,linecounter,1);
    gridLayout->addWidget(addTag,linecounter,2);
    linecounter++;
    auto * semtaglabel = new QLabel(curwidget);
    semtaglabel->setText("Semantic Tags:");
    auto * semtagedit=new QListWidget(curwidget);
    auto *it = new QListWidgetItem("");
    it->setFlags(it->flags()| Qt::ItemIsEditable);
    semtagedit->addItem(it);
    auto *delegate = new CompleterDelegate(curwidget,tags);
    semtagedit->setItemDelegate(delegate);
    semtagedit->setEditTriggers(QAbstractItemView::AllEditTriggers);
    auto * addSemTag=new QPushButton(curwidget);
    addSemTag->setText("Add Semantic Tag");
    gridLayout->addWidget(semtaglabel,linecounter,0);
    gridLayout->addWidget(semtagedit,linecounter,1);
    gridLayout->addWidget(addSemTag,linecounter,2);
}

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

QJsonObject QGMDialogAnnotation::saveAnnotationJSON(){
    QJsonObject result=QJsonObject();
    result.insert("body",QJsonArray());
    QJsonArray body=result.value("body").toArray();
    std::cout << "SAVE ANNOTATION: START " << "\n";
    auto *curlayout = dynamic_cast<QGridLayout *>(this->tabw->currentWidget()->layout());
    for(int i=0;i<curlayout->rowCount();i++){
        std::cout << "ROWCOUNT: " << std::to_string(i) << "\n";
        QLabel lab= (const QLabel) curlayout->itemAtPosition(i, 0)->widget();
        std::cout << "ROW: " << lab.text().toStdString() << "\n";
        std::cout << "ROW: " << std::to_string(curlayout->itemAtPosition(i, 1)->widget()->height()) << "\n";
        if(this->inputmap.at(i)=="QLineEdit"){
            QLineEdit edit= (const QLineEdit) curlayout->itemAtPosition(i, 1)->widget();
            body.append(QJsonObject());
            body.last().toObject().insert("type","TextualBody");
            body.last().toObject().insert("purpose",lab.text().replace(":",""));
            body.last().toObject().insert("value",edit.text());
            body.last().toObject().insert("source",lab.text().replace(":",""));
        }else if(this->inputmap.at(i)=="QComboBox"){
            std::cout << "A QComboBox "<< "\n";
            QComboBox edit= (const QComboBox) curlayout->itemAtPosition(i, 1)->widget();
            body.append(QJsonObject());
            body.last().toObject().insert("type","TextualBody");
            body.last().toObject().insert("purpose",lab.text().replace(":",""));
            body.last().toObject().insert("value",edit.currentText());
            body.last().toObject().insert("source",lab.text().replace(":",""));
        }/*else if(instanceof<QListWidget>(curlayout->itemAtPosition(i, 1)->widget())){
            QListWidget edit= (const QListWidget) curlayout->itemAtPosition(i, 1)->widget();
            for(int j=0;j<edit.count();j++){
                body.append(QJsonObject());
                body.last().toObject().insert("type","TextualBody");
                body.last().toObject().insert("purpose",lab.text().replace(":",""));
                body.last().toObject().insert("value", edit.item(j)->text());
                body.last().toObject().insert("source",lab.text().replace(":",""));
            }
        }*/
    }
    std::cout << "SAVE ANNOTATION: "<<std::to_string(body.count()) << endl;
    this->curanno=body;
    return result;
}

void QGMDialogAnnotation::createInputFieldByType(const QString& inputtype, int linecounter, const QString& key, QGridLayout* gridLayout, QWidget* curwidget, const QJsonObject& data, bool hasdata, QJsonArray curanno){
    QRegExp numberregex("\b[0-9]+\b");
    numberregex.setCaseSensitivity(Qt::CaseInsensitive);
    numberregex.setPatternSyntax(QRegExp::RegExp);
    QHash<QString,QString> valmap=getValuesFromAnnotation(curanno);
    if(inputtype=="text" || inputtype=="number"){
        auto * label = new QLabel(curwidget);
        label->setText(key+":");
        QRegExpValidator regValidator( numberregex, 0 );
        this->inputmap.append("QLineEdit");
        auto * edit =new QLineEdit(curwidget);
        if(valmap.contains(key)){
            edit->setText(valmap.value(key));
        }else{
            edit->setText("");
        }
        if(inputtype=="number"){
            edit->setValidator( &regValidator );
        }
        gridLayout->addWidget(label,linecounter,0);
        gridLayout->addWidget(edit,linecounter,1);
    }else if(inputtype=="select" && hasdata){
        auto * label = new QLabel(curwidget);
        label->setText(key+":");
        auto * cbox=new QComboBox(curwidget);
        this->inputmap.append("QComboBox");
        int selectedindex=-1;
        QString selectedval=*new QString("");
        if(valmap.contains(key)){
            selectedval=valmap.value(key);
        }
        auto dkeys=data.keys();
        dkeys.sort();
        int i=0;
        for(const QString& dkey:dkeys){
            cbox->addItem(dkey);
            if(dkey==selectedval){
                selectedindex=i;
            }
            i++;
        }
        if(selectedindex!=-1){
            cbox->setCurrentIndex(selectedindex);
        }
        gridLayout->addWidget(label,linecounter,0);
        gridLayout->addWidget(cbox,linecounter,1);
    }else if(inputtype=="paleocodage"){
        auto * label = new QLabel(curwidget);
        label->setText(key+":");
        this->inputmap.append("QLineEdit");
        QRegExpValidator regValidator( numberregex, 0 );
        auto * edit =new QLineEdit(curwidget);
        if(valmap.contains(key)){
            edit->setText(valmap.value(key));
        }else{
            edit->setText("");
        }
        if(inputtype=="number"){
            edit->setValidator( &regValidator );
        }
        gridLayout->addWidget(label,linecounter,0);
        gridLayout->addWidget(edit,linecounter,1);
        /*auto svgw=new QSvgWidget(this);
        gridLayout->addWidget(svgw,linecounter,2);*/
    }
}