// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"

RawLab::RawLab(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_plblState = new QLabel(this);
	m_plblProgress = new QLabel(this);
	m_plblScale = new QLabel(this);
	m_plblInfo = new QLabel(this);


	statusBar()->addPermanentWidget(m_plblState, 5);
	statusBar()->addPermanentWidget(m_plblProgress, 3);
	statusBar()->addPermanentWidget(m_plblScale, 2);
	statusBar()->addPermanentWidget(m_plblInfo,8);

	m_plblState->setText("Ready");
	m_plblProgress->setText("Not running");
	m_plblInfo->setText("Some information");

	connect(ui.action_Exit, SIGNAL(triggered()), this, SLOT(onExit()));
	connect(ui.action_Open, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.actionZoom_In, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoomIn()));
	connect(ui.actionZoom_Out, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoomOut()));
	connect(ui.actionFit_To_Window, SIGNAL(triggered()), ui.openGLWidget, SLOT(onFitToWindow()));
	connect(ui.actionZoom_100, SIGNAL(triggered()), ui.openGLWidget, SLOT(onUnZoom()));

	connect(ui.action_About, SIGNAL(triggered()), this, SLOT(onAbout()));
}

RawLab::~RawLab()
{
	delete m_plblState;
	delete m_plblProgress;
	delete m_plblScale;
	delete m_plblInfo;
}

void RawLab::onOpen()
{
	auto fileName = QFileDialog::getOpenFileName(this, tr("Open Image File"), "", tr("Jpeg Files (*.jpeg *.jpg);;Raw Files(*.cr2 *.nef *.dng)")); // .toStdString();
	m_plblState->setText(fileName);

	// update RenderWidget
	ui.openGLWidget->SetImageJpegFile(fileName);

}

void RawLab::SetZoomStatus(int prc)
{
	m_plblScale->setText(QString("Scale: %1%").arg(QString::number(prc)));
}

void RawLab::onExit()
{
	QApplication::quit();
}

void RawLab::onAbout()
{
	QMessageBox::about(this, "RawLab", 
		"RawLab ver.0.1,\n"
		"Copyright (C) 2017-2019 Konstantin A. Pankov\n"
		"GNU GPL v.3\n");
}
