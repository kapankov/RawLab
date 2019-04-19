// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"
#include "render_widget.h"

RenderWidget::RenderWidget(QWidget * parent)
	: QOpenGLWidget(parent)
	, m_tex_id(0)
	, m_dblZoom(.0)
{
	resetCenter();
}

RenderWidget::~RenderWidget()
{

}

void RenderWidget::SetImageJpegFile(QString filename)
{
	QFile file(filename);
	file.open(QFile::ReadOnly);
	if (uchar *memdata = file.map(0, file.size()))
	{
		m_pImgBuff = std::move(GetBufFromJpeg(memdata, file.size(), true));
		file.unmap(memdata);
	}
	file.close();

	if (m_pImgBuff)
	{
		if (m_tex_id) glDeleteTextures(1, &m_tex_id);
		glGenTextures(1, &m_tex_id);
		glBindTexture(GL_TEXTURE_2D, m_tex_id);
		//	GLenum err_code = glGetError();
		// glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // по умолчанию выравнивание 32-битное
		glTexImage2D(
			GL_TEXTURE_2D, 0,
			GL_RGB8, m_pImgBuff->m_width, m_pImgBuff->m_height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, m_pImgBuff->m_buff);
		//glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// GL_LINEAR_MIPMAP_LINEAR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// GL_LINEAR

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		m_dblZoom = .0;
		resetCenter();
		onZoomEvent();

		update();

		setMouseTracking(true);
	}
}

void RenderWidget::initializeGL()
{
	glClearColor(0.4, 0.4, 0.4, 1);	// задает цвет фона
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHTING);
//	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//	glEnable(GL_COLOR_MATERIAL);
}

void RenderWidget::paintGL()
{
	if (!m_tex_id || !m_pImgBuff)
		return;

	QRectF rctImg(-static_cast<qreal>(m_pImgBuff->m_width) + 0.5,	// left
		static_cast<qreal>(m_pImgBuff->m_height) - 0.5,	// top
		static_cast<qreal>(m_pImgBuff->m_width*2),	// width
		-static_cast<qreal>(m_pImgBuff->m_height*2));	// height

	GLdouble dblZoom = m_dblZoom ? m_dblZoom : getZoom(width(), height());

	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_tex_id);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();

	if (m_ScrollOffset.x() || m_ScrollOffset.y()) glTranslated(m_ScrollOffset.x(), m_ScrollOffset.y(), 0.0);

	if (dblZoom != 1.0) glScaled(dblZoom, dblZoom, 1.0);

	glBegin(GL_QUADS);
		glTexCoord2d(0, 0);	// bottom-left
		glVertex3d(rctImg.left(), rctImg.bottom(), .0f);
		glTexCoord2d(1, 0);	// bottom-right
		glVertex3d(rctImg.right(), rctImg.bottom(), .0f);
		glTexCoord2d(1, 1);	// top-right
		glVertex3d(rctImg.right(), rctImg.top(), .0f);
		glTexCoord2d(0, 1);	// top-left
		glVertex3d(rctImg.left(), rctImg.top(), .0f);
	glEnd();

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void RenderWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-static_cast<GLdouble>(width) + 0.5,	// left
		static_cast<GLdouble>(width) - 0.5,	// right
		-static_cast<GLdouble>(height) + 0.5,	// bottom
		static_cast<GLdouble>(height) - 0.5,	// top
		1.0, -1.0);

	onZoomEvent();
}

void RenderWidget::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_DragPoint = event->pos();
	}
}

void RenderWidget::mouseReleaseEvent(QMouseEvent * event)
{
//	QMessageBox::information(this, "Mouse event", "mouseReleaseEvent", QMessageBox::Ok);
}

void RenderWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (m_dblZoom) onFitToWindow();
		else onZoomToNormal();
	}
}

void RenderWidget::mouseMoveEvent(QMouseEvent * event)
{
	if (event->buttons() & Qt::LeftButton && m_pImgBuff)
	{
		QPoint ptMousePos = event->pos();

		if (int iX = (ptMousePos.x() - m_DragPoint.x()))
			m_ScrollOffset.setX(m_ScrollOffset.x() + 2 * iX);

		if (int iY = (ptMousePos.y() - m_DragPoint.y()))
			m_ScrollOffset.setY(m_ScrollOffset.y() - 2.0 * iY);

		m_DragPoint = ptMousePos;

		update();
	}
}

void RenderWidget::wheelEvent(QWheelEvent * event)
{
	Qt::KeyboardModifiers x = event->modifiers();
	if (x & Qt::AltModifier)
	{
		QPoint numPixels = event->pixelDelta();
		QPoint numDegrees = event->angleDelta() / 8;

		int iSteps = 0;
		if (!numPixels.isNull())
			iSteps = numPixels.x()? numPixels.x(): numPixels.y();
		else if (!numDegrees.isNull())
			iSteps = (numDegrees.x()? numDegrees.x(): numDegrees.y()) / 15;

		if (iSteps > 0)
			while (iSteps)
			{
				onZoomIn();
				iSteps--;
			}

		if (iSteps < 0)
			while (iSteps)
			{
				onZoomOut();
				iSteps++;
			}

		event->accept();
	}
}

void RenderWidget::onZoomEvent()
{
	static_cast<RawLab*>(QWidget::window())->SetZoomStatus(getZoom(width(), height()) * 100);
}

GLdouble RenderWidget::getZoom(int width, int height) const
{
	if (m_pImgBuff)
	{
		if (!m_dblZoom)
		{
			GLdouble zoom = std::max(static_cast<GLdouble>(m_pImgBuff->m_width) / width, static_cast<GLdouble>(m_pImgBuff->m_height) / height);
			return 1.0f / (zoom ? zoom : 1.0f);
		}
		else return m_dblZoom;
	}
	return 1.0f;
}

void RenderWidget::resetCenter()
{
	m_ScrollOffset.setX(0);
	m_ScrollOffset.setY(0);
}

void RenderWidget::onZoomIn()
{
	if (!m_pImgBuff) return;
	if (!m_dblZoom)
		m_dblZoom = getZoom(width(), height());

	if ((static_cast<int>(m_dblZoom * m_pImgBuff->m_width) < 100000 && static_cast<int>(m_dblZoom * m_pImgBuff->m_height) < 100000) && m_dblZoom < 20)
	{
		m_dblZoom *= m_dblZoomFactor;
		update();
		onZoomEvent();
	}
}

void RenderWidget::onZoomOut()
{
	if (!m_pImgBuff) return;
	if (!m_dblZoom)
		m_dblZoom = getZoom(width(), height());
	if (static_cast<int>(m_dblZoom * m_pImgBuff->m_width) > 100 && static_cast<int>(m_dblZoom * m_pImgBuff->m_height) > 100)
	{
		m_dblZoom /= m_dblZoomFactor;
		update();
		onZoomEvent();
	}
}

void RenderWidget::onFitToWindow()
{
	if (!m_pImgBuff) return;
	m_dblZoom = .0f;
	resetCenter();
	update();
	onZoomEvent();
}

void RenderWidget::onZoomToNormal()
{
	if (!m_pImgBuff) return;
	m_dblZoom = 1.0f;
	resetCenter();
	update();
	onZoomEvent();
}

void RenderWidget::onCenter()
{
	if (!m_pImgBuff) return;
	resetCenter();
	update();
}