#pragma once

#include <QObject>
#include <QtWidgets/QMainWindow>
#include "ui_rawlab.h"

class RawLab : public QMainWindow
{
	Q_OBJECT
	
public:
	RawLab(QWidget *parent = Q_NULLPTR);
	~RawLab();

private:
	Ui::RawLabClass ui;

	QLabel* m_plblState;
	QLabel* m_plblProgress;
	QLabel* m_plblScale;
	QLabel* m_plblInfo;
public slots:
	void onOpen();
	void onExit();
	void onAbout();
	void onZoomChanged(int prc);

};
