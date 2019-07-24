#ifndef IMGSCROLL_WIDGET_H
#define IMGSCROLL_WIDGET_H

#include <QAbstractScrollArea>
#include <QObject>

class ImageScrollWidget : public QAbstractScrollArea
{
	Q_OBJECT
	QSize m_lastContentSize;
	QSize m_lastViewportSize;

	bool m_ignoreScrollContents;
public:
	explicit ImageScrollWidget(QWidget* parent = 0);
signals:
//	void onPaint(QPaintEvent* event);
//	void sizeChanged(QResizeEvent * event);
public slots:
	void onScrollOffsetChanged(int xOffset, int yOffset);
	void onScrollSizeChanged(int x, int y);
private:
	void scrollContentsBy(int dx, int dy) override;
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent * event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseDoubleClickEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
};

#endif
