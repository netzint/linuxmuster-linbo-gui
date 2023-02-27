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

#include "linboprogressbar.h"

LinboProgressBar::LinboProgressBar(QWidget* parent) : QProgressBar(parent)
{
    this->_indeterminateAnimtion = new QPropertyAnimation(this, "value");
    this->_indeterminateAnimtion->setDuration(2000);
    this->_indeterminateAnimtion->setStartValue(0);
    this->_indeterminateAnimtion->setEndValue(1000);
    this->_indeterminateAnimtion->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    this->_indeterminateAnimtion->setLoopCount(-1);

    this->setValue(0);
}

void LinboProgressBar::setIndeterminate(bool indeterminate) {
    if(this->_indeterminate == indeterminate)
        return;

    this->_indeterminate = indeterminate;

    if(this->_indeterminate) {
        this->_setIndeterminate();
    }
    else {
        this->_setDeterminate();
    }
}

void LinboProgressBar::_setIndeterminate() {
    this->_preIndeterminateValue = this->value();
    this->_preIndeterminateMinimum = this->minimum();
    this->_preIndeterminateMaximum = this->maximum();
    // finer steps, so the Animation is fluid
    this->setMinimum(0);
    this->setMaximum(1000);
    this->_indeterminateAnimtion->start();
}

void LinboProgressBar::_setDeterminate() {
    // reset minimum and maximum
    this->setMinimum(this->_preIndeterminateMinimum);
    this->setMaximum(this->_preIndeterminateMaximum);
    this->setValue(this->_preIndeterminateValue);
    this->_indeterminateAnimtion->stop();
}

void LinboProgressBar::setReversed(bool reversed) {
    if(this->_reversed == reversed)
        return;

    this->_reversed = reversed;
    this->update();
}

bool LinboProgressBar::indeterminate() {
    return this->_indeterminate;
}

void LinboProgressBar::paintEvent(QPaintEvent *e) {
    _FromTo values = this->_calculateFromTo();

    if(this->_reversed) {
        values = this->_reverseFromTo(values);
    }

    this->_paint(e, values);
    QWidget::paintEvent(e);
}

LinboProgressBar::_FromTo LinboProgressBar::_calculateFromTo() {
    if(this->_indeterminate) {
        return this->_calculateFromToIndeterminate();
    }
    else {
        return this->_calculateFromToDeterminate();
    }
}

LinboProgressBar::_FromTo LinboProgressBar::_calculateFromToIndeterminate() {
    _FromTo result;
    int maximum = this->maximum() / 2;
    if(this->value() <= maximum)
        // for the first half -> fill from left
        result.to = double(double(this->value()) / double(maximum));
    else {
        // for the second half -> empty from right
        result.from = double(double(this->value()- (maximum)) / double(maximum));
        result.to = 1;
    }
    return result;
}

LinboProgressBar::_FromTo LinboProgressBar::_calculateFromToDeterminate() {
    _FromTo result;
    result.to = double(double(this->value()) / double(this->maximum()));
    result.from = 0.0;
    return result;
}

LinboProgressBar::_FromTo LinboProgressBar::_reverseFromTo(_FromTo values) {
    double tmp = 1 - values.from;
    values.from = 1 - values.to;
    values.to = tmp;
    return values;
}

void LinboProgressBar::_paint(QPaintEvent* e, _FromTo values) {
    QPainter painter;
    painter.begin(this);
    // background
    painter.fillRect(e->rect(), gTheme->color(LinboTheme::ElevatedBackgroundColor));
    // progress
    painter.fillRect(
        QRect(
            e->rect().width() * values.from,
            0,
            e->rect().width() * values.to,
            e->rect().height()
        ),
        gTheme->color(LinboTheme::AccentColor)
    );

    painter.end();
}
