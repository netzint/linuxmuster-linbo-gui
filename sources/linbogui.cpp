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

#include "linbogui.h"

LinboGui::LinboGui()
{
#ifdef TEST_ENV
    this->setFixedHeight(QGuiApplication::screens().at(0)->geometry().height() * 0.9 );
    this->setFixedWidth(QGuiApplication::screens().at(0)->geometry().height() * 1.25 * 0.9 );

    this->setFixedHeight(QGuiApplication::screens().at(0)->geometry().height() * 0.9);
    this->setFixedWidth(QGuiApplication::screens().at(0)->geometry().width() * 0.9);

    //this->setFixedHeight(QGuiApplication::screens().at(0)->geometry().height() * 0.1);
    //this->setFixedWidth(QGuiApplication::screens().at(0)->geometry().width() * 0.1);

#else
    // fill whole screen
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    this->setGeometry(QGuiApplication::screens().at(0)->geometry());
    this->setFixedHeight(QGuiApplication::screens().at(0)->geometry().height());
    this->setFixedWidth(QGuiApplication::screens().at(0)->geometry().width());
#endif

    // Load PTSans font
    QFontDatabase::addApplicationFont(":/fonts/PTSans-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/PTSans-BoldItalic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/PTSans-Italic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/PTSans-Regular.ttf");

    // Load UbuntuMono font
    QFontDatabase::addApplicationFont(":/fonts/UbuntuMono-B.ttf");
    QFontDatabase::addApplicationFont(":/fonts/UbuntuMono-BI.ttf");
    QFontDatabase::addApplicationFont(":/fonts/UbuntuMono-R.ttf");
    QFontDatabase::addApplicationFont(":/fonts/UbuntuMono-RI.ttf");

    QFont defaultFont ("PTSans");
    QApplication::setFont(defaultFont);

    // some debug logs
    qDebug() << "Display width: " << this->width() << " height: " << this->height();

    // create the backend
    this->backend = new LinboBackend(this);

    // create the theme
    this->theme = new LinboGuiTheme(this->backend, this, this);

    // set background
    this->setStyleSheet( "QMainWindow { background: " + gTheme->getColor(LinboTheme::BackgroundColor).name() + "; }"
                         "QLabel { color: " "black" "; }"
                         "QToolTip {"
                         "border: 0 0 0 0;"
                         "background: " + gTheme->getColor(LinboTheme::ElevatedBackgroundColor).name() + ";"
                         "color: " + gTheme->getColor(LinboTheme::TextColor).name() + ";"
                         "padding: " + QString::number(gTheme->getSize(LinboTheme::RowFontSize) * 0.2) + ";"
                         "font-size: " + QString::number(gTheme->getSize(LinboTheme::RowFontSize)) + "px;"
                         "}");

    // attach translator
    QString localeName = this->backend->getConfig()->locale();
    if(localeName.isEmpty() || (localeName.length() == 5 && localeName[2] == "-")) {

        if(!localeName.isEmpty()) {
            // correct case (de-de -> de-DE)
            QStringList tmpLocaleName = this->backend->getConfig()->locale().split("-");
            localeName = tmpLocaleName[0] + "-";
            localeName += tmpLocaleName[1].toUpper();
        }

        QTranslator* translator = new QTranslator(this);

        // fallback to en-GB!
        if(!translator->load(":/" + localeName + ".qm"))
            translator->load(":/en-GB.qm");

        QApplication::installTranslator(translator);
    }

    // create start page
    this->startPage = new LinboMainPage(this->backend, this);

}
