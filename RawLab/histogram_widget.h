#ifndef HISTOGRAM_WIDGET_H
#define HISTOGRAM_WIDGET_H

#include <QObject>

class HistogramWidget : public QWidget
{

	Q_OBJECT
public:
	explicit HistogramWidget(QWidget* parent = 0);
signals:

public slots:

private:
	void paintEvent(QPaintEvent* event);
};

#endif