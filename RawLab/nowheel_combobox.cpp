// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"
#include "nowheel_combobox.h"

NoWheelComboBox::NoWheelComboBox(QWidget* parent/* = 0*/) : QComboBox(parent) {
    setFocusPolicy(Qt::StrongFocus);
}

void NoWheelComboBox::wheelEvent(QWheelEvent* event) {
    if (!hasFocus()) {
        event->ignore();
    }
    else {
        QComboBox::wheelEvent(event);
    }
}