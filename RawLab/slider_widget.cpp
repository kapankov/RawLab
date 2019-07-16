// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"
#include "slider_widget.h"

class SliderDoubleValidator : public QDoubleValidator
{
public:
	SliderDoubleValidator(double bottom, double top, int decimals, QObject *parent = 0)
		:QDoubleValidator(bottom, top, decimals, parent)
	{
		setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
	};
	QValidator::State validate(QString &input, int &pos) const
	{
		/*
		 * Original Qt Documentation:
		 * ---------
		 * QDoubleValidator::validate()
		 * Returns Intermediate if input contains a double that is
		 * outside the range or is in the wrong format; e.g. with too many
		 * digits after the decimal point or is empty.
		 * ---------
		 * Problem: Not what the user expects.
		 * Example: Range 0.0-10.0 with 1 digit (QDoubleValidator( 0, 10, 1, parent ) ):
		 * QDoubleValidator::validate() reports intermediate for "10.3".
		 * However we expect invalid instead and QLineEdit to decline input.
		 * Fix this by overloading the validate() operator.
		 */
		const QValidator::State origState = QDoubleValidator::validate(input, pos);
		double t = top();
		double b = bottom();
		double i = QLocale(QLocale::English, QLocale::UnitedStates).toDouble(input); // not QLocale::system()
		if ((origState == QValidator::Intermediate) && (i > t || i < b))
			return QValidator::Invalid;
		else if ((origState == QValidator::Acceptable) && input.contains(','))
			return QValidator::Invalid;
		else
			return origState;
	}
};

/*
| Label: | ---------0------ | Editor |
*/
#define EDITORWIDTH 64
#define EDITORLEFTMARGIN 4
#define KNOBWDTH 11
#define KNOBHGTH 10

SliderWidget::SliderWidget(QWidget* parent) : 
	m_mode(smStandard)
	, m_value(0.0)
	, m_defvalue(0.0)
	, m_isCaptured(false)
	, m_top(1.0)
	, m_bottom(0.0)
	, m_decimals(2)
	, m_preeditvalue(0.0)
	, m_editor(nullptr)
	, m_BackgroundColor(Qt::gray)
	, m_LabelColor(Qt::white)
	, m_BorderColor(Qt::darkGray)
	, m_GradientA(Qt::black)
	, m_GradientB(Qt::white)

{
	setMouseTracking(true);
	setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
		this, SLOT(onContextMenu(const QPoint &)));
}

SliderWidget::~SliderWidget()
{
	delete m_editor;
}

void SliderWidget::setLabel(const QString & value)
{
	m_Label = value;
}

void SliderWidget::setGradient(const QColor & a, const QColor & b)
{
	m_GradientA = a;
	m_GradientB = b;
}

void SliderWidget::setRange(double top, double bottom, int decimals)
{
	m_top = top;
	m_bottom = bottom;
	m_decimals = decimals;
	update();
}

std::tuple<double, double, int> SliderWidget::getRange() const
{
	return std::make_tuple(m_top, m_bottom, m_decimals);
}

void SliderWidget::setDefaultValue(double value)
{
	m_defvalue = value;
}

QColor SliderWidget::getBackgroundColor() const
{
	return m_BackgroundColor;
}

void SliderWidget::setBackgroundColor(const QColor& c)
{
	m_BackgroundColor = c;
}

QColor SliderWidget::getLabelColor() const
{
	return m_LabelColor;
}

void SliderWidget::setLabelColor(const QColor& c)
{
	m_LabelColor = c;
}

QColor SliderWidget::getDisabledLabelColor() const
{
	return m_DisabledLabelColor;
}

void SliderWidget::setDisabledLabelColor(const QColor& c)
{
	m_DisabledLabelColor = c;
}

QColor SliderWidget::getBorderColor() const
{
	return m_BorderColor;
}

void SliderWidget::setBorderColor(const QColor& c)
{
	m_BorderColor = c;
}

QColor SliderWidget::getDisabledBorderColor() const
{
	return m_DisabledBorderColor;
}

void SliderWidget::setDisabledBorderColor(const QColor& c)
{
	m_DisabledBorderColor = c;
}

void SliderWidget::mousePressEvent(QMouseEvent * event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		double pos = getPos(event->pos());
		if (pos >= 0.0)
		{
			m_isCaptured = true;
			setValueByPos(pos);
			repaint();
		}
		else if (!m_editor)
		{
			QRect r = getEditorRect();
			if (r.contains(event->pos()))
			{
				m_editor = new (std::nothrow) SliderEditor(getStrValue(m_value), this);
				if (m_editor)
				{
					m_preeditvalue = m_value;
					m_editor->move(r.left(), r.top());
					m_editor->resize(r.width(), r.height());
					m_editor->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
					if (SliderDoubleValidator* dblValidator = new (std::nothrow) SliderDoubleValidator(m_bottom, m_top, m_decimals, m_editor))
					{
						dblValidator->setNotation(QDoubleValidator::StandardNotation);
						m_editor->setValidator(dblValidator);
						connect(m_editor, SIGNAL(lostFocus()), this, SLOT(onEditingFinished()));
						connect(m_editor, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
						connect(m_editor, SIGNAL(upValue()), this, SLOT(onUpValue()));
						connect(m_editor, SIGNAL(downValue()), this, SLOT(onDownValue()));
						connect(m_editor, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdited(const QString&)));
						connect(m_editor, SIGNAL(escapePressed()), this, SLOT(onEscapePressed()));
					}
					m_editor->selectAll();
					m_editor->show();
					m_editor->setFocus();
				}
			}
		}
	}
}

void SliderWidget::mouseReleaseEvent(QMouseEvent * event)
{
	m_isCaptured = false;
}

void SliderWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		QRect r = rect();
		r.setRight(r.width() / 3 - 8);
		if (r.contains(event->pos()))
		{
			emit valueChanged(m_value = m_defvalue);
			update();
		}
	}
}

void SliderWidget::mouseMoveEvent(QMouseEvent * event)
{
	if (m_isCaptured && event->buttons() & Qt::LeftButton)
	{
		double pos = getPos(event->pos());
		if (pos >= 0.0)
		{
			setValueByPos(pos);
			repaint();
		}
	}
}

QRect SliderWidget::getSliderRect() const
{
	QRect r = rect();
	r.setLeft(r.width() / 3);
	r.setRight(r.right() - EDITORWIDTH);
	r.setTop(r.top() + r.height() / 2);
	r.setBottom(r.top() + 3);
	return r;
}

double SliderWidget::getPos() const
{
	if (m_value <= m_top && m_value >= m_bottom)
		return (m_value - m_bottom) / (m_top - m_bottom);
	return m_bottom;
}

double SliderWidget::getPos(const QPoint& p) const
{
	QRect r = getSliderRect();
	int pos = p.x() - r.left();
	if (!m_isCaptured)
	{
		if ((r.top() - 6) > p.y() || (r.bottom() + 6) < p.y()) return -1.0;
		if (pos < -6) return -1.0;
		else if (pos >= r.width() + 6) return -1.0;
	}
	if (pos < 0) return 0.0;
	else if (pos >= r.width()) return 1.0;
	return static_cast<double>(pos) / r.width();
}

void SliderWidget::setValueByPos(double pos)
{
	emit valueChanged(m_value = m_bottom + (m_top - m_bottom) * pos);
}

QRect SliderWidget::getEditorRect() const
{
	QRect r = rect();
	r.setLeft(r.right() - EDITORWIDTH + EDITORLEFTMARGIN);
	r.setTop(r.top() + 2);
	r.setBottom(r.bottom() - 2);
	return r;
}

void SliderWidget::setValue(double value, bool update)
{
	if (value > m_top) m_value = m_top;
	else if (value < m_bottom) m_value = m_bottom;
	else m_value = value;
	if (update) this->update();
}

double SliderWidget::getValue() const
{
	return m_value;
}

QString SliderWidget::getStrValue(double value) const
{
	QString s = QString("%1").arg(value, 0, 'f', m_decimals);
	if (s.contains(QRegExp("[,.]*")))
	{
		while (s.endsWith('0')) s.chop(1);
		if (s.endsWith(',') || s.endsWith('.')) s.chop(1);
	}
	return s;
}

void SliderWidget::slideValue(bool up)
{
	if (m_editor)
	{
		QString text = m_editor->text();
		double value = text.isEmpty() ? m_defvalue : text.toDouble();
		if (up)
			value += (m_top - m_bottom) / 100;
		else
			value -= (m_top - m_bottom) / 100;
		if (value > m_top) value = m_top;
		else if (value < m_bottom) value = m_bottom;
		m_value = value;
		m_editor->setText(getStrValue(m_value));
		update();
		emit valueChanged(m_value);
	}
}

void SliderWidget::setMode(SliderMode mode)
{
	m_mode = mode;
}

void SliderWidget::onEditingFinished()
{
	if (m_editor)
	{
		QString text = m_editor->text();
		m_value = text.isEmpty() ? m_defvalue : text.toDouble();
		SliderEditor* editor = m_editor;
		m_editor = nullptr;
		//		editor->hide();
		delete editor;
		update();
		emit valueChanged(m_value);
	}
}

void SliderWidget::onEscapePressed()
{
	if (m_editor)
	{
		m_value = m_preeditvalue;
		SliderEditor* editor = m_editor;
		m_editor = nullptr;
		delete editor;
		update();
		emit valueChanged(m_value);
	}
}

void SliderWidget::onUpValue()
{
	slideValue(true);
}

void SliderWidget::onDownValue()
{
	slideValue(false);
}

void SliderWidget::onTextEdited(const QString & text)
{
	if (m_editor)
	{
		m_value = text.isEmpty() ? m_defvalue/*m_preeditvalue*/ : text.toDouble();
		update();
		emit valueChanged(m_value);
	}
}

void SliderWidget::onContextMenu(const QPoint & pos)
{
/*	if (!m_editor)
	{
		QMenu contextMenu(tr("Slider context menu"), this);

		QAction action1("Change bottom of range", this);
		connect(&action1, SIGNAL(triggered()), this, SLOT(changeBottom()));
		contextMenu.addAction(&action1);

		QAction action2("Change top of range", this);
		connect(&action2, SIGNAL(triggered()), this, SLOT(changeTop()));
		contextMenu.addAction(&action2);

		QAction action3("Change floating point precision", this);
		connect(&action3, SIGNAL(triggered()), this, SLOT(changePrecision()));
		contextMenu.addAction(&action3);

		QAction action4("Change default value", this);
		connect(&action4, SIGNAL(triggered()), this, SLOT(changeDefault()));
		contextMenu.addAction(&action4);

		contextMenu.exec(mapToGlobal(pos));
	}*/
}

void SliderWidget::changeBottom()
{
	setMode(smBottom);
}

void SliderWidget::changeTop()
{
	setMode(smTop);
}

void SliderWidget::changePrecision()
{
	setMode(smPrecision);
}

void SliderWidget::changeDefault()
{
	setMode(smDefault);
}

bool SliderWidget::event(QEvent * event)
{
	if (event->type() == QEvent::ToolTip) {
		QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
		
		// Label
		QRect rcLabel = rect();
		rcLabel.setRight(rcLabel.width() / 3 - 8);
		QRect rc = rect();
		rc.setLeft(rc.width() / 3 - 8);

		if (rcLabel.contains(helpEvent->pos()))
		{
			QToolTip::showText(helpEvent->globalPos(), toolTip());
		}
		else if (rc.contains(helpEvent->pos()))
		{
			QString tip = getStrValue(m_bottom) + ".." + getStrValue(m_top) + tr(", default = ") + getStrValue(m_defvalue);
			QToolTip::showText(helpEvent->globalPos(), tip);
		}
		else
		{
			QToolTip::hideText();
			event->ignore();
		}

		return true;
	}
	return QWidget::event(event);
}

void SliderWidget::paintEvent(QPaintEvent* event)
{
//	QWidget::paintEvent(event); // draws the background

	QPainter painter(this);
	// Label
	QRect r = rect();
	painter.fillRect(r, getBackgroundColor());
	r.setRight(r.width() / 3 - 8);
	painter.setPen(isEnabled() ? getLabelColor() : getDisabledLabelColor());
	painter.drawText(r, Qt::AlignRight | Qt::AlignVCenter, m_Label);
	// Value
	if (!m_editor)
	{
		r = rect();
		r.setLeft(r.right() - EDITORWIDTH + 8);
		r.setRight(r.right() - 2);
		painter.drawText(r, Qt::AlignRight | Qt::AlignVCenter, getStrValue(m_value));
	}
	// Central slider
	r = getSliderRect();
	painter.fillRect(r, isEnabled() ? getBorderColor() : getDisabledBorderColor());
	r.setLeft(r.left() + 1);
	r.setTop(r.top() + 1);
	r.setRight(r.right() - 1);
	r.setBottom(r.bottom() - 1);
	//painter.fillRect(r, getBackgroundColor());
	QLinearGradient gradient(r.topLeft(), r.topRight());
	if (isEnabled())
	{
		// градиент активного слайдера
		gradient.setColorAt(0, m_GradientA);
		gradient.setColorAt(1, m_GradientB);
	}
	else
	{
		// градиент неактивного слайдера
		double b = getPerceivedBrightness(
			static_cast<double>(m_GradientA.red()) / 255., 
			static_cast<double>(m_GradientA.green()) / 255., 
			static_cast<double>(m_GradientA.blue()) / 255.
		);
		gradient.setColorAt(0, QColor::fromRgb(static_cast<int>(b*255.), static_cast<int>(b*255.), static_cast<int>(b*255.)));
		b = getPerceivedBrightness(
			static_cast<double>(m_GradientB.red()) / 255.,
			static_cast<double>(m_GradientB.green()) / 255.,
			static_cast<double>(m_GradientB.blue()) / 255.
		);
		gradient.setColorAt(1, QColor::fromRgb(static_cast<int>(b*255.), static_cast<int>(b*255.), static_cast<int>(b*255.)));
	}
	painter.fillRect(r, gradient);
	// Marks
	r = getSliderRect();
	QLine lines[5];
	lines[0].setLine(r.left(), r.top() - 6, r.left(), r.top() - 3);
	lines[1].setLine(r.left() + (r.width()>>2) - 1, r.top() - 5, r.left() + (r.width()>>2) - 1, r.top() - 3);
	lines[2].setLine(r.left() + (r.width()>>1) - 1, r.top() - 6, r.left() + (r.width()>>1) - 1, r.top() - 3);
	lines[3].setLine(r.left() + ((r.width() * 3)>>2) - 1, r.top() - 5, r.left() + ((r.width() * 3)>>2) - 1, r.top() - 3);
	lines[4].setLine(r.right(), r.top() - 6, r.right(), r.top() - 3);
	painter.setPen(isEnabled() ? getBorderColor() : getDisabledBorderColor());
	painter.drawLines(lines, 5);

/*	QImage knob(const_cast<uchar*>(btKnob), KNOBWDTH, KNOBHGTH, sizeof(btKnob) / KNOBHGTH, QImage::Format_RGB888);
	knob.convertToFormat(QImage::Format_ARGB32);
	knob.setAlphaChannel(knob.createMaskFromColor(qRgb(btKnob[0], btKnob[1], btKnob[2]), Qt::MaskOutColor)); */
	QImage knob(const_cast<uchar*>(isEnabled() ? m_btKnob : m_btDsblKnob),
		KNOBWDTH, 
		KNOBHGTH, 
		(isEnabled() ? sizeof(m_btKnob) : sizeof(m_btDsblKnob)) / KNOBHGTH,
		QImage::Format_ARGB32);

	QRect rctKnob = rect();
	rctKnob.setLeft(rctKnob.width() / 3);
	rctKnob.setRight(rctKnob.right() - EDITORWIDTH);
	rctKnob.setTop(rctKnob.top() + rctKnob.height() / 2 - 3);
//	rctKnob.setBottom(rctKnob.top() + KNOBHGTH - 1);
	rctKnob.setLeft(rctKnob.left() + static_cast<int>(static_cast<double>(rctKnob.width() - 1) * getPos()) - KNOBWDTH/2);
//	rctKnob.setRight(rctKnob.left() + KNOBWDTH - 1);
	painter.drawImage(rctKnob.left(), rctKnob.top(), knob);
}

