//
// Created by timo.homburg on 01.05.2024.
//


#include <QWidget>
#include <QColorDialog>
#include "QGMDialogImportAnnotations.h"
#include "ColorSelectorButton.h"

QGMDialogImportAnnotations::QGMDialogImportAnnotations(MeshWidget* mMeshWidget,QWidget *parent){
    this->meshwidget=mMeshWidget;
    int linecounter=0;
    this->setWindowTitle("Import Annotations");
    auto * gridLayout = new QGridLayout(this);
    auto* annoctemplatecboxLabel=new QLabel();
    annoctemplatecboxLabel->setText("Color Template:");
    gridLayout->addWidget(annoctemplatecboxLabel,linecounter,0);
    this->annostyletemplatecbox=new QComboBox();
    gridLayout->addWidget(annostyletemplatecbox,linecounter,1);
    linecounter+=1;
    auto* annostylecboxLabel=new QLabel();
    annostylecboxLabel->setText("Annotation Style:");
    gridLayout->addWidget(annostylecboxLabel,linecounter,0);
    this->annostylecbox=new QComboBox();
    annostylecbox->addItem("Border + Filling");
    annostylecbox->addItem("Border Only");
    annostylecbox->addItem("Filling Only");
    gridLayout->addWidget(annostylecbox,linecounter,1);
    linecounter+=1;
    auto* annocolorLabel=new QLabel();
    annocolorLabel->setText("Annotation Color:");
    gridLayout->addWidget(annocolorLabel,linecounter,0);
    annocolorButton=new ColorSelectorButton(this);
    annocolorButton->setColor(QColor().blue());
    gridLayout->addWidget(annocolorButton,linecounter,1);
    linecounter+=1;
    auto* annobordercolorLabel=new QLabel();
    annobordercolorLabel->setText("Annotation Border Color:");
    gridLayout->addWidget(annobordercolorLabel,linecounter,0);
    annobordercolorButton=new ColorSelectorButton(this);
    annobordercolorButton->setColor(QColor().black());
    gridLayout->addWidget(annobordercolorButton,linecounter,1);
    linecounter+=1;
    auto* borderThicknessLabel=new QLabel();
    borderThicknessLabel->setText("Annotation Border Thickness:");
    gridLayout->addWidget(borderThicknessLabel,linecounter,0);
    thicknessEdit=new QDoubleSpinBox();
    thicknessEdit->setSingleStep(0.1);
    thicknessEdit->setMinimum(0.0);
    thicknessEdit->setValue(0.5);
    gridLayout->addWidget(thicknessEdit,linecounter,1);
    linecounter+=1;
    auto* selectFileLabel=new QLabel();
    selectFileLabel->setText("Select Annotation File:");
    gridLayout->addWidget(selectFileLabel,linecounter,0);
    filepath=new QLineEdit();
    gridLayout->addWidget(filepath,linecounter,1);
    auto* searchFile=new QPushButton("...");
    connect(searchFile,SIGNAL(clicked()),
            this,SLOT(chooseFile()));
    gridLayout->addWidget(searchFile,linecounter,2);
    linecounter+=1;
    auto * okbutton=new QPushButton(this);
    okbutton->setText("Ok");
    gridLayout->addWidget(okbutton,linecounter,1);
    auto * cancelbutton=new QPushButton(this);
    cancelbutton->setText("Cancel");
    gridLayout->addWidget(cancelbutton,linecounter,0);
    connect(okbutton,SIGNAL(clicked()),
            this,SLOT(importAnnotations()));
    connect(cancelbutton,SIGNAL(clicked()),
            this,SLOT(close()));
    this->setLayout(gridLayout);
    setWindowIcon( QIcon( _GIGAMESH_LOGO_ ) );
    this->show();
}

void QGMDialogImportAnnotations::importAnnotations(){
    QString filename = this->filepath->text();
    this->close();
    int side=0;
    QString sidestr="noside";
    std::cout << filename.toStdString() << endl;
    if (!filename.isNull()) {
        if (filename.contains("front")) {
            side = 3;
            sidestr="front";
        } else if (filename.contains("back")) {
            side = 6;
            sidestr="back";
        } else if (filename.contains("bottom")) {
            side = 5;
            sidestr="bottom";
        } else if (filename.contains("top")) {
            side = 1;
            sidestr="top";
        } else if (filename.contains("left")) {
            side = 2;
            sidestr="left";
        } else if (filename.contains("right")) {
            side = 4;
            sidestr="right";
        }
        QFile jsonfile;
        jsonfile.setFileName(filename);
        qDebug() << filename;
        jsonfile.open(QIODevice::ReadOnly);
        QByteArray data = jsonfile.readAll();
        //qDebug()<<QJsonDocument::fromJson(data);
        QJsonDocument annoDoc;
        annoDoc = QJsonDocument::fromJson(data);
        QJsonObject mainObject = annoDoc.object();
        QStringList thekeys = mainObject.keys();
        std::list<Annotation*> annotationlist;
        std::cout << "Getting border parameters" << endl;
        bool borderOnly=false;
        bool border=true;
        if(this->annostylecbox->currentText().contains("Border Only")){
            borderOnly=true;
        }else if(this->annostylecbox->currentText().contains("Filling Only")){
            borderOnly=false;
        }else if(this->annostylecbox->currentText().contains("Border + Filling")){
            border=true;
            borderOnly=false;
        }
        std::cout << "Accessing meshwidget" << endl;
        if (!meshwidget->getMesh()->annotationsLoaded) {
            meshwidget->getMesh()->labelVerticesNone();
            meshwidget->getMesh()->labelVerticesBackground();
        }
        QColor annocolor=annocolorButton->getColor();
        QColor annobordercolor=annobordercolorButton->getColor();
        std::cout << "Creating annos" << endl;
        for (int i = 0; i < mainObject.size(); ++i) {//mainObject.size();
            QJsonObject curannojson = mainObject.find(thekeys.at(i))->toObject();
            QString annotype = curannojson.find("target")->toObject().find("selector")->toObject().find(
                    "type")->toString();
            std::cout << "Creating first anno" << endl;
            Annotation* curanno = new Annotation(curannojson, thekeys.at(i), meshwidget->getMesh(), sidestr);
            std::cout << "CurAnno created " << endl;
            if (annotype == "WKTSelector" || annotype == "WktSelector") {
                meshwidget->getMesh()->labelVerticesInBBOX(curanno->minX, curanno->maxX, curanno->minY, curanno->maxY,
                                                            curanno->minZ, curanno->maxZ, 2.0, border,borderOnly,annocolor,annobordercolor,thicknessEdit->value());
            } else if (annotype == "SvgSelector" || annotype == "SVGSelector") {
                meshwidget->getMesh()->labelVerticesInBBOX(curanno->minX, curanno->maxX, curanno->minY, curanno->maxY,
                                                            side, 2.0, border, borderOnly,annocolor,annobordercolor,thicknessEdit->value());
            }
            annotationlist.push_back(curanno);
            meshwidget->addAnnotation(curanno);
        }
        meshwidget->calculateRelativeAnnotationPositions();
        meshwidget->getMesh()->annotationsLoaded = true;
        std::cout << annotationlist.size() << endl;
        jsonfile.close();
    }

}


void QGMDialogImportAnnotations::chooseFile()
{
    QString directory = QFileDialog::getOpenFileName(this,
                                                          tr("Find Files"), QDir::currentPath());

    if (!directory.isEmpty()) {
        this->filepath->setText(directory);
    }
}