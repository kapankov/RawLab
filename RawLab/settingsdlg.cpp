// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"

SettingsDialog::SettingsDialog(CSettings* pSettings, QWidget* parent)
	: QDialog(parent)
	, m_settings(pSettings)
{
	ui.setupUi(this);
	// remove question mark from the title bar
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	connect(ui.edtSaveLastDir, SIGNAL(textChanged(const QString&)), this, SLOT(onLastDirChanged(const QString&)));
	connect(ui.btnOpenLastDir, SIGNAL(clicked(bool)), this, SLOT(onSelectLastDir()));
	connect(ui.chbSaveLastDir, SIGNAL(clicked(bool)), this, SLOT(onSaveLastDirClicked(bool)));
	connect(ui.chbAutoGreen2, SIGNAL(clicked(bool)), this, SLOT(onAutoGreen2Clicked(bool)));
	connect(ui.sbJpegQual, SIGNAL(valueChanged(int)), this, SLOT(onJpegQualChanged(int)));
	

	if (m_settings)
	{
		QString lastDir = QString::fromStdString(m_settings->getValue(std::string("lastdir")));
		if (!lastDir.isEmpty())
		{
			if (lastDir.at(0) != '!') // всегда использовать указанную директорию в качестве последней
			{
				ui.chbSaveLastDir->setChecked(m_saveLastDir = true);
				ui.edtSaveLastDir->setText(m_lastDir = lastDir);
			}
			else
			{
				ui.chbSaveLastDir->setChecked(m_saveLastDir = false);
				ui.edtSaveLastDir->setText(m_lastDir = lastDir.remove('!'));
			}
		}
		else // не сохранять последнюю директорию
		{
			ui.chbSaveLastDir->setChecked(m_saveLastDir = false);
			ui.edtSaveLastDir->setText(m_lastDir = lastDir);
		}
		m_isAutoGreen2 = m_settings->getValue(std::string("autogreen2")).compare(std::string("true")) == 0;
		ui.chbAutoGreen2->setChecked(m_isAutoGreen2);
		QString jpegQual = QString::fromStdString(m_settings->getValue(std::string("jpegqual")));
		m_JpegQual = jpegQual.toInt();
		if (m_JpegQual < JPEGQUALMIN || m_JpegQual > JPEGQUALMAX) m_JpegQual = JPEGQUALDEF;
		ui.sbJpegQual->setValue(m_JpegQual);
	}
	else
		ui.sbJpegQual->setValue(JPEGQUALDEF);

}

void SettingsDialog::onLastDirChanged(const QString& text)
{
	m_lastDir = text;
}

void SettingsDialog::onSelectLastDir()
{
	QString dir = ui.edtSaveLastDir->text();
	QFileInfo check_file(dir);
	dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
		check_file.exists()?dir:"",
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) ui.edtSaveLastDir->setText(dir);
}

QString SettingsDialog::getSaveLastDir()
{
	// не проверяем на существование каталога

	if (!m_saveLastDir)
		return QString("!") + m_lastDir;
	else
		return m_lastDir;

}

bool SettingsDialog::getAutoGreen2()
{
	return m_isAutoGreen2;
}

int SettingsDialog::getJpegQuality()
{
	return m_JpegQual;
}

void SettingsDialog::onSaveLastDirClicked(bool checked)
{
	m_saveLastDir = checked;
}

void SettingsDialog::onAutoGreen2Clicked(bool checked)
{
	m_isAutoGreen2 = checked;
}

void SettingsDialog::onJpegQualChanged(int i)
{
	m_JpegQual = i;
}

