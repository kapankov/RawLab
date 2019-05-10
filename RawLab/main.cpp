// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*!
	\file main.cpp
	\brief This file contains a global function named main, which is the designated 
	start of the program.
	\authors Konstantin A. Pankov, explorus@mail.ru
	\copyright Copyright 2017-2019 Konstantin A. Pankov. All rights reserved.
	\license This project is released under the LGPL v.3
	\version 0.1
	\date 02/04/2019
	\warning In developing. Not a stable tested code.
	...

*/


#include "stdafx.h"
#include "rawlab.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QCoreApplication::addLibraryPath("./");
	QApplication a(argc, argv);
	RawLab w;
	w.show();
	if (argc > 0)
	{
		w.openFile(QString(argv[1]));
	}
	return a.exec();
}
