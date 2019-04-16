// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"
#include "histogram_widget.h"

HistogramWidget::HistogramWidget(QWidget* parent)
{

}

void HistogramWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect r = rect();
	r.adjust(1, 1, -1, -1);	// non-client imitation
	painter.fillRect(r, QColor::fromRgb(0x88, 0x88, 0x88));
	r.adjust(1, 1, -1, -1);
	painter.fillRect(r, QColor::fromRgb(0x60, 0x60, 0x60));
}
