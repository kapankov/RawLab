#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <QObject>
#include <QtWidgets/QOpenGLWidget>

class RenderWidget : public QOpenGLWidget
{
	Q_OBJECT

	// private members
	const GLdouble m_dblZoomFactor = 1.5f;

	GLuint	m_tex_id;	// OpenGL texture id
	GLdouble	m_dblZoom;	// Zoom factor, 0 - fit to window
	RgbBuffPtr	m_pImgBuff;	// image buffer with sizes

	// scroll support
	QPoint	m_DragPoint;
	// ���������� ������ �����������
	// ����������� ��� ������ ����, ������ � �������
	// ����������� � paintGL()
	QPointF	m_CenterCoord;

public:
	RenderWidget(QWidget *parent = 0);
	~RenderWidget();

	void SetImageJpegFile(QString filename);
public slots:
	void onZoomIn();
	void onZoomOut();
	void onFitToWindow();
	void onUnZoom();
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
};

#endif