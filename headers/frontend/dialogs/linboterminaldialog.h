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

#ifndef LINBOTERMINALDIALOG_H
#define LINBOTERMINALDIALOG_H

#include <QObject>
#include <QWidget>
#include <QTextBrowser>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QProcess>
#include <QScrollBar>
#include <QKeyEvent>

#include "linbodialog.h"
#include "linbolineedit.h"
#include "linbotextbrowser.h"

class LinboTerminalDialog : public LinboDialog
{
    Q_OBJECT
public:
    LinboTerminalDialog(QWidget* parent);

protected:
    virtual void setVisibleAnimated(bool visible) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    LinboTextBrowser* textBrowser;
    QLineEdit* lineEdit;
    QVBoxLayout* mainLayout;
    QProcess* process;
    QString commandBeforeHistorySwitch;
    QStringList commandHistory;
    int currentHistoryIndex;

protected slots:
    void readOutput();
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void execute();
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // LINBOTERMINALDIALOG_H
