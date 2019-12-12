#ifndef HISTOGRAM_WIDGET_H
#define HISTOGRAM_WIDGET_H

#include <QObject>

class HistogramWidget : public QWidget
{

	Q_OBJECT

	const size_t m_Columns = 256;
	const size_t m_HorzLines = 4; // ���-�� �������������� ����� �� �����������
	const int m_StatisticsIndent = 6;
	std::unique_ptr<QImage> m_imm;
	QString m_stat; // ����������
public:
	explicit HistogramWidget(QWidget* parent = 0);
signals:

public slots:
	void onImageChanged(RgbBuff* rgbBuff);
private:
	void paintEvent(QPaintEvent* event);
};

#endif