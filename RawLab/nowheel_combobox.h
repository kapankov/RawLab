#ifndef NOWHEEL_COMBOBOX_H
#define NOWHEEL_COMBOBOX_H

#include <QObject>

class NoWheelComboBox : public QComboBox {

    Q_OBJECT

public:

    NoWheelComboBox(QWidget* parent = 0);

protected:

    virtual void wheelEvent(QWheelEvent* event);
};

#endif
