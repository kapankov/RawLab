// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"
#include "version.h"

RawLab::RawLab(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_settings.setDefaultValue(QString("opendir").toStdWString(), QFileInfo(QCoreApplication::applicationFilePath()).path().toStdWString());
	m_settings.setDefaultValue(QString("minfilter").toStdWString(), QString("LINEAR").toStdWString());
	m_settings.setDefaultValue(QString("magfilter").toStdWString(), QString("NEAREST").toStdWString());
	m_settings.setPath(QFileInfo(QCoreApplication::applicationFilePath()).path().toStdString());

	m_plblState = new QLabel(this);
	m_plblProgress = new QLabel(this);
	m_plblScale = new QLabel(this);
	m_plblInfo = new QLabel(this);

	statusBar()->addPermanentWidget(m_plblState, 5);
	statusBar()->addPermanentWidget(m_plblProgress, 3);
	statusBar()->addPermanentWidget(m_plblScale, 2);
	statusBar()->addPermanentWidget(m_plblInfo,8);

	m_plblState->setText(tr("Ready"));
	m_plblProgress->setText(tr("Not running"));
	m_plblInfo->setText(tr("Some information"));

	ui.imageScrollWidget->setViewport(ui.openGLWidget);

	connect(ui.action_Exit, SIGNAL(triggered()), this, SLOT(onExit()));
	connect(ui.action_Open, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.actionZoom_In, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoomIn()));
	connect(ui.actionZoom_Out, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoomOut()));
	connect(ui.actionFit_To_Window, SIGNAL(triggered()), ui.openGLWidget, SLOT(onFitToWindow()));
	connect(ui.actionZoom_25, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoom_25()));
	connect(ui.actionZoom_50, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoom_50()));
	connect(ui.actionZoomToNormal, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoomToNormal()));
	connect(ui.actionZoom_200, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoom_200()));
	connect(ui.actionZoom_400, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoom_400()));
	connect(ui.actionZoom_800, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoom_800()));
	connect(ui.actionCenter, SIGNAL(triggered()), ui.openGLWidget, SLOT(onCenter()));
	connect(ui.openGLWidget, SIGNAL(zoomChanged(int)), this, SLOT(onZoomChanged(int)));
	connect(ui.openGLWidget, SIGNAL(scrollSizeChanged(int, int)), ui.imageScrollWidget, SLOT(onScrollSizeChanged(int, int)));
	connect(ui.openGLWidget, SIGNAL(scrollOffsetChanged(int, int)), ui.imageScrollWidget, SLOT(onScrollOffsetChanged(int, int)));
	connect(ui.action_About, SIGNAL(triggered()), this, SLOT(onAbout()));
}

RawLab::~RawLab()
{
	delete m_plblState;
	delete m_plblProgress;
	delete m_plblScale;
	delete m_plblInfo;
}

void RawLab::openFile(const QString& filename)
{
	if (filename.isNull()) return;
	QFileInfo check_file(filename);
	if (check_file.exists() && check_file.isFile())
	{
		m_plblState->setText(filename);
		ui.openGLWidget->SetImageJpegFile(filename);
	}
}

void RawLab::onOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image File"), "", tr("Jpeg Files (*.jpeg *.jpg);;Raw Files(*.cr2 *.nef *.dng)")); // .toStdString();
	openFile(fileName);
}

void RawLab::onZoomChanged(int prc)
{
	m_plblScale->setText(QString(tr("Scale: %1%")).arg(QString::number(prc)));
}

void RawLab::onExit()
{
	QApplication::quit();
}

void RawLab::onAbout()
{
	QMessageBox::about(this, APPNAME,
		QString(APPNAME " ver. %1.%2.%3 build %4\n"
		"Copyright(C) " LEGALCOPYRIGHT "\n"
		PRODUCTSUPPORT "\n"
		"\n"
		"IJG JPEG LIBRARY (libjpeg) ver. 8d\n"
		"Copyright(C) 1991 - 2012, Thomas G.Lane, Guido Vollbeding\n").arg(QString::number(MAJOR_VER), QString::number(MINOR_VER), QString::number(RELEASE_VER), QString::number(BUILD_VER)));
}
