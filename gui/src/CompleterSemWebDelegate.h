//
// Created by timo on 2/13/23.
//

#ifndef GIGAMESH_COMPLETERSEMWEBDELEGATE_H
#define GIGAMESH_COMPLETERSEMWEBDELEGATE_H


#include <QStyledItemDelegate>
#include <QCompleter>
#include <QtWidgets>
#include "FileDownloader.h"

class CompleterSemWebDelegate: public QStyledItemDelegate
{
public:
    void showPopup();
    FileDownloader* downloader;
    using QStyledItemDelegate::QStyledItemDelegate;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QWidget *editor = QStyledItemDelegate::createEditor(parent, option, index);
        QUrl searchUrl("https://en.wikipedia.org/w/api.php?action=query&prop=pageprops&ppprop=wikibase_item&titles="+qobject_cast<QLineEdit *>(editor)->text()+"&formatversion=2&format=json");
        /*downloader = new FileDownloader(searchUrl);
        connect(downloader, SIGNAL (downloaded()), this, SLOT (showPopup()));
        */return editor;
    }
};


void CompleterSemWebDelegate::showPopup(){
    QByteArray result=downloader->downloadedData();
    QJsonDocument doc=QJsonDocument::fromJson(result);
    QJsonObject root=doc.object();
    QJsonArray pages=root.find("query")->toObject().find("pages")->toArray();
    QStringList wordList{};
    /*for(int i=0;i<pages.size();i++){
        if()
    }
    auto *completer = new QCompleter(wordList, le);
    //completer->setCompletionMode(QCompleter::InlineCompletion);
    le->setCompleter(completer);*/
}



#endif //GIGAMESH_COMPLETERSEMWEBDELEGATE_H
