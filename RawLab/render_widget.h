#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <QObject>
#include <QtWidgets/QOpenGLWidget>

// �������� ����������� 
// ����� ������������
// ���-������
// �������� � ������� ����
// ���. �������� ��� ������

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
	// ���������� ������ �����������
	// ����������� ��� ������ ����, ������ � �������
	// ����������� � paintGL()
	QPointF	m_ScrollOffset;
	// ����� ��� ������(+moving)
	// ��� ����, ��� ������������ �������� ����������� � viewport
	QPointF	m_ScrollLimit;
	// �������������� �������� (����) ����������� ��� �������
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