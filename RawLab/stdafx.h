#pragma once


#define _USE_MATH_DEFINES
#include <math.h>	// to prevent Qt macro redefinitions

#include <cassert>
#include <string>
#include <codecvt>
#include <map>
#include <thread>
#include <atomic>
#include <chrono>
#include <array>
//#include <omp.h>	// OpenMP
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

#include "common_types.h"
#include "cms.h"
#include "color_math.h"
#include "jpeg.h"
#include "bitmap.h"
#include "settings.h"
#include "settingsdlg.h"
#include "librawex.h"
#include "process_thread.h"
#include "rawlab.h"

