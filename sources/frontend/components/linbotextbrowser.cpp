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

#include "linbotextbrowser.h"

LinboTextBrowser::LinboTextBrowser(QWidget* parent) : QTextBrowser(parent)
{
    this->setStyleSheet(
        gTheme->insertValues(
            "QTextBrowser {"
            "    border: 0 0 0 0;"
            "    border-bottom: 1px solid %LineColor;"
            "    background: %ElevatedBackgroundColor;"
            "    padding-left: 5px;"
            "    color: %TextColor;"
            "}"
            "QTextBrowser:focus {"
            "    border-bottom: 1px solid %AccentColor;"
            "}"
        ));

    this->verticalScrollBar()->setStyleSheet(
        gTheme->insertValues(
            "QScrollBar:vertical {"
            "    background: %ElevatedBackgroundColor;"
            "    width: 10px;    "
            "    margin: 0px 0px 0px 0px;"
            "}"
            "QScrollBar::handle:vertical {"
            "    background: %TextColor;"
            "    min-height: 10px;"
            "}"
            "QScrollBar::add-line:vertical {"
            "    height: 0px;"
            "}"
            "QScrollBar::sub-line:vertical {"
            "    height: 0 px;"
            "}"
        ));

    this->horizontalScrollBar()->setStyleSheet(
        gTheme->insertValues(
            "QScrollBar:horizontal {"
            "    background: %ElevatedBackgroundColor;"
            "    height: 10px;    "
            "    margin: 0px 0px 0px 0px;"
            "}"
            "QScrollBar::handle:horizontal {"
            "    background: %TextColor;"
            "    min-width: 10px;"
            "}"
            "QScrollBar::add-line:horizontal {"
            "    width: 0px;"
            "}"
            "QScrollBar::sub-line:horizontal {"
            "    width: 0 px;"
            "}"
        ));
}
