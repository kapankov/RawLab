// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"

// BT.709 (Rec.709)
#define  Pr  .2126
#define  Pg  .7152
#define  Pb  .0722

double getPerceivedBrightness(double r, double g, double b)
{
	return sqrt(r * r * Pr + g * g * Pg + b * b * Pb);
}