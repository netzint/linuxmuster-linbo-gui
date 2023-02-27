/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020-2021  Dorian Zedler <dorian@itsblue.de>
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Affero General Public License as published
 ** by the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Affero General Public License for more details.
 **
 ** You should have received a copy of the GNU Affero General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "linboradiobutton.h"

LinboRadioButton::LinboRadioButton(QString label, QWidget* parent) : QRadioButton(label, parent)
{

}

void LinboRadioButton::paintEvent(QPaintEvent *e) {
    QString indicatorSize = QString::number(this->font().pixelSize());
    this->setStyleSheet(
        gTheme->insertValues(
            QString(
                "QRadioButton {"
                "    color: %TextColor;"
                "}"
                "QRadioButton::indicator {"
                "    width: %1px;"
                "    height: %1px;"
                "}"
                "QRadioButton::indicator:unchecked {"
                "    image: url(%RadioButtonUncheckedIcon);"
                "}"
                "QRadioButton::indicator:checked {"
                "    image: url(%RadioButtonCheckedIcon);"
                "}"
                "QRadioButton::indicator:disabled {"
                "    image: url(%RadioButtonDisabledIcon);"
                "}"
            )
            .arg(indicatorSize)));

    QRadioButton::paintEvent(e);
}
