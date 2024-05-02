//
// Created by timo.homburg on 01.05.2024.
//

#include "ColorSelectorButton.h"

#include <QColorDialog>

ColorSelectorButton::ColorSelectorButton( QWidget* parent )
        : QPushButton(parent)
{
    connect( this, SIGNAL(clicked()), this, SLOT(changeColor()) );
}

void ColorSelectorButton::updateColor()
{
    setStyleSheet( "background-color: " + color.name() );
}

void ColorSelectorButton::changeColor()
{
    QColor newColor = QColorDialog::getColor(color, parentWidget());
    if ( newColor != color )
    {
        setColor( newColor );
    }
}

void ColorSelectorButton::setColor( const QColor& color )
{
    this->color = color;
    updateColor();
}

const QColor& ColorSelectorButton::getColor() const
{
    return color;
}

