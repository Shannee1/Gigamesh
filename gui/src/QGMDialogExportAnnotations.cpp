//
// Created by timo.homburg on 01.05.2024.
//

#include "QGMDialogExportAnnotations.h"
#include "QGMMacros.h"
#include <QPushButton>
#include <QGridLayout>
#include <QJsonDocument>


QGMDialogExportAnnotations::QGMDialogExportAnnotations(std::list<Annotation*> annotations,MeshQt* themesh,std::set<std::string> fieldnames,QWidget *parent){
    this->annotations=annotations;
    int linecounter=0;
    this->themesh=themesh;
    this->fieldnames=fieldnames;
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
    annostylecbox->addItem("PLY embedded in JSON (Web Annotation Data Model)");
    annostylecbox->addItem("Individual PLY Files");
    gridLayout->addWidget(annostylecbox,linecounter,1);
    linecounter+=1;
    onlyBorderCheckBox=new QCheckBox("Only Border");
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
    QString fileNameSuggestion="annoexport.json";
    QString outpath="";
    QString formatstr="";
    if(chosenformat.contains("Individual PLY Files")){
        std::string titlestring="Choose file export pattern.\n No input equals to default name.\n The following variables will be replaced:\n";
        for(const std::string& fieldname:this->fieldnames){
            titlestring+="["+fieldname+"] ";
        }
        outpath=QFileDialog::getExistingDirectory(this,tr("Choose save directory for annotations",".",QFileDialog::ShowDirsOnly))+"/";
        if(outpath==nullptr || outpath==""){
            return;
        }
        bool ok;
        formatstr=QInputDialog::getText(this,"Choose export filename pattern",QString::fromStdString(titlestring),QLineEdit::Normal);
        format="PLYFiles";
    }
    if(this->onlyBorderCheckBox->isChecked()){
        fileNameSuggestion="border_"+fileNameSuggestion;
    }else{
        fileNameSuggestion="allverts_"+fileNameSuggestion;
    }
    if(chosenformat.contains("WKTSelector")){
        format="WKTSelector";
        fileNameSuggestion="wkt_"+fileNameSuggestion;
    }else if(chosenformat.contains("SVGSelector")){
        format="SVGSelector";
        fileNameSuggestion="svg_"+fileNameSuggestion;
    }else if(chosenformat.contains("MeshID")){
        format="MeshIDSelector";
        fileNameSuggestion="meshid_"+fileNameSuggestion;
    }else if(chosenformat.contains("VertexList")){
        format="MeshVertexSelector";
        fileNameSuggestion="meshvertex_"+fileNameSuggestion;
    }else if(chosenformat.contains("PLY embedded in JSON")){
        format="PLYSelector";
    }

    QJsonArray result=QJsonArray();
    for(Annotation* curanno:this->annotations){
        QJsonObject curannojson = curanno->getAnnotation(format,this->themesh,outpath,formatstr,this->onlyBorderCheckBox->isChecked());
        result.append(curannojson);
    }
    if(!chosenformat.contains("Individual PLY Files")) {
        fileNameSuggestion = QString::fromStdString(this->themesh->getBaseName().string()) + "_" + fileNameSuggestion;
        QByteArray ba = QJsonDocument(result).toJson();
        QFileDialog::saveFileContent(ba, fileNameSuggestion);
    }
    this->close();
}