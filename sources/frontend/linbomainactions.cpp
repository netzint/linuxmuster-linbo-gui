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

#include "linbomainactions.h"

LinboMainActions::LinboMainActions(LinboBackend* backend, QWidget *parent) : QWidget(parent)
{
    this->_backend = backend;
    connect(this->_backend, &LinboBackend::stateChanged, this, &LinboMainActions::_handleLinboStateChanged);
    connect(this->_backend, &LinboBackend::timeoutProgressChanged, this, &LinboMainActions::_handleTimeoutProgressChanged);
    connect(this->_backend->logger(), &LinboLogger::latestLogChanged, this, &LinboMainActions::_handleLatestLogChanged);

    this->_stackView = new LinboStackedWidget(this);

    this->_inited = false;

    this->setStyleSheet(gTheme->insertValues("QLabel { color: %TextColor; }"));

    // Progress bar
    this->_progressBarWidget = new QWidget();
    this->_progressBar = new LinboProgressBar(this->_progressBarWidget);
    this->_progressBar->setRange(0,1000);
    this->_progressBar->setIndeterminate(true);

    this->_logLabel = new QLabel("", this->_progressBarWidget);
    this->_logLabel->setAlignment(Qt::AlignCenter);
    this->_passedTimeLabel = new QLabel("00:00", this->_progressBarWidget);
    this->_passedTimeLabel->setAlignment(Qt::AlignCenter);

    this->_passedTimeTimer = new QTimer(this->_progressBarWidget);
    connect(this->_passedTimeTimer, &QTimer::timeout, this, [=]() {
        int passedSecs = QDateTime::currentSecsSinceEpoch() - this->_processStartedAt;
        QString passedTime =
            QStringLiteral("%1").arg(passedSecs / 60, 2, 10, QLatin1Char('0'))
            + ":"
            + QStringLiteral("%1").arg(passedSecs % 60, 2, 10, QLatin1Char('0'));
        this->_passedTimeLabel->setText(passedTime);
    });
    this->_passedTimeTimer->setInterval(1000);
    this->_processStartedAt = QDateTime::currentSecsSinceEpoch();

    this->_cancelButton = new LinboToolButton(LinboTheme::CancelIcon, this->_progressBarWidget);
    connect(this->_cancelButton, &LinboToolButton::clicked, this->_backend, &LinboBackend::cancelCurrentAction);

    this->_stackView->addWidget(this->_progressBarWidget);

    // Message widget
    this->_messageWidget = new QWidget();
    this->_messageLabel = new QLabel("", this->_messageWidget);
    this->_messageLabel->setAlignment(Qt::AlignCenter);

    this->_messageDetailsTextBrowser = new LinboTextBrowser(this->_messageWidget);
    this->_messageDetailsTextBrowser->setReadOnly(true);
    this->_messageDetailsTextBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->_messageDetailsTextBrowser->setLineWrapMode(QTextEdit::NoWrap);

    this->_resetMessageButton = new LinboToolButton(LinboTheme::BackIcon, this->_messageWidget);
    connect(this->_resetMessageButton, &LinboToolButton::clicked, this->_backend, &LinboBackend::resetMessage);

    this->_stackView->addWidget(this->_messageWidget);

    // Root widget
    this->_rootWidget = new QWidget();
    this->_rootLayout = new QVBoxLayout(this->_rootWidget);
    this->_rootLayout->setAlignment(Qt::AlignCenter);
    this->_rootLayout->setContentsMargins(0,0,0,0);

    LinboPushButton* buttonCache;

    //% "Open terminal"
    buttonCache = new LinboToolButton(qtTrId("main_root_button_openTerminal"), LinboTheme::TerminalIcon, LinboTheme::TextColor);
    connect(buttonCache, &LinboPushButton::clicked, this, &LinboMainActions::terminalRequested);
    this->_rootActionButtons.append(buttonCache);

    //% "Update cache"
    buttonCache = new LinboToolButton(qtTrId("main_root_button_updateCache"), LinboTheme::SyncIcon, LinboTheme::TextColor);
    buttonCache->setVisible(this->_backend->config()->operatingSystems().length() > 0);
    this->_rootActionButtons.append(buttonCache);
    connect(buttonCache, &LinboPushButton::clicked, this, &LinboMainActions::cacheUpdateRequested);

    //% "Partition drive"
    buttonCache = new LinboToolButton(qtTrId("main_root_button_partitionDrive"), LinboTheme::PartitionIcon, LinboTheme::TextColor);
    buttonCache->setVisible(this->_backend->config()->operatingSystems().length() > 0);
    this->_rootActionButtons.append(buttonCache);
    connect(buttonCache, &LinboPushButton::clicked, this, &LinboMainActions::drivePartitioningRequested);

    //% "Register"
    buttonCache = new LinboToolButton(qtTrId("main_root_button_register"), LinboTheme::RegisterIcon, LinboTheme::TextColor);
    this->_rootActionButtons.append(buttonCache);
    connect(buttonCache, &LinboPushButton::clicked, this, &LinboMainActions::registrationRequested);

    QHBoxLayout* horizontalRootLayoutCache = nullptr;

    for(int i = 0; i < this->_rootActionButtons.length(); i++) {
        if(i % 2 == 0)
            horizontalRootLayoutCache = new QHBoxLayout();

        this->_rootLayout->addLayout(horizontalRootLayoutCache);
        horizontalRootLayoutCache->addWidget(this->_rootActionButtons[i]);
    }

    this->_rootLayout->addStretch();

    this->_stackView->addWidget(this->_rootWidget);

    // empty widget
    this->_emptyWidget = new QWidget();
    this->_stackView->addWidget(this->_emptyWidget);


    connect(this->_stackView, &LinboStackedWidget::currentChanged, this, &LinboMainActions::_resizeAndPositionAllItems);
    this->_handleLinboStateChanged(this->_backend->state());
}

void LinboMainActions::_resizeAndPositionAllItems() {

    int defaultSpacing = this->height() * 0.03;
    QFont fontCache;

    // stack view
    this->_stackView->setFixedSize(this->size());

    // Progress bar
    this->_progressBarWidget->setGeometry(0,0,this->width(), this->height());
    int progressBarHeight = this->_progressBarWidget->height() * 0.1;
    int progressBarWidth = this->_progressBarWidget->width() * 0.5;
    int logLabelHeight = progressBarHeight * 2;
    int logLabelWidth = this->_progressBarWidget->width() * 0.8;
    int cancelButtonWidth = this->_progressBarWidget->height() * 0.4;

    fontCache = this->_logLabel->font();
    fontCache.setPixelSize(gTheme->toFontSize(logLabelHeight * 0.8));
    this->_logLabel->setFont(fontCache);
    this->_logLabel->setGeometry((this->_progressBarWidget->width() - logLabelWidth) / 2, 0, logLabelWidth, logLabelHeight);

    _progressBar->setGeometry((this->_progressBarWidget->width() - progressBarWidth) / 2, this->_logLabel->y() + logLabelHeight + defaultSpacing, progressBarWidth, progressBarHeight);

    fontCache = this->_passedTimeLabel->font();
    fontCache.setPixelSize(gTheme->toFontSize(logLabelHeight * 0.8));
    this->_passedTimeLabel->setFont(fontCache);
    this->_passedTimeLabel->setGeometry((this->_progressBarWidget->width() - logLabelWidth) / 2, this->_progressBar->y() + progressBarHeight + defaultSpacing, logLabelWidth, logLabelHeight);

    this->_cancelButton->setGeometry((this->_progressBarWidget->width() - cancelButtonWidth) / 2, this->_passedTimeLabel->y() + logLabelHeight + defaultSpacing, cancelButtonWidth, cancelButtonWidth);

    // Message widget
    this->_messageWidget->setGeometry(QRect(0,0, this->width(), this->height()));

    if(this->_messageDetailsTextBrowser->isVisible()) {
        this->_messageLabel->setGeometry(0,0, this->width(), this->height() * 0.2);

        int messageDetailsFontHeight = this->height() * 0.6;
        QFont messageDetailsFont = this->_messageDetailsTextBrowser->font();
        messageDetailsFont.setPixelSize(gTheme->toFontSize(messageDetailsFontHeight / 12.5));
        this->_messageDetailsTextBrowser->setFont(messageDetailsFont);

        QFont messageFont = this->_messageLabel->font();
        messageFont.setBold(true);
        messageFont.setPixelSize(gTheme->toFontSize(messageDetailsFont.pixelSize() * 1.5));
        this->_messageLabel->setFont(messageFont);

        this->_messageDetailsTextBrowser->setFixedWidth(this->width() * 0.8);
        this->_messageDetailsTextBrowser->move((this->width() - this->_messageDetailsTextBrowser->width()) / 2, this->_messageLabel->height());
        this->_messageDetailsTextBrowser->setFixedHeight(messageDetailsFontHeight);
    }
    else {
        QFont messageFont = this->_messageLabel->font();
        messageFont.setBold(true);
        messageFont.setPixelSize(gTheme->toFontSize(this->height() * 0.1));
        this->_messageLabel->setFont(messageFont);
        this->_messageLabel->setGeometry(0, 0, this->width(), this->height());
    }

    int resetMessageButtonSize = this->height() * 0.15;

    this->_resetMessageButton->setGeometry((this->width() - resetMessageButtonSize) / 2, this->height() - resetMessageButtonSize * 1.1, resetMessageButtonSize, resetMessageButtonSize);

    // Root widget
    this->_rootWidget->setGeometry(QRect(0,0, this->width(), this->height()));

    int rootActionButtonHeight = this->height() / (this->_rootActionButtons.length() / 2) - this->height() * 0.03;
    this->_rootLayout->setSpacing(defaultSpacing);

    for(LinboPushButton* button : this->_rootActionButtons) {
        button->setFixedHeight(rootActionButtonHeight);
        button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    }

    this->_inited = true;
}

void LinboMainActions::resizeEvent(QResizeEvent *event) {
    this->_resizeAndPositionAllItems();
    QWidget::resizeEvent(event);
}

void LinboMainActions::_handleCurrentOsChanged(LinboOs* newOs) {
    Q_UNUSED(newOs)
    this->_resizeAndPositionAllItems();
}

void LinboMainActions::_handleLinboStateChanged(LinboBackend::LinboState newState) {

    QWidget* currentWidget = nullptr;
    this->_passedTimeTimer->stop();

    switch (newState) {
    case LinboBackend::Autostarting:
    case LinboBackend::RootTimeout:
        this->_progressBar->setIndeterminate(false);
        this->_progressBar->setReversed(true);
        this->_progressBar->setValue(0);
        currentWidget = this->_progressBarWidget;
        break;
    case LinboBackend::Idle:
        currentWidget = this->_emptyWidget;
        break;


    case LinboBackend::Disabled:
        this->_cancelButton->hide();
        //% "This client is controlled remotely. Please don't power it off."
        this->_logLabel->setText(qtTrId("main_message_disabled"));
    // fall through
    case LinboBackend::Starting:
    case LinboBackend::Syncing:
    case LinboBackend::CreatingImage:
    case LinboBackend::UploadingImage:
    case LinboBackend::Reinstalling:
    case LinboBackend::Partitioning:
    case LinboBackend::UpdatingCache:
        this->_passedTimeLabel->setText("00:00");
        this->_processStartedAt = QDateTime::currentSecsSinceEpoch();
        this->_passedTimeTimer->start();
        this->_progressBar->setIndeterminate(true);
        this->_progressBar->setReversed(false);
        currentWidget = this->_progressBarWidget;
        break;

    case LinboBackend::StartActionError:
    case LinboBackend::RootActionError: {
        QList<LinboLogger::LinboLog> chaperLogs = this->_backend->logger()->logsOfCurrentChapter();
        //% "The process %1 crashed:"
        this->_messageLabel->setText(qtTrId("main_message_processCrashed").arg("\"" + chaperLogs[chaperLogs.length()-1].message + "\""));
        QString errorDetails;
        if(chaperLogs.length() == 0)
            //% "No logs before this crash"
            errorDetails = "<b>" + qtTrId("main_message_noLogs") + "</b>";
        else {
            //% "The last logs before the crash were:"
            errorDetails = "<b>" + qtTrId("main_message_lastLogs") + "</b><br>";
            errorDetails += LinboLogger::logsToStacktrace(chaperLogs).join("<br>");
        }

        //% "Please ask your system administrator for help."
        errorDetails += "<br><br><b>" + qtTrId("main_message_askForHelp") + "</b>";

        this->_messageDetailsTextBrowser->setText("<html>" + errorDetails + "</html>");
        this->_messageLabel->setStyleSheet("QLabel { color : red; }");
        this->_messageDetailsTextBrowser->setVisible(true);
        currentWidget = this->_messageWidget;
        break;
    }

    case LinboBackend::RootActionSuccess: {
        QList<LinboLogger::LinboLog> chaperLogs = this->_backend->logger()->logsOfCurrentChapter();
        //% "The process %1 finished successfully."
        this->_messageLabel->setText(qtTrId("main_message_processFinishedSuccessfully").arg("\"" + chaperLogs[chaperLogs.length()-1].message + "\"" ));
        this->_messageDetailsTextBrowser->setText("");
        this->_messageLabel->setStyleSheet("QLabel { color : green; }");
        currentWidget = this->_messageWidget;
        this->_messageDetailsTextBrowser->setVisible(false);
        break;
    }

    case LinboBackend::Root:
        currentWidget = this->_rootWidget;
        break;

    default:
        break;
    }

    if(this->_inited)
        this->_stackView->setCurrentWidgetAnimated(currentWidget);
    else
        this->_stackView->setCurrentWidget(currentWidget);
}

void LinboMainActions::_handleLatestLogChanged(const LinboLogger::LinboLog& latestLog) {
    if(this->_backend->state() == LinboBackend::Idle)
        return;

    QString logColor = gTheme->color(LinboTheme::TextColor).name();

    if (latestLog.type == LinboLogger::StdErr)
        logColor = "red";

    this->_logLabel->setStyleSheet("QLabel { color : " + logColor + "; }");
    this->_logLabel->setText(latestLog.message);
}

void LinboMainActions::_handleTimeoutProgressChanged(double progress, int remaningMilliseconds) {
    if(this->_backend->state() != LinboBackend::Autostarting && this->_backend->state() != LinboBackend::RootTimeout)
        return;

    QString label = "";

    if(this->_backend->state() == LinboBackend::Autostarting) {
        //% "Starting"
        label = qtTrId("main_autostart_label") + " " + this->_backend->osOfCurrentAction()->name();
    }
    else {
        //% "Logging out automatically"
        label = qtTrId("main_rootTimeout_label");
    }

    this->_progressBar->setValue(1000 - progress * 1000);

    this->_logLabel->setText(label);

    int remaningSeconds = remaningMilliseconds / 1000;
    QString remaningTime =
        QStringLiteral("%1").arg(remaningSeconds / 60, 2, 10, QLatin1Char('0'))
        + ":"
        + QStringLiteral("%1").arg(remaningSeconds % 60, 2, 10, QLatin1Char('0'));

    this->_passedTimeLabel->setText(remaningTime);
}
