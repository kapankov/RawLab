/*!
	\file render_widget.h
	\brief Header file with RenderWidget class (inherited from QOpenGLWidget class)
	\details The RenderWidget class allows you to display a loaded image using OpenGL.
	Features:
		1. Zoom - zoom in / out
		Zoom In
			Shortcut - Ctrl + +
			Rotate the mouse wheel forward with the Alt key held down
		Zoom Out
			Shortcut - Ctrl + -
			Rotate the mouse wheel back with the Alt key held down
		100% zoom
			Shortcut - Ctrl + 1
			Or double click on the image in window fit mode
		Zoom fit to window
			Shortcut - Ctrl + 0
			Or double click on the image when not in window fit mode
		2. Move (scroll)

	\authors Konstantin A. Pankov, explorus@mail.ru
	\copyright Copyright 2017-2019 Konstantin A. Pankov. All rights reserved.
	\license This project is released under the LGPL v.3
	\version 0.1
	\date 02/04/2019
	\warning In developing. Not a stable tested code.
	...

*/
#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <QObject>
#include <QtWidgets/QOpenGLWidget>

// задавать настройками 
// режим интерполяции
// зум-фактор
// максимум и минимум зума
// доп. смещение при скроле

class RenderWidget : public QOpenGLWidget
{
	Q_OBJECT

	friend class ImageScrollWidget;

	// private members
	const double m_dblZoomFactor = 2.0;
	const double m_MaxZoomPix = 100000.0;
	const double m_MaxZoomFactor = 20.0;

	GLuint	m_tex_id;	// OpenGL texture id
	RgbBuffPtr	m_pImgBuff;	// image buffer with sizes

	bool m_isCenter; // флаг - изображение по центру
	// координаты сдвига изображения
	// обновляются при каждом зуме, ресайзе, сдвиге и скролле (уточнить все варианты)
	// учитываются в paintGL()
	QPointF	m_ScrollOffset;

	GLdouble	m_dblZoom;	// Zoom factor, 0 - fit to window

	// scroll support
	QPoint	m_DragPoint;
	// нужен для скрола(+moving)
	// для того, что ограничивать смещение изображения в viewport
	// обновляется при зуме и ресайзе
	QPointF	m_ScrollLimit;
	// дополнительное смещение (поля) изображение при скролле
	qreal m_ExtraOffset;
	
public:
	RenderWidget(QWidget *parent = 0);
	~RenderWidget();

	bool setRgbBuff(RgbBuffPtr ptr);

signals:
	void zoomChanged(int prc);
	void scrollOffsetChanged(int xOffset, int yOffset);
	/* новый размер изображения с учетом зума */
	void scrollSizeChanged(int x, int y);
	void pointerChanged(int x, int y);

public slots:
	void onZoomIn();
	void onZoomOut();
	void onFitToWindow();
	void onZoom_25();
	void onZoom_50();
	void onZoomToNormal();
	void onZoom_200();
	void onZoom_400();
	void onZoom_800();
	void onCenter();
	void onMoveLeft();
	void onMoveRight();
	void onMoveUp();
	void onMoveDown();
protected:

	bool UpdateImage();
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;

	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseDoubleClickEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void enterEvent(QEvent* event) override;
	void leaveEvent(QEvent* event) override;

	void onZoomEvent();
	GLdouble getZoom(int width, int height) const;
	void resetCenter();
	void applyScrollLimit();
	// for parent scroll
	void setOffset(int x, int y);
	QPointF getCurrentCenter();
};

#endif