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

#include "linboosselectbutton.h"

LinboOsSelectButton::LinboOsSelectButton(QString icon, LinboOs* os, QButtonGroup* buttonGroup, QWidget* parent) : QWidget(parent)
{
    this->showActionButtons = false;
    this->inited = false;
    this->buttonGroup = buttonGroup;
    this->os = os;
    this->shouldBeVisible = true;

    connect(os->getBackend(), &LinboBackend::stateChanged, this, &LinboOsSelectButton::handleBackendStateChange);

    if(!os->getBackend()->getConfig()->getUseMinimalLayout()) {
        QStringList startActionButtonIcons = {
            ":/svgIcons/startBg.svg",
            ":/svgIcons/syncBg.svg",
            ":/svgIcons/reinstallBg.svg",
            ":/svgIcons/infoBg.svg"
        };

        for(QString actionButtonIcon : startActionButtonIcons) {
            QModernPushButton* actionButton = new QModernPushButton(actionButtonIcon, this);
            actionButton->setGeometry(0,0,0,0);
            this->startActionButtons.append(actionButton);
        }

        connect(this->startActionButtons[0], &QModernPushButton::clicked, this->os, &LinboOs::start);
        connect(this->startActionButtons[1], &QModernPushButton::clicked, this->os, &LinboOs::sync);
        connect(this->startActionButtons[2], &QModernPushButton::clicked, this->os, &LinboOs::reinstall);

        QStringList rootActionButtons = {
            ":/svgIcons/imageBg.svg",
            ":/svgIcons/uploadBg.svg"
        };

        for(QString actionButtonIcon : rootActionButtons) {
            QModernPushButton* actionButton = new QModernPushButton(actionButtonIcon, this);
            actionButton->setGeometry(0,0,0,0);
            this->rootActionButtons.append(actionButton);
        }

        this->defaultStartActionOverlay = new QModernPushButtonOverlay (
                    QModernPushButtonOverlay::Background,
                    new QSvgWidget(":/svgIcons/overlayStart.svg"),
                    false
                    );

        this->defaultRootActionOverlay = new QModernPushButtonOverlay (
                    QModernPushButtonOverlay::Background,
                    new QSvgWidget(":/svgIcons/overlayImage.svg"),
                    false
                    );

        this->button = new QModernPushButton(icon, "", {this->defaultStartActionOverlay, this->defaultRootActionOverlay}, this);
    }
    else {
        this->button = new QModernPushButton(icon, "", this);
    }

    this->button->setCheckable(true);
    this->buttonGroup->addButton(this->button);

    QWidget::setVisible(true);
}

LinboOs* LinboOsSelectButton::getOs() {
    return this->os;
}

void LinboOsSelectButton::setVisibleAnimated(bool visible) {
    this->shouldBeVisible = visible;
    this->button->setVisibleAnimated(visible);

    this->updateActionButtonVisibility();
}

void LinboOsSelectButton::setVisible(bool visible) {
    this->shouldBeVisible = visible;
    this->button->setVisible(visible);

    for(QModernPushButton* actionButton : this->startActionButtons)
        actionButton->setVisible(this->shouldBeVisible);

    for(QModernPushButton* actionButton : this->rootActionButtons)
        actionButton->setVisible(this->shouldBeVisible);
}

void LinboOsSelectButton::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    if(!this->showActionButtons || this->os->getBackend()->getConfig()->getUseMinimalLayout()) {
        this->button->setGeometry((this->width() - this->height()) / 2 , 0, this->height(), this->height());
    }
    else {
        this->button->setGeometry(0, 0, event->size().height(), event->size().height());

        //qDebug() << "OS " << this->os->getName() << " Width: " << this->width() << " Height: " << this->height();

        int x = this->height();
        int spacing = this->width() * 0.04;
        int actionButtonSize = (this->width() - this->height()) / this->startActionButtons.length() - spacing;

        if(actionButtonSize < 0) {
            // only the big button is visible
            actionButtonSize = 0;
            x = 0;
            spacing = 0;
        }

        //qDebug() << "OS " << this->os->getName() << " X: " << x << " spacing: " << spacing << " actionButtonSize: " << actionButtonSize << " Button length: ";

        for(QModernPushButton* actionButton : this->startActionButtons) {
            actionButton->setGeometry(x + spacing, this->height() - actionButtonSize, actionButtonSize, actionButtonSize);
            //qDebug() << "Setting geometry to: " << actionButton->geometry();
            x += actionButtonSize + spacing;
        }

        x = this->height();

        for(QModernPushButton* actionButton : this->rootActionButtons) {
            actionButton->setGeometry(x + spacing, this->height() - actionButtonSize, actionButtonSize, actionButtonSize);
            x += actionButtonSize + spacing;
        }
    }

    this->updateActionButtonVisibility();
}


void LinboOsSelectButton::setShowActionButtons(bool showActionButtons) {
    if(this->showActionButtons == showActionButtons && this->inited)
        return;

    qDebug() << "OS " << this->os->getName() << " Shows action buttons: " << showActionButtons;

    this->showActionButtons = showActionButtons;
    this->button->setCheckable(!showActionButtons);

    this->updateActionButtonVisibility();
}

void LinboOsSelectButton::handleBackendStateChange(LinboBackend::LinboState state) {
    Q_UNUSED(state)
    this->updateActionButtonVisibility();
}

void LinboOsSelectButton::updateActionButtonVisibility() {
    if(this->os->getBackend()->getConfig()->getUseMinimalLayout())
        return;

    if(!this->showActionButtons) {
        for(QModernPushButton* actionButton : this->startActionButtons)
            actionButton->setVisible(false);

        for(QModernPushButton* actionButton : this->rootActionButtons)
            actionButton->setVisible(false);

        this->defaultStartActionOverlay->setVisible(false);
        this->defaultRootActionOverlay->setVisible(false);

        return;
    }

    bool startActionVisible = this->shouldBeVisible && this->os->getBackend()->getState() < LinboBackend::Root;
    bool rootActionVisible = this->shouldBeVisible && this->os->getBackend()->getState() >= LinboBackend::Root;

    for(QModernPushButton* actionButton : this->startActionButtons)
        if(this->inited)
            actionButton->setVisibleAnimated(startActionVisible);
        else
            actionButton->setVisible(this->shouldBeVisible && this->os->getBackend()->getState() < LinboBackend::Root);

    for(QModernPushButton* actionButton : this->rootActionButtons)
        if(this->inited)
            actionButton->setVisibleAnimated(rootActionVisible);
        else
            actionButton->setVisible(rootActionVisible);

    bool defaultOverlaysVisible = this->width() > this->height() * 1.2;

    if(false && this->inited) {
        this->defaultStartActionOverlay->setVisibleAnimated(startActionVisible && defaultOverlaysVisible);
        this->defaultRootActionOverlay->setVisibleAnimated(rootActionVisible && defaultOverlaysVisible);
    }
    else {
        this->defaultStartActionOverlay->setVisible(startActionVisible && defaultOverlaysVisible);
        this->defaultRootActionOverlay->setVisible(rootActionVisible && defaultOverlaysVisible);
    }

    this->inited = true;
}
