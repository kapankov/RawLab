// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"
#include "image_scroll_widget.h"

ImageScrollWidget::ImageScrollWidget(QWidget* parent)
	: QAbstractScrollArea(parent)
	, m_ignoreScrollContents(false)
{
	
}

void ImageScrollWidget::onScrollOffsetChanged(int xOffset, int yOffset)
{
	horizontalScrollBar()->setValue(-xOffset);
	verticalScrollBar()->setValue(-yOffset);
}

void ImageScrollWidget::onScrollSizeChanged(int x, int y)
{
	m_ignoreScrollContents = true;
	QSize areaSize = viewport()->size();
	if (x > areaSize.width())
	{
		if (horizontalScrollBarPolicy() != Qt::ScrollBarAlwaysOn)
		{
			setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
			areaSize = viewport()->size();
		}
		horizontalScrollBar()->setPageStep(areaSize.width()/2);
		horizontalScrollBar()->setRange(0, x - areaSize.width());
	}
	else
	{
		if (horizontalScrollBarPolicy() != Qt::ScrollBarAlwaysOff)
			setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	if (y > areaSize.height())
	{
		if (verticalScrollBarPolicy() != Qt::ScrollBarAlwaysOn)
		{
			setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
			areaSize = viewport()->size();
		}
		verticalScrollBar()->setPageStep(areaSize.height()/2);
		verticalScrollBar()->setRange(0, y - areaSize.height());
	}
	else
	{
		if (verticalScrollBarPolicy() != Qt::ScrollBarAlwaysOff)
			setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	//	viewport()->update();
	m_ignoreScrollContents = false;
}

void ImageScrollWidget::scrollContentsBy(int dx, int dy)
{
	if (m_ignoreScrollContents) return;
	int x = horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOn ? horizontalScrollBar()->value() : -1;
	int y = verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOn ? verticalScrollBar()->value() : -1;

	dynamic_cast<RenderWidget*>(viewport())->setOffset(-x, -y);
}

void ImageScrollWidget::paintEvent(QPaintEvent* event)
{
	dynamic_cast<RenderWidget*>(viewport())->paintEvent(event);
}

void ImageScrollWidget::resizeEvent(QResizeEvent * event)
{
	dynamic_cast<RenderWidget*>(viewport())->resizeEvent(event);
}

void ImageScrollWidget::mousePressEvent(QMouseEvent *event)
{
	m_ignoreScrollContents = true;
	dynamic_cast<RenderWidget*>(viewport())->mousePressEvent(event);
}

void ImageScrollWidget::mouseReleaseEvent(QMouseEvent *event)
{
	dynamic_cast<RenderWidget*>(viewport())->mouseReleaseEvent(event);
	m_ignoreScrollContents = false;
}

void ImageScrollWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	m_ignoreScrollContents = true;
	dynamic_cast<RenderWidget*>(viewport())->mouseDoubleClickEvent(event);
}

void ImageScrollWidget::mouseMoveEvent(QMouseEvent *event)
{
	dynamic_cast<RenderWidget*>(viewport())->mouseMoveEvent(event);
}

void ImageScrollWidget::wheelEvent(QWheelEvent *event)
{
	m_ignoreScrollContents = true;
	dynamic_cast<RenderWidget*>(viewport())->wheelEvent(event);
	m_ignoreScrollContents = false;
}
