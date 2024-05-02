//
// Created by timo.homburg on 01.05.2024.
//

#ifndef GIGAMESH_COLORSELECTORBUTTON_H
#define GIGAMESH_COLORSELECTORBUTTON_H


#include <QColor>
#include <QWidget>
#include <QArgument>
#include <QPushButton>

class ColorSelectorButton : public QPushButton {
    Q_OBJECT
public:
    ColorSelectorButton( QWidget* parent );

    void setColor( const QColor& color );
    const QColor& getColor() const;

public slots:
            void updateColor();
    void changeColor();

private:
    QColor color;
};


#endif //GIGAMESH_COLORSELECTORBUTTON_H
