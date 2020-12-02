/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020  Dorian Zedler <dorian@itsblue.de>
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

#ifndef LINBOSTARTACTIONS_H
#define LINBOSTARTACTIONS_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QList>
#include <QLabel>

#include "linbobackend.h"
#include "linbologger.h"
#include "linboosselectionrow.h"
#include "qmodernstackedwidget.h"
#include "qmodernpushbutton.h"
#include "qmodernprogressbar.h"

class LinboStartActions : public QWidget
{
    Q_OBJECT
public:
    explicit LinboStartActions(LinboBackend* backend, QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    LinboBackend* backend;

    QModernStackedWidget* stackView;

    QWidget* buttonWidget;
    QModernPushButton* startOsButton;
    QModernPushButton* syncOsButton;
    QModernPushButton* reinstallOsButton;
    QList<QModernPushButton*> actionButtons;
    QLabel* noBaseImageLabel;
    QFont noBaseImageLabelFont;

    QWidget* progressBarWidget;
    QModernProgressBar* progressBar;
    QFont logFont;
    QLabel* logLabel;
    QModernPushButton* cancelButton;

    QWidget* errorWidget;
    QVBoxLayout* errorLayout;
    QLabel* errorLabel;
    QFont errorLabelFont;
    QLabel* errorDetailsLabel;
    QFont errorDetailsFont;
    QModernPushButton* resetErrorButton;

    bool inited;

private slots:
    void resizeAndPositionAllItems();
    void handleCurrentOsChanged(LinboOs* newOs);
    void handleLinboStateChanged(LinboBackend::LinboState newState);
    void handleLatestLogChanged(const LinboLogger::LinboLog& latestLog);
    void handleAutostartTimeoutProgressChanged();

signals:
    void selectedOsChanged();

};

#endif // LINBOSTARTACTIONS_H