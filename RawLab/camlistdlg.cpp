// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"


CamListDialog::CamListDialog(const char** camlist, QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	// remove question mark from the title bar
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	char** camList = const_cast<char**>(camlist);
	size_t i = 0;
	while (camList[i])
	{
		ui.listCameras->addItem(QString(camList[i++]));
	}
	ui.lblTotal->setText(QString("Total: %1").arg(i));
}