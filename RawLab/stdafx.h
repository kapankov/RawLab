#pragma once

#define _USE_MATH_DEFINES
#include <math.h>	// to prevent Qt macro redefinitions

#include <string>
#include <codecvt>
#include <map>
#include <winsock2.h>
#include <iconv.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <QtWidgets>
#include <QFileInfo>
#include <jpeglib.h>
#include <lcms2.h>
#include <pthread.h>
#pragma warning(push)
#pragma warning(disable: 4251)
#include "libraw/libraw.h"
#pragma warning(pop)

#include "jpeg.h"
#include "settings.h"
#include "librawex.h"
#include "rawlab.h"

