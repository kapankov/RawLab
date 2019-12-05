// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"
#include "image_scroll_widget.h"

ImageScrollWidget::ImageScrollWidget(QWidget* parent)
	: QAbstractScrollArea(parent)
	, m_lastContentSize(0,0)
	, m_lastViewportSize(0,0)
	, m_ignoreScrollContents(false)
{
	
}

void ImageScrollWidget::onScrollOffsetChanged(int xOffset, int yOffset)
{
	horizontalScrollBar()->setValue(-xOffset);
	verticalScrollBar()->setValue(-yOffset);
}

/*!
	новый размер изображения: x - width, y - height
*/
void ImageScrollWidget::onScrollSizeChanged(int x, int y)
{
	QSize areaSize = viewport()->size();
	// проверим на реальные изменения (чтобы не создавать stack overflow)
	if (m_lastContentSize.width() == x && m_lastContentSize.height() == y && areaSize == m_lastViewportSize) return;
	m_lastContentSize.setWidth(x);
	m_lastContentSize.setHeight(y);

	m_ignoreScrollContents = true;
	if (x > areaSize.width())
	{
		if (horizontalScrollBarPolicy() != Qt::ScrollBarAlwaysOn)
		{
			setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
			areaSize = viewport()->size();
		}
		horizontalScrollBar()->setPageStep(areaSize.width()/2);
		horizontalScrollBar()->setRange(0, x - areaSize.width());
		horizontalScrollBar()->setSingleStep(areaSize.width() > 100 ? areaSize.width() / 100 : 1);
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
		verticalScrollBar()->setSingleStep(areaSize.height() > 100 ? areaSize.height() / 100 : 1);
	}
	else
	{
		if (verticalScrollBarPolicy() != Qt::ScrollBarAlwaysOff)
			setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	//	viewport()->update();
	m_ignoreScrollContents = false;
	m_lastViewportSize = viewport()->size();
}

void ImageScrollWidget::scrollContentsBy(int/* dx*/, int/* dy*/)
{
	if (m_ignoreScrollContents) return;
	int x = horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOn ? horizontalScrollBar()->value() : -1;
	int y = verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOn ? verticalScrollBar()->value() : -1;

	if (RenderWidget* rw = dynamic_cast<RenderWidget*>(viewport())) 
		rw->setOffset(-x, -y);
}

void ImageScrollWidget::paintEvent(QPaintEvent* event)
{
	if (RenderWidget* rw = dynamic_cast<RenderWidget*>(viewport()))
		rw->paintEvent(event);
}

void ImageScrollWidget::resizeEvent(QResizeEvent * event)
{
	if (RenderWidget* rw = dynamic_cast<RenderWidget*>(viewport()))
		rw->resizeEvent(event);
}

void ImageScrollWidget::mousePressEvent(QMouseEvent *event)
{
	m_ignoreScrollContents = true;
	if (RenderWidget* rw = dynamic_cast<RenderWidget*>(viewport()))
		rw->mousePressEvent(event);
}

void ImageScrollWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (RenderWidget* rw = dynamic_cast<RenderWidget*>(viewport()))
		rw->mouseReleaseEvent(event);
	m_ignoreScrollContents = false;
}

void ImageScrollWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	m_ignoreScrollContents = true;
	if (RenderWidget* rw = dynamic_cast<RenderWidget*>(viewport()))
		rw->mouseDoubleClickEvent(event);
}

void ImageScrollWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (RenderWidget* rw = dynamic_cast<RenderWidget*>(viewport()))
		rw->mouseMoveEvent(event);
}

void ImageScrollWidget::wheelEvent(QWheelEvent *event)
{
	m_ignoreScrollContents = true;
	if (RenderWidget* rw = dynamic_cast<RenderWidget*>(viewport()))
		rw->wheelEvent(event);
	m_ignoreScrollContents = false;
}
