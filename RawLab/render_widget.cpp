// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"
#include "render_widget.h"

RenderWidget::RenderWidget(QWidget * parent)
	: QOpenGLWidget(parent)
	, m_tex_id(0)
	, m_isCenter(true)
	, m_dblZoom(.0)
	, m_ExtraOffset(0.0)
	, m_EmptyLabel(tr("No image"))
	, m_CmsState(false)
{
	resetCenter();
}

RenderWidget::~RenderWidget()
{
}

bool RenderWidget::setRgbBuff(RgbBuffPtr&& ptr)
{
	m_pImgBuff = std::move(ptr);
	// сообщить об изменении картинки (например, для создания гистограммы в приложении)
	emit imageChanged(m_pImgBuff.get());
	return UpdateImage();
}

void RenderWidget::setEmptyLabel(const QString& label)
{
	m_EmptyLabel = label;
}

void RenderWidget::enableCms(bool enable)
{
	m_CmsState = enable;
	if (m_CmsState)
	{
		setMonitorProfile(QString::fromStdString(GetMonitorProfile(reinterpret_cast<HWND>(winId()))));
	}
	// update something
	UpdateImage(false);
}

bool RenderWidget::isCmsEnabled() const noexcept
{
	return m_CmsState;
}

QString RenderWidget::getMonitorProfile() const
{
	return m_MonitorProfilePath;
}

bool RenderWidget::setMonitorProfile(const QString& profile)
{
	if (m_MonitorProfilePath.compare(profile))
	{
		m_MonitorProfilePath = profile;
		emit monitorProfileChanged(m_CmsState);
		::OutputDebugString((QString("Monitor profile was changed: ") + m_MonitorProfilePath + QString("\n")).toStdWString().c_str());
		return true;
	}
	return false;
}

void RenderWidget::checkProfile()
{
	if (m_CmsState && setMonitorProfile(QString::fromStdString(GetMonitorProfile(reinterpret_cast<HWND>(winId())))))
		UpdateImage(false);
}

bool RenderWidget::UpdateImage(bool resetZoom)
{
	bool result = false;
	if (m_tex_id)
	{
		glDeleteTextures(1, &m_tex_id);
		// glDeleteTextures не обнуляет id текстуры, и из-за этого можно удалить чужую текстуру
		// например, в релизе некорректно работает QPainter::drawText
		m_tex_id = 0;
	}
	if (resetZoom)
	{
		m_dblZoom = .0;
		m_isCenter = true;
	}

	setMouseTracking(false);

	if (m_pImgBuff)
	{
		if (m_pImgBuff->m_buff)
		{
			RgbBuffPtr	pCmsImgBuff;
			GLsizei width = static_cast<GLsizei>(m_pImgBuff->m_width);
			GLsizei height = static_cast<GLsizei>(m_pImgBuff->m_height);
			GLvoid* pixels = m_pImgBuff->m_buff;
			GLenum type = m_pImgBuff->m_bits == 8 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT;
			if (m_CmsState)
			{
				// создать буфер для CMS
				pCmsImgBuff = m_pImgBuff->copy();
				CmsParams* params = pCmsImgBuff->m_params.get();
				if (params && params->m_iColorSpace == 2 && params->output_profile.compare("PREVIEW")==0)
				{
					if (cmsHPROFILE hInProfile = cmsCreateAdobeRGBProfile())
					{
						TransformToMonitorColor(
							pCmsImgBuff->m_buff,
							pCmsImgBuff->m_width,
							pCmsImgBuff->m_height,
							pCmsImgBuff->m_bits,
							hInProfile,
							const_cast<char*>(m_MonitorProfilePath.isEmpty() ? nullptr : m_MonitorProfilePath.toStdString().c_str()));
						cmsCloseProfile(hInProfile);
					}
				}
				else
					TransformToMonitorColor(
						pCmsImgBuff->m_buff,
						pCmsImgBuff->m_width,
						pCmsImgBuff->m_height,
						pCmsImgBuff->m_bits,
						const_cast<char*>(m_MonitorProfilePath.isEmpty() ? nullptr : m_MonitorProfilePath.toStdString().c_str()),
						params ? params->m_iColorSpace : 1,
						params&& abs(params->gamm[0]) > DBL_EPSILON ? params->gamm : nullptr,
						params&& params->m_iColorSpace == 0 && abs(params->cam_xyz[0][0]) > FLT_EPSILON ? params->cam_xyz : nullptr,
						params&& params->m_iColorSpace == -1 ? params->output_profile.c_str() : nullptr
					);
				pixels = pCmsImgBuff->m_buff;
			}

			glGenTextures(1, &m_tex_id);
			glBindTexture(GL_TEXTURE_2D, m_tex_id);
			//	GLenum err_code = glGetError();
			// glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // по умолчанию выравнивание 32-битное
			glTexImage2D(
				GL_TEXTURE_2D, 0,
				GL_RGB8, width, height, 0,
				GL_RGB, type, pixels);
			//glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// GL_LINEAR_MIPMAP_LINEAR
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// GL_LINEAR

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			setMouseTracking(true);
		}
		// если размер изображения меньше меньше окна, не растягивать, оставить масштаб 100%
		if (resetZoom && m_pImgBuff->m_width < width() && m_pImgBuff->m_height < height())
			m_dblZoom = 1.0;
		result = true;
	}

	if (resetZoom)
		onZoomEvent();
	update();

	return result;
}

void RenderWidget::initializeGL()
{
	glClearColor(0.5, 0.5, 0.5, 1);	// задает цвет фона
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHTING);
//	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//	glEnable(GL_COLOR_MATERIAL);
}

void RenderWidget::paintGL()
{
	if (m_pImgBuff && !m_pImgBuff->m_buff)
	{
		QPainter painter(this);
		painter.setPen(qRgb(0xC0, 0xC0, 0xC0));

		QFont font = painter.font(); // enlarged the text
		font.setPixelSize(14);
		font.setBold(true);
		painter.setFont(font);

		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
		painter.drawText(rect(), Qt::AlignHCenter | Qt::AlignVCenter, m_EmptyLabel);
//		painter.end(); // It will be called by the painter's destructor implicitly

		return;
	}

	if (!m_tex_id || !m_pImgBuff)
		return;

	QRectF rctImg(0,	// left
		0,	// top
		static_cast<qreal>(m_pImgBuff->m_width),	// width
		static_cast<qreal>(m_pImgBuff->m_height));	// height

	GLdouble dblZoom = getZoom(width(), height());

	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_tex_id);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();

	if (m_ScrollOffset.x() || m_ScrollOffset.y()) 
		glTranslated(std::floor(m_ScrollOffset.x()), std::floor(m_ScrollOffset.y()), 0);

	if (fabs(dblZoom - 1.0) > DBL_EPSILON) 
		glScaled(dblZoom, dblZoom, 1.0);

	glBegin(GL_QUADS);
		glTexCoord2d(0, 0);	// bottom-left
		glVertex3d(rctImg.left(), rctImg.bottom(), 0);
		glTexCoord2d(1, 0);	// bottom-right
		glVertex3d(rctImg.right(), rctImg.bottom(), 0);
		glTexCoord2d(1, 1);	// top-right
		glVertex3d(rctImg.right(), rctImg.top(), 0);
		glTexCoord2d(0, 1);	// top-left
		glVertex3d(rctImg.left(), rctImg.top(), 0);
	glEnd();

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void RenderWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0,	// left
		static_cast<GLdouble>(width),	// right
		static_cast<GLdouble>(height),	// bottom
		0.0,	// top
		1.0, -1.0);

	if (m_isCenter) resetCenter();

	onZoomEvent();
}

void RenderWidget::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_DragPoint = event->pos();
	}
}

void RenderWidget::mouseReleaseEvent(QMouseEvent* /*event*/)
{
//	QMessageBox::information(this, "Mouse event", "mouseReleaseEvent", QMessageBox::Ok);
}

void RenderWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (fabs(m_dblZoom) > DBL_EPSILON) onFitToWindow();
		else onZoomToNormal();
	}
}

void RenderWidget::mouseMoveEvent(QMouseEvent * event)
{
	if (m_pImgBuff)
	{
		QPoint ptMousePos = event->pos();
		// скроллинг мышью
		if (event->buttons() & Qt::LeftButton && m_dblZoom)
		{
			m_isCenter = false;
			if (fabs(m_dblZoom) < DBL_EPSILON) m_dblZoom = getZoom(width(), height());

			if (int iX = (ptMousePos.x() - m_DragPoint.x()))
				m_ScrollOffset.setX(m_ScrollOffset.x() + iX);

			if (int iY = (ptMousePos.y() - m_DragPoint.y()))
				m_ScrollOffset.setY(m_ScrollOffset.y() + iY);

			applyScrollLimit();

			m_DragPoint = ptMousePos;

			emit scrollOffsetChanged(static_cast<int>(m_ScrollOffset.x()), static_cast<int>(m_ScrollOffset.y()));

			repaint();
		}
		// просигналить позицию курсора на изображении (в координатах изображения без зума)
		else onMousePosChanged(ptMousePos);
	}
}

void RenderWidget::wheelEvent(QWheelEvent * event)
{
	if (!m_pImgBuff) return;
	Qt::KeyboardModifiers x = event->modifiers();

	QPoint numPixels = event->pixelDelta();
	QPoint numDegrees = event->angleDelta() / 8;

	int iSteps = 0;
	if (!numPixels.isNull())
		iSteps = numPixels.x() ? numPixels.x() : numPixels.y();
	else if (!numDegrees.isNull())
		iSteps = (numDegrees.x() ? numDegrees.x() : numDegrees.y()) / 15;

	if (x & Qt::AltModifier)
	{
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
	else if (x & Qt::ControlModifier) // горизонтальный скролл
	{
		if (iSteps > 0)
			while (iSteps)
			{
				onMoveLeft();
				iSteps--;
			}
		if (iSteps < 0)
			while (iSteps)
			{
				onMoveRight();
				iSteps++;
			}
		event->accept();
	}
	else // вертикальный скролл
	{
		if (iSteps > 0)
			while (iSteps)
			{
				onMoveUp();
				iSteps--;
			}
		if (iSteps < 0)
			while (iSteps)
			{
				onMoveDown();
				iSteps++;
			}
		event->accept();
	}
}

void RenderWidget::enterEvent(QEvent* /*event*/)
{
	if (m_pImgBuff)
		onMousePosChanged(mapFromGlobal(QCursor::pos()));
}

void RenderWidget::leaveEvent(QEvent* /*event*/)
{
	emit pointerChanged(-1, -1);
}

void RenderWidget::onMousePosChanged(const QPoint& ptMousePos)
{
	QPointF ptCoord(ptMousePos);
	ptCoord.setX(ptCoord.x() - floor(m_ScrollOffset.x()));
	ptCoord.setY(ptCoord.y() - floor(m_ScrollOffset.y()));
	if (ptCoord.x() >= .0 && ptCoord.y() >= .0)
	{
		double dblZoom = m_dblZoom;
		if (fabs(dblZoom) < DBL_EPSILON) dblZoom = getZoom(width(), height());
		ptCoord.setX(ptCoord.x() / dblZoom);
		ptCoord.setY(ptCoord.y() / dblZoom);

		QPoint ptMouseChangedPos(ptCoord.x(), ptCoord.y());
		if (ptMouseChangedPos.x() >= m_pImgBuff->m_width || ptMouseChangedPos.y() >= m_pImgBuff->m_height)
			emit pointerChanged(-1, -1);
		else
			emit pointerChanged(ptMouseChangedPos.x(), ptMouseChangedPos.y());
	}
	else emit pointerChanged(-1, -1);
}

void RenderWidget::onZoomEvent()
{
	if (m_pImgBuff)
	{
		if (m_isCenter) resetCenter();

		// определим новое ограничение для скролла
		GLdouble zoom = getZoom(width(), height());
		qreal w = zoom * m_pImgBuff->m_width;
		qreal h = zoom * m_pImgBuff->m_height;
		// отрицательное смещение разрешает смещаться влево или вверх
		// положительное или нулевое запрещает смещение
		m_ScrollLimit.setX(
			// не использовать fabs! здесь
			((w - width()) > DBL_EPSILON ?
				static_cast<qreal>(width()) - w : 
				0.5*(static_cast<qreal>(width()) - w)
			) + m_ExtraOffset
		);
		m_ScrollLimit.setY(
			// не использовать fabs! здесь
			((h - height()) > DBL_EPSILON ?
				static_cast<qreal>(height()) - h : 
				0.5*(static_cast<qreal>(height()) - h)
			) + m_ExtraOffset
		);
		applyScrollLimit();

		emit scrollSizeChanged(static_cast<int>(w), static_cast<int>(h));
		emit scrollOffsetChanged(static_cast<int>(m_ScrollOffset.x()), static_cast<int>(m_ScrollOffset.y()));
	}
	emit zoomChanged(getZoom(width(), height()) * 100);	// signal
	//static_cast<RawLab*>(QWidget::window())->SetZoomStatus(getZoom(width(), height()) * 100);
}

GLdouble RenderWidget::getZoom(int width, int height) const
{
	if (m_pImgBuff)
	{
		if (fabs(m_dblZoom) < DBL_EPSILON) // m_dblZoom == 0
		{
			GLdouble zoom = std::max(static_cast<GLdouble>(m_pImgBuff->m_width) / width, 
				static_cast<GLdouble>(m_pImgBuff->m_height) / height);
			return 1.0f / (fabs(zoom) > DBL_EPSILON ? zoom : 1.0f);
		}
		else return m_dblZoom;
	}
	return 1.0f;
}

void RenderWidget::resetCenter()
{
	if (!m_isCenter) m_isCenter = true;
	GLdouble dblZoom = getZoom(width(), height());
	m_ScrollOffset.setX(
		m_pImgBuff ? 0.5 * (static_cast<double>(width()) - dblZoom*m_pImgBuff->m_width) : 0.0
	);
	m_ScrollOffset.setY(
		m_pImgBuff ? 0.5 * (static_cast<double>(height()) - dblZoom*m_pImgBuff->m_height) : 0.0
	);
}

void RenderWidget::applyScrollLimit()
{
	// m_ScrollLimit checking
	if (m_ScrollLimit.x() >= 0.0) m_ScrollOffset.setX(m_ScrollLimit.x());
	else if (m_ScrollOffset.x() > 0.0) m_ScrollOffset.setX(0.0);
	else if (m_ScrollOffset.x() < m_ScrollLimit.x()) m_ScrollOffset.setX(m_ScrollLimit.x());
	if (m_ScrollLimit.y() >= 0.0) m_ScrollOffset.setY(m_ScrollLimit.y());
	else if (m_ScrollOffset.y() > 0.0) m_ScrollOffset.setY(0.0);
	else if (m_ScrollOffset.y() < m_ScrollLimit.y()) m_ScrollOffset.setY(m_ScrollLimit.y());
}

void RenderWidget::setOffset(int x, int y)
{
	if (x <= 0) m_ScrollOffset.setX(x);
	if (y <= 0) m_ScrollOffset.setY(y);
	update();
}

QPointF RenderWidget::getCurrentCenter()
{
	if (!m_pImgBuff) return QPointF();

	QPointF r(0.5 * m_pImgBuff->m_width, 0.5 * m_pImgBuff->m_height);
	GLdouble zoom = getZoom(width(), height());
	qreal w = zoom * m_pImgBuff->m_width;
	qreal h = zoom * m_pImgBuff->m_height;

	if (w > width())
		r.setX((0.5 * width() - m_ScrollOffset.x()) / zoom);
	if (h > height())
		r.setY((0.5 * height() - m_ScrollOffset.y()) / zoom);

	return r;
}

int RenderWidget::getMonitorNumber()
{
	return QApplication::desktop()->screenNumber(this);;
}

void RenderWidget::onZoomIn()
{
	if (!m_pImgBuff) return;

	QPointF p = getCurrentCenter();

	if (fabs(m_dblZoom) < DBL_EPSILON) // m_dblZoom == 0
		m_dblZoom = getZoom(width(), height());

	if ((static_cast<int>(m_dblZoom * m_pImgBuff->m_width) < m_MaxZoomPix 
		&& static_cast<int>(m_dblZoom * m_pImgBuff->m_height) < m_MaxZoomPix) 
		&& m_dblZoom < m_MaxZoomFactor)
	{
		m_dblZoom *= m_dblZoomFactor;
		onZoomEvent();
		update();
	}
}

void RenderWidget::onZoomOut()
{
	if (!m_pImgBuff) return;

	QPointF p = getCurrentCenter();

	if (fabs(m_dblZoom) < DBL_EPSILON) // m_dblZoom == 0
		m_dblZoom = getZoom(width(), height());
	if (static_cast<int>(m_dblZoom * m_pImgBuff->m_width) > 100 
		&& static_cast<int>(m_dblZoom * m_pImgBuff->m_height) > 100)
	{
		m_dblZoom /= m_dblZoomFactor;
		onZoomEvent();
		update();
	}
}

void RenderWidget::onFitToWindow()
{
	if (!m_pImgBuff) return;
	m_dblZoom = .0f;
	m_isCenter = true;
	onZoomEvent();
	update();
}

void RenderWidget::onZoom_25()
{
	if (!m_pImgBuff) return;
	m_dblZoom = 0.25f;
	onZoomEvent();
	update();
}

void RenderWidget::onZoom_50()
{
	if (!m_pImgBuff) return;
	m_dblZoom = 0.50f;
	onZoomEvent();
	update();
}

void RenderWidget::onZoom_200()
{
	if (!m_pImgBuff) return;
	m_dblZoom = 2.0f;
	onZoomEvent();
	update();
}

void RenderWidget::onZoom_400()
{
	if (!m_pImgBuff) return;
	m_dblZoom = 4.0f;
	onZoomEvent();
	update();
}

void RenderWidget::onZoom_800()
{
	if (!m_pImgBuff) return;
	m_dblZoom = 8.0f;
	onZoomEvent();
	update();
}

void RenderWidget::onZoomToNormal()
{
	if (!m_pImgBuff) return;
	m_dblZoom = 1.0f;
	m_isCenter = true;
	onZoomEvent();
	update();
}

void RenderWidget::onCenter()
{
	if (!m_pImgBuff) return;
	resetCenter();
	emit scrollOffsetChanged(static_cast<int>(m_ScrollOffset.x()), static_cast<int>(m_ScrollOffset.y()));
	update();
}

void RenderWidget::onMoveLeft()
{
	if (!m_pImgBuff) return;
	int dblW = m_dblZoom * m_pImgBuff->m_width > width() ? width() : 1;
	dblW = dblW > 100 ? dblW / 100 : 1;
	if (m_ScrollOffset.x() <= 0) m_ScrollOffset.setX(m_ScrollOffset.x() + dblW);
	applyScrollLimit();
	emit scrollOffsetChanged(static_cast<int>(m_ScrollOffset.x()), static_cast<int>(m_ScrollOffset.y()));
	update();
}

void RenderWidget::onMoveRight()
{
	if (!m_pImgBuff) return;
	int dblW = m_dblZoom * m_pImgBuff->m_width > width() ? width() : 1;
	dblW = dblW > 100 ? dblW / 100 : 1;
	if (m_ScrollOffset.x() <= 0) m_ScrollOffset.setX(m_ScrollOffset.x() - dblW);
	applyScrollLimit();
	emit scrollOffsetChanged(static_cast<int>(m_ScrollOffset.x()), static_cast<int>(m_ScrollOffset.y()));
	update();
}

void RenderWidget::onMoveUp()
{
	if (!m_pImgBuff) return;
	double dblH = m_dblZoom * m_pImgBuff->m_height > height() ? height() : 1;
	dblH = dblH > 100. ? dblH / 100. : 1.;
	if (m_ScrollOffset.y() <= 0) m_ScrollOffset.setY(m_ScrollOffset.y() + dblH);
	applyScrollLimit();
	emit scrollOffsetChanged(static_cast<int>(m_ScrollOffset.x()), static_cast<int>(m_ScrollOffset.y()));
	update();
}

void RenderWidget::onMoveDown()
{
	if (!m_pImgBuff) return;
	double dblH = m_dblZoom * m_pImgBuff->m_height > height() ? height() : 1;
	dblH = dblH > 100. ? dblH / 100. : 1.;
	if (m_ScrollOffset.y() <= 0) m_ScrollOffset.setY(m_ScrollOffset.y() - dblH);
	applyScrollLimit();
	emit scrollOffsetChanged(static_cast<int>(m_ScrollOffset.x()), static_cast<int>(m_ScrollOffset.y()));
	update();
}