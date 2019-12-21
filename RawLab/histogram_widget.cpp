// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"
#include "histogram_widget.h"

HistogramWidget::HistogramWidget(QWidget* /* parent*/)
{
}

void HistogramWidget::onImageChanged(RgbBuff* rgbBuff)
{
	if (rgbBuff && rgbBuff->m_buff)
	{
		bool is16bit = rgbBuff->m_bits == 16;
		// создаем гистограмму
		size_t stride = rgbBuff->m_width * static_cast<size_t>(3 * rgbBuff->m_bits/8);
		if (stride & 3) stride += SIZEOFDWORD - stride & 3; // DWORD aligned

		unsigned char* buff = rgbBuff->m_buff;
//		#pragma omp parallel
		for (size_t c = 0; c < 3; ++c)
		{
			histogram_channel& hg = rgbBuff->m_histogram[c];
			for (size_t h = 0, hcount = static_cast<size_t>(rgbBuff->m_height); h < hcount; ++h)
				for (size_t w = 0, wcount = static_cast<size_t>(rgbBuff->m_width); w < wcount; ++w)
				{
					size_t n = h * stride + w * static_cast<size_t>(3 * rgbBuff->m_bits / 8);
					unsigned char k = 0;
					if (is16bit)
						k = buff[n + (c << 1) + 1];
					else
						k = buff[n + c];
					unsigned int& t = hg[k];
					// на случай переполнения
					if (t != UINT_MAX)
						t++;
				}
		}

		QRect hr(0, 0, static_cast<int>(m_Columns), rect().height() - 6);
		m_imm = std::make_unique<QImage>(hr.width(), hr.height(), QImage::Format_RGB888);
		QPainter painter(m_imm.get());

		painter.fillRect(hr, QColor(0, 0, 0));

		painter.setCompositionMode(QPainter::CompositionMode_Plus);

		histogram_channel& hgred = rgbBuff->m_histogram[0];
		histogram_channel& hggreen = rgbBuff->m_histogram[1];
		histogram_channel& hgblue = rgbBuff->m_histogram[0];

		size_t count = hgred.size();
		// найдем максимум без последнего элемента (света) во всех трех каналах
		double maxh = std::max(
			std::max(
				*std::max_element(hgred.begin(), hgred.end() - 1),
				*std::max_element(hggreen.begin(), hggreen.end() - 1)
			),
			*std::max_element(hgblue.begin(), hgblue.end() - 1)
		);

		m_stat = QString(tr("max = %1\n\nOE:\nR = %2\nG = %3\nB = %4\n\nUE:\nR = %5\nG = %6\nB = %7")).arg(
			QString::number(static_cast<int>(maxh)),
			QString::number(hgred.at(255)),
			QString::number(hggreen.at(255)),
			QString::number(hgblue.at(255)),
			QString::number(hgred.at(0)),
			QString::number(hggreen.at(0)),
			QString::number(hgblue.at(0)));
		// прорисовать наложение каналов
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
				double value = rgbBuff->m_histogram[c][i] / maxh;
				painter.drawLine(QPoint(hr.left() + static_cast<int>(i), hr.bottom()), QPoint(hr.left() + static_cast<int>(i), hr.bottom() - value * (hr.height())));
			}
		}
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
		// сетка
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
		// восстановить линии (можно нарисовать другим цветом, более выраженным)
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
				double value = rgbBuff->m_histogram[c][i] / maxh;
				QPoint next(hr.left() + static_cast<int>(i), hr.bottom() - (value * (hr.height())));
				painter.drawLine(prev, next);
				prev = next;
			}
			painter.drawLine(prev, QPoint(static_cast<int>(count), hr.bottom()));
		}

	}

	update();
}

void HistogramWidget::paintEvent(QPaintEvent* /*event*/)
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
		// статистика
		r.adjust(static_cast<int>(m_Columns) + m_StatisticsIndent, 2, -2, - 2);
		painter.setPen(QColor::fromRgb(0xA0, 0xA0, 0xA0));

		painter.drawText(r, Qt::AlignLeft | Qt::AlignTop, m_stat);
	}
}
