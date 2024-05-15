//
// Created by timo.homburg on 14.05.2024.
//

#include "QGMColorAnnotationDialog.h"
#include "annotation.h"
#include <QtWidgets>

QGMColorAnnotationDialog::QGMColorAnnotationDialog(MeshWidget* themesh) {
    this->themesh=themesh;
    std::set<std::string> fieldnames=themesh->getCommonAnnotationFieldNames();
    auto * gridLayout = new QGridLayout(this);
    int linecounter=0;
    auto * titlelabel=new QLabel(this);
    titlelabel->setText("Color Annotations By Attribute ");
    this->setWindowTitle("Color Annotations By Attribute");
    auto * chooseattlabel=new QLabel(this);
    titlelabel->setText("Choose Attribute:");
    gridLayout->addWidget(chooseattlabel,0,0,1,1);
    attCBox=new QComboBox();
    for(const std::string& item:fieldnames){
        attCBox->addItem(QString::fromStdString(item));
    }
    gridLayout->addWidget(attCBox,0,1,1,1);
    auto* okButton=new QPushButton();
    okButton->setText("OK");
    auto* cancelButton=new QPushButton();
    cancelButton->setText("Cancel");
    connect(okButton,SIGNAL(clicked()),
            this,SLOT(colorAnnotationsByAttribute()));
    connect(cancelButton,SIGNAL(clicked()),
            this,SLOT(close()));
    gridLayout->addWidget(okButton,1,0,1,1);
    gridLayout->addWidget(cancelButton,1,1,1,1);
}

void QGMColorAnnotationDialog::colorAnnotationsByAttribute(){
   QString attribute=this->attCBox->currentText();
   this->themesh->colorAnnotationsByAttribute(attribute);
   this->close();
}