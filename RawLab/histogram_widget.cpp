// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"
#include "histogram_widget.h"

HistogramWidget::HistogramWidget(QWidget* parent)
	: m_img(nullptr)
{

}

void HistogramWidget::onImageChanged(RgbBuff* buff)
{
	m_img = buff;
	if (m_img && m_img->m_buff)
	{
		// ������� �����������
		m_img->alloc_histogram();
		size_t stride = static_cast<size_t>(m_img->m_width) * 3;
		if (stride & 3) stride += SIZEOFDWORD - stride & 3; // DWORD aligned

		for (size_t h = 0, hcount = static_cast<size_t>(m_img->m_height); h < hcount; ++h)
			for (size_t w = 0, wcount = static_cast<size_t>(m_img->m_width); w < wcount; ++w)
			{
				size_t n = h * stride + w * 3;
				for (size_t c = 0; c < 3; ++c)
				{
					const unsigned char& k = m_img->m_buff[n + c];
					m_img->m_histogram[c][k]++;
					if (!m_img->m_histogram[c][k]) m_img->m_histogram[c][k]--; // �� ������ ������������
				}
			}

		QRect hr(0, 0, m_Columns, rect().height() - 6);
		m_imm = std::make_unique<QImage>(hr.width(), hr.height(), QImage::Format_RGB888);
		QPainter painter(m_imm.get());

		painter.fillRect(hr, QColor(0, 0, 0));

		painter.setCompositionMode(QPainter::CompositionMode_Plus);

		size_t count = m_img->m_histogram[0].size();
		// ������ �������� ��� ���������� �������� (�����) �� ���� ���� �������
		double maxh = std::max(
			std::max(
				*std::max_element(m_img->m_histogram[0].begin(), m_img->m_histogram[0].end() - 1),
				*std::max_element(m_img->m_histogram[1].begin(), m_img->m_histogram[1].end() - 1)
			),
			*std::max_element(m_img->m_histogram[2].begin(), m_img->m_histogram[2].end() - 1)
		);

		m_stat = QString(tr("max = %1\n\nOE:\nR = %2\nG = %3\nB = %4\n\nUE:\nR = %5\nG = %6\nB = %7")).arg(
			QString::number(static_cast<int>(maxh)),
			QString::number(m_img->m_histogram[0].at(255)),
			QString::number(m_img->m_histogram[1].at(255)),
			QString::number(m_img->m_histogram[2].at(255)),
			QString::number(m_img->m_histogram[0].at(0)),
			QString::number(m_img->m_histogram[1].at(0)),
			QString::number(m_img->m_histogram[2].at(0)));
		// ����������� ��������� �������
		for (size_t c = 0; c < 3; ++c)
		{
			switch (c)
			{
			case 0:
				painter.setPen(QPen(QColor(0xb0, 0x60, 0x60))); // Red channel
				break;
			case 1:
				painter.setPen(QPen(QColor(0x60, 0xb0, 0x60))); // Green channel
				break;
			case 2:
				painter.setPen(QPen(QColor(0x60, 0x60, 0xb0))); // Blue channel
				break;
			};
			for (size_t i = 0; i < count; ++i)
			{
				double value = m_img->m_histogram[c].at(i) / maxh;
				painter.drawLine(QPoint(hr.left() + static_cast<int>(i), hr.bottom()), QPoint(hr.left() + static_cast<int>(i), hr.bottom() - value * (hr.height())));
			}
		}
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
		// �����
		painter.setPen(QPen(QColor(0x80, 0x80, 0x80)));
		for (size_t i = 0; i < 10; ++i)
		{
			int x = static_cast<int>(25.5 * static_cast<double>(i + 1));
			painter.drawLine(QPoint(hr.left() + x, 0), QPoint(hr.left() + x, hr.bottom()));
		}
		for (size_t i = 0; i < m_HorzLines; ++i)
		{
			int y = static_cast<int>(static_cast<double>(hr.height())/ static_cast<double>(m_HorzLines) * static_cast<double>(i + 1));
			painter.drawLine(QPoint(0, y), QPoint(hr.right(), y));
		}
		// ������������ ����� (����� ���������� ������ ������, ����� ����������)
		for (size_t c = 0; c < 3; ++c)
		{
			switch (c)
			{
			case 0:
				painter.setPen(QPen(QColor(0xf0, 0x60, 0x60))); // Red channel
				break;
			case 1:
				painter.setPen(QPen(QColor(0x60, 0xf0, 0x60))); // Green channel
				break;
			case 2:
				painter.setPen(QPen(QColor(0x60, 0x60, 0xf0))); // Blue channel
				break;
			};
			QPoint prev(0, 0);
			for (size_t i = 0; i < count; ++i)
			{
				double value = m_img->m_histogram[c].at(i) / maxh;
				QPoint next(hr.left() + i, hr.bottom() - (value * (hr.height())));
				painter.drawLine(prev, next);
				prev = next;
			}
			painter.drawLine(prev, QPoint(static_cast<int>(count), hr.bottom()));
		}

	}
	else
		m_imm.reset();
	update();
}

void HistogramWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	QRect r = rect();
	r.adjust(1, 1, -1, -1);	// non-client imitation
	painter.fillRect(r, QColor::fromRgb(0x88, 0x88, 0x88));
	r.adjust(1, 1, -1, -1);
	painter.fillRect(r, QColor::fromRgb(0x60, 0x60, 0x60));

	if (m_imm)
	{
		painter.setCompositionMode(QPainter::CompositionMode_Lighten);
		painter.drawImage(r.topLeft()+=QPoint(1,1), *m_imm);
		// ����������
		r.adjust(m_Columns + 4, 2, -2, - 2);
		painter.setPen(QColor::fromRgb(0xA0, 0xA0, 0xA0));

		painter.drawText(r, Qt::AlignLeft | Qt::AlignTop, m_stat);
	}
}
