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
		if (check_file.suffix().compare("jpg", Qt::CaseInsensitive) == 0 || check_file.suffix().compare("jpeg", Qt::CaseInsensitive) == 0)
		{
			if (!ui.openGLWidget->SetImageJpegFile(filename))
				QMessageBox::critical(this, tr("RawLab error"), QString(tr("Unable to open jpeg file:\n")) + filename);
			else
				m_plblState->setText(filename);
		}
		else
		{
			if (!ui.openGLWidget->SetImageRawFile(filename))
				QMessageBox::critical(this, tr("RawLab error"), QString(tr("Unable to open RAW file:\n")) + filename);
			else
				m_plblState->setText(filename);
		}
	}
}

void RawLab::onOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image File"), "", 
		tr(	"Adobe Digital Negative files (*.dng);;"
			"Canon RAW 2 files (*.cr2);;"
			"Nikon RAW files (*.nef);;"
			"FujiFilm RAW files (*.raf);;"
			"Hasselblad RAW files (*.3fr);;"
			"Sony Alpha RAW files (*.arw);;"
			"Canon RAW files (*.ciff);;"
			"Canon RAW files (*.crw);;"
			"Kodak RAW files (*.dcr);;"
			"Epson RAW files (*.erf);;"
			"Phase One RAW files (*.iiq);;"
			"Kodak DC25 RAW files (*.k25);;"
			"Kodak RAW files (*.kdc);;"
			"Mamiya RAW files (*.mef);;"
			"Minolta RAW files (*.mrw);;"
			"Nikon RAW 2 files (*.nrw);;"
			"Olympus RAW files (*.orf);;"
			"Pentax RAW files (*.pef);;"
			"Panasonic RAW files (*.raw);;"
			"Panasonic RAW 2 files (*.rw2);;"
			"Leica RAW files (*.rwl);;"
			"Sony RAW 2 files (*.sr2);;"
			"Sony RAW files (*.srf);;"
			"Samsung RAW files (*.srw);;"
			"Sigma/Foveon RAW files (*.x3f);;"
			"Jpeg Files (*.jpeg *.jpg);;"
			"All Files (*.*)")); // .toStdString();
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
		"Qt ver. %5\n"
		"Copyright (C) 2016 The Qt Company Ltd.\n"
		"\n"
		"IJG JPEG LIBRARY (libjpeg) ver. %6.%7\n"
		"Copyright(C) 1991 - 2012, Thomas G.Lane, Guido Vollbeding\n"
		"\n").
		arg(QString::number(MAJOR_VER), QString::number(MINOR_VER), QString::number(RELEASE_VER), QString::number(BUILD_VER), 
			QT_VERSION_STR,
			QString::number(JPEG_LIB_VERSION_MAJOR), QString::number(JPEG_LIB_VERSION_MINOR)
		) +
		QString("GNU LIBICONV ver. %1.%2\n"
		"Copyright (C) 1999-2019 Free Software Foundation, Inc.\n"
		"\n"
		"LIBXML ver. %3\n"
		"Copyright (C) 1998-2012 Daniel Veillard.  All Rights Reserved.\n"
		"\n"
		"Little CMS ver. %4\n"
		"Copyright (c) 1998-2011 Marti Maria Saguer\n"
		"\n"
		"PTHREADS-WIN32 ver. %5\n"
		"Copyright(C) 1998 John E. Bossom\n"
		"Copyright(C) 1999,2005 Pthreads-win32 contributors\n"
		"\n"
		"RawSpeed\n"
		"Copyright (C) 2009 Klaus Post\n"
		"\n"
		"LibRaw ver. %6\n"
		"Copyright 2008-2019 LibRaw LLC (info@libraw.org)").
		arg(QString::number((_LIBICONV_VERSION >> 8) & 0xFF), QString::number(_LIBICONV_VERSION & 0xFF),
			LIBXML_DOTTED_VERSION,
			QString::number(LCMS_VERSION / 1000.0),
			QString(PTW32_VERSION_STRING).replace(", ", "."),
			LIBRAW_VERSION_STR
		)
	);
}
