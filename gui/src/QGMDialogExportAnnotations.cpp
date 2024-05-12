//
// Created by timo.homburg on 01.05.2024.
//

#include "QGMDialogExportAnnotations.h"
#include "QGMMacros.h"
#include <QPushButton>
#include <QGridLayout>
#include <QJsonDocument>


QGMDialogExportAnnotations::QGMDialogExportAnnotations(std::list<Annotation> annotations,QWidget *parent){
    this->annotations=annotations;
    int linecounter=0;
    this->setWindowTitle("Export Annotations");
    auto * gridLayout = new QGridLayout(this);
    auto* annostylecboxLabel=new QLabel();
    annostylecboxLabel->setText("Export Format:");
    gridLayout->addWidget(annostylecboxLabel,linecounter,0);
    annostylecbox=new QComboBox();
    annostylecbox->addItem("WKTSelector (Web Annotation Data Model)");
    annostylecbox->addItem("SVGSelector (Web Annotation Data Model)");
    annostylecbox->addItem("VertexList Selector (Web Annotation Data Model)");
    annostylecbox->addItem("MeshID Selector (Web Annotation Data Model)");
    gridLayout->addWidget(annostylecbox,linecounter,1);
    linecounter+=1;
    auto* onlyBorderCheckBox=new QCheckBox("Only Border");
    gridLayout->addWidget(onlyBorderCheckBox,linecounter,1);
    linecounter+=1;
    auto * okbutton=new QPushButton(this);
    okbutton->setText("Ok");
    gridLayout->addWidget(okbutton,linecounter,1);
    auto * cancelbutton=new QPushButton(this);
    cancelbutton->setText("Cancel");
    gridLayout->addWidget(cancelbutton,linecounter,0);
    connect(okbutton,SIGNAL(clicked()),
            this,SLOT(exportAnnotations()));
    connect(cancelbutton,SIGNAL(clicked()),
            this,SLOT(this->close()));
    this->setLayout(gridLayout);
    setWindowIcon( QIcon( _GIGAMESH_LOGO_ ) );
    this->show();
}

void QGMDialogExportAnnotations::exportAnnotations(){
    std::string format="WKTSelector";
    QString chosenformat=annostylecbox->currentText();
    std::cout << "Chosen Format: " << chosenformat.toStdString() ;
    if(chosenformat.contains("WKTSelector")){
        format="WKTSelector";
    }else if(chosenformat.contains("SVGSelector")){
        format="SVGSelector";
    }else if(chosenformat.contains("MeshID")){
        format="MeshIDSelector";
    }else if(chosenformat.contains("VertexList")){
        format="MeshVertexSelector";
    }
    QJsonArray result=QJsonArray();
    for(Annotation curanno:this->annotations){
        QJsonObject curannojson = curanno.getAnnotation(format);
        result.append(curannojson);
    }
    QByteArray ba = QJsonDocument(result).toJson();
    QFileDialog::saveFileContent(ba,"annoexport.json");
    this->close();
}