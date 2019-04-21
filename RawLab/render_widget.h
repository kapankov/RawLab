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

	// private members
	const double m_dblZoomFactor = 2.0;
	const double m_MaxZoomPix = 100000.0;

	GLuint	m_tex_id;	// OpenGL texture id
	GLdouble	m_dblZoom;	// Zoom factor, 0 - fit to window
	RgbBuffPtr	m_pImgBuff;	// image buffer with sizes

	// scroll support
	QPoint	m_DragPoint;
	// координаты сдвига изображения
	// обновляются при каждом зуме, сдвиге и скролле
	// учитываются в paintGL()
	QPointF	m_ScrollOffset;
	// нужен для скрола(+moving)
	// для того, что ограничивать смещение изображения в viewport
	QPointF	m_ScrollLimit;
	// дополнительное смещение (поля) изображение при скролле
	qreal m_ExtraOffset;
	
public:
	RenderWidget(QWidget *parent = 0);
	~RenderWidget();

	void SetImageJpegFile(QString filename);
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
protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;

	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseDoubleClickEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;

	void onZoomEvent();
	GLdouble getZoom(int width, int height) const;
	void resetCenter();
	void CorrectOffset();
};

#endif