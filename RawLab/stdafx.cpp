// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"

std::string toStdString(QString str)
{
	QByteArray ba = str.toLocal8Bit();
	return std::string(ba.data(), ba.size());
}