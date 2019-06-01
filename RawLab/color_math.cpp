#include "stdafx.h"

// BT.709 (Rec.709)
#define  Pr  .2126
#define  Pg  .7152
#define  Pb  .0722

double getPerceivedBrightness(double r, double g, double b)
{
	return sqrt(r * r * Pr + g * g * Pg + b * b * Pb);
}