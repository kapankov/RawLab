// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"

//#define isequal(x,y,p) (abs((x)-(y))<(p))
//constexpr cmsInt32Number iccType = 2;

// from http://brucelindbloom.com/index.html?WorkingSpaceInfo.html
constexpr cmsCIExyYTRIPLE adobe_primaries = {
{0.6400, 0.3300, 1.0},
{0.2100, 0.7100, 1.0},
{0.1500, 0.0600, 1.0}
};

cmsHPROFILE cmsCreate_AdobeRGBProfile()
{
	cmsToneCurve* curve[3];
	cmsCIExyY whitepoint = { 0.3127, 0.3290, 1.0 };
	curve[0] = curve[1] = curve[2] = cmsBuildGamma(NULL, 2.19921875);
	cmsHPROFILE hProfile = cmsCreateRGBProfile(&whitepoint, &adobe_primaries, curve);
	cmsFreeToneCurve(curve[0]);
	return hProfile;
}

void* getProfile(cmsHPROFILE hProfile)
{
	void* MemPtr = nullptr;
	cmsUInt32Number BytesNeeded = 0;
	if (cmsSaveProfileToMem(hProfile, MemPtr, &BytesNeeded))
	{
		if (BytesNeeded)
		{
			MemPtr = new unsigned char[BytesNeeded];
			cmsSaveProfileToMem(hProfile, MemPtr, &BytesNeeded);
		}
	}
	return MemPtr;
}

void* getAdobeProfile()
{
	void* MemPtr = nullptr;
	if (cmsHPROFILE hProfile = cmsCreate_AdobeRGBProfile())
	{
		MemPtr = getProfile(hProfile);
		cmsCloseProfile(hProfile);
	}
	return MemPtr;
}

void* getsRGBProfile()
{
	void* MemPtr = nullptr;
	if (cmsHPROFILE hProfile = cmsCreate_sRGBProfile())
	{
		MemPtr = getProfile(hProfile);
		cmsCloseProfile(hProfile);
	}
	return MemPtr;
}
/*
cmsHPROFILE cmsCreateProfile(int iColorSpace, cmsToneCurve* tonecurve, cmsCIExyYTRIPLE* pprimaries, cmsCIExyY* pwhitepoint)
{
	cmsHPROFILE hProfile = 0;
	if (iColorSpace >= 0 && iColorSpace < 7)
	{
		// from http://brucelindbloom.com/index.html?WorkingSpaceInfo.html
		cmsCIExyYTRIPLE srgb_primaries = {
			{0.6400, 0.3300, 1.0},
			{0.3000, 0.6000, 1.0},
			{0.1500, 0.0600, 1.0}
		};

		cmsCIExyYTRIPLE wide_primaries = {
			{0.7350, 0.2650, 1.0},
			{0.1150, 0.8260, 1.0},
			{0.1570, 0.0180, 1.0}
		};
		cmsCIExyYTRIPLE prophoto_primaries = {
			{0.7347, 0.2653, 1.0},
			{0.1596, 0.8404, 1.0},
			{0.0366, 0.0001, 1.0}
		};
		cmsCIExyYTRIPLE aces_primaries = {
			{0.7347, 0.2653, 1.0},
			{0.00001, 1.0, 1.0},
			{0.0001, -0.077, 1.0}
		};
		cmsCIExyYTRIPLE xyz_primaries = {
			{1.0, .0, 1.0},
			{.0, 1.0, 1.0},
			{.0, .0, 1.0}
		};

		cmsCIExyY whitepoint;
		// точки белого для разных спецификаций
		cmsCIExyY d65_wp = { 0.3127, 0.3290, 1.0 };
//		cmsCIExyY d60_aces = { 0.32168, 0.33767, 1.0 };

		cmsToneCurve* curve[3];
		const cmsFloat64Number srgb_parameters[5] = { 2.4, 1.0 / 1.055,  0.055 / 1.055, 1.0 / 12.92, 0.04045 };

		switch (iColorSpace)
		{
		case 0: // RAW
			assert(pwhitepoint);
			assert(pprimaries);
			curve[0] = curve[1] = curve[2] = tonecurve ? tonecurve : cmsBuildGamma(NULL, 1.00);
			hProfile = cmsCreateRGBProfile(pwhitepoint, pprimaries, curve);
			break;
		case 1: // sRGB
			whitepoint = d65_wp;
			curve[0] = curve[1] = curve[2] = tonecurve ? tonecurve : cmsBuildParametricToneCurve(NULL, iccType, srgb_parameters);
			hProfile = cmsCreateRGBProfile(&whitepoint, &srgb_primaries, curve);
			break;
		case 2: // AdobeRGB
			whitepoint = d65_wp;
			curve[0] = curve[1] = curve[2] = tonecurve ? tonecurve : cmsBuildGamma(NULL, 2.19921875);
			hProfile = cmsCreateRGBProfile(&whitepoint, &adobe_primaries, curve);
			break;
		case 3: // WideRGB
			whitepoint = d65_wp; //d50_romm_wp;
			curve[0] = curve[1] = curve[2] = tonecurve ? tonecurve : cmsBuildGamma(NULL, 2.19921875);
			hProfile = cmsCreateRGBProfile(&whitepoint, &wide_primaries, curve);
			break;
		case 4: // ProPhoto
			whitepoint = d65_wp; // d50_romm_wp;
			curve[0] = curve[1] = curve[2] = tonecurve ? tonecurve : cmsBuildGamma(NULL, 1.80078125);
			hProfile = cmsCreateRGBProfile(&whitepoint, &prophoto_primaries, curve);
			break;
		case 5: // XYZ 65
			//hProfile = cmsCreateXYZProfile();
			whitepoint = d65_wp;
			curve[0] = curve[1] = curve[2] = tonecurve ? tonecurve : cmsBuildGamma(NULL, 1.00);
			hProfile = cmsCreateRGBProfile(&whitepoint, &xyz_primaries, curve);
			break;
		case 6: // ACES
			whitepoint = d65_wp; // d60_aces;
			curve[0] = curve[1] = curve[2] = tonecurve ? tonecurve : cmsBuildGamma(NULL, 1.00);
			hProfile = cmsCreateRGBProfile(&whitepoint, &aces_primaries, curve);
			break;
		}
		if (!tonecurve) cmsFreeToneCurve(curve[0]);
	}

	return hProfile;
}

cmsToneCurve* cmsGetToneCurve(const double* gamm)
{
	cmsToneCurve* tonecurve = nullptr;
	// если задана гамма, построить тоновую кривую
	if (gamm)
	{
		if (isequal(1.0 / gamm[0], 1, 0.1) && isequal(gamm[1], 1, 0.1))
			tonecurve = cmsBuildGamma(NULL, 1.00);
		else if (isequal(gamm[1], 0, 0.001))
			tonecurve = cmsBuildGamma(NULL, 1.0 / gamm[0]);
		else
		{
			cmsFloat64Number ToneCurve_parameters[5];
			ToneCurve_parameters[0] = 1.0 / gamm[0];
			ToneCurve_parameters[1] = 1.0 / (1.0 + gamm[4]);
			ToneCurve_parameters[2] = gamm[4] / (1.0 + gamm[4]);
			ToneCurve_parameters[3] = 1.0 / gamm[1];
			ToneCurve_parameters[4] = gamm[3] * gamm[1];
			tonecurve = cmsBuildParametricToneCurve(NULL, iccType, ToneCurve_parameters);
		}
	}
	return tonecurve;
}

// Get Color profile
// создать профиль по индексу 0..6, гамме и параметрами primaries+whitepoint
// iColorSpace:
//	0 - RAW, необходимо указывать pprimaries и pwhitepoint
//	1 - sRGB
//	2 - AdobeRGB
//	3 - WideRGB
//	4 - ProPhoto
//	5 - XYZ
//	6 - ACES
// gamm - гамма
cmsHPROFILE cmsCreateProfile(const int iColorSpace, const double* gamm, cmsCIExyYTRIPLE* pprimaries, cmsCIExyY* pwhitepoint)
{
	cmsHPROFILE hProfile = 0;

	cmsToneCurve* tonecurve = cmsGetToneCurve(gamm);

	hProfile = cmsCreateProfile(iColorSpace, tonecurve, pprimaries, pwhitepoint);

	if (tonecurve) cmsFreeToneCurve(tonecurve);

	return hProfile;
}

void pseudoinverse(double(*in)[3], double(*out)[3], size_t size)
{
	double work[3][6], num;
	size_t i, j, k;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 6; j++)
			work[i][j] = static_cast<double>(j == i + 3);
		for (j = 0; j < 3; j++)
			for (k = 0; k < size; k++)
				work[i][j] += in[k][i] * in[k][j];
	}
	for (i = 0; i < 3; i++) {
		num = work[i][i];
		for (j = 0; j < 6; j++)
			work[i][j] /= num;
		for (k = 0; k < 3; k++) {
			if (k == i) continue;
			num = work[k][i];
			for (j = 0; j < 6; j++)
				work[k][j] -= work[i][j] * num;
		}
	}
	for (i = 0; i < size; i++)
		for (j = 0; j < 3; j++)
			for (out[i][j] = static_cast<double>(k = 0); k < 3; k++)
				out[i][j] += work[j][k + 3] * in[i][k];
}

void CreateChromatisities(const float(*mtx)[3], cmsCIExyYTRIPLE* pprimaries, cmsCIExyY* pwhitepoint)
{
	double invmatx[3][3], inverse[3][3], num;
	int i, j;
	for (i = 0; i < 3; i++) {			// Normalize cam_rgb so that
		for (num = j = 0; j < 3; j++)	// invmatx * (1,1,1) is (1,1,1,1)
			num += mtx[i][j];
		for (j = 0; j < 3; j++)
			invmatx[i][j] = mtx[i][j] / num;
	}
	pseudoinverse(invmatx, inverse, 3);
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			invmatx[i][j] = inverse[j][i];
	pprimaries->Red.x = invmatx[0][0] / (invmatx[0][0] + invmatx[1][0] + invmatx[2][0]);
	pprimaries->Red.y = invmatx[1][0] / (invmatx[0][0] + invmatx[1][0] + invmatx[2][0]);
	pprimaries->Red.Y = 1.0;
	pprimaries->Green.x = invmatx[0][1] / (invmatx[0][1] + invmatx[1][1] + invmatx[2][1]);
	pprimaries->Green.y = invmatx[1][1] / (invmatx[0][1] + invmatx[1][1] + invmatx[2][1]);
	pprimaries->Green.Y = 1.0;
	pprimaries->Blue.x = invmatx[0][2] / (invmatx[0][2] + invmatx[1][2] + invmatx[2][2]);
	pprimaries->Blue.y = invmatx[1][2] / (invmatx[0][2] + invmatx[1][2] + invmatx[2][2]);
	pprimaries->Blue.Y = 1.0;

	pwhitepoint->x = (invmatx[0][0] + invmatx[0][1] + invmatx[0][2]) / (invmatx[0][0] + invmatx[0][1] + invmatx[0][2] + invmatx[1][0] + invmatx[1][1] + invmatx[1][2] + invmatx[2][0] + invmatx[2][1] + invmatx[2][2]);
	pwhitepoint->y = (invmatx[1][0] + invmatx[1][1] + invmatx[1][2]) / (invmatx[0][0] + invmatx[0][1] + invmatx[0][2] + invmatx[1][0] + invmatx[1][1] + invmatx[1][2] + invmatx[2][0] + invmatx[2][1] + invmatx[2][2]);
	pwhitepoint->Y = 1.0;
}*/

bool TransformColor(void* buff, const size_t width, const size_t height, const int bps, const cmsHPROFILE hInProfile, const cmsHPROFILE hOutProfile)
{
	bool result = false;

	size_t stride;
	stride = width * static_cast<size_t>(3 * bps / 8);
	if (stride & 3) stride += SIZEOFDWORD - stride & 3; // DWORD aligned
#ifndef TYPE_XYZ_8
#define TYPE_XYZ_8            (COLORSPACE_SH(PT_XYZ)|CHANNELS_SH(3)|BYTES_SH(1))
#endif
#ifndef TYPE_XYZ_16
#define TYPE_XYZ_16            (COLORSPACE_SH(PT_XYZ)|CHANNELS_SH(3)|BYTES_SH(2))
#endif

	cmsUInt32Number inputFormat = (bps > 8 ? TYPE_RGB_16 : TYPE_RGB_8);
	cmsUInt32Number outputFormat = (bps > 8 ? TYPE_RGB_16 : TYPE_RGB_8);
	if (cmsHTRANSFORM hTransform = cmsCreateTransform(hInProfile, inputFormat, hOutProfile, outputFormat, INTENT_PERCEPTUAL /*INTENT_RELATIVE_COLORIMETRIC*/, cmsFLAGS_NOCACHE | cmsFLAGS_BLACKPOINTCOMPENSATION))
	{
		for (size_t n = 0; n < height; ++n)
			cmsDoTransform(hTransform, &static_cast<unsigned char*>(buff)[stride * n], &static_cast<unsigned char*>(buff)[stride * n], static_cast<cmsUInt32Number>(width));
		result = true;
		cmsDeleteTransform(hTransform);
	}

	return result;
}

bool TransformToColor(void* buff, size_t width, size_t height, int bps, cmsHPROFILE hInProfile, char* szProfile)
{
	bool result = false;
	cmsHPROFILE hOutProfile = 0;
	if (szProfile)
		hOutProfile = cmsOpenProfileFromFile(szProfile, "r");
	else
		hOutProfile = cmsCreate_sRGBProfile();
	if (hOutProfile)
	{
		result = TransformColor(buff, width, height, bps, hInProfile, hOutProfile);
		cmsCloseProfile(hOutProfile);
	}

	return result;
}

bool TransformToMonitorColor(void* buff, size_t width, size_t height, int bps, void* pProfile, char* szMonitorProfile)
{
	cmsHPROFILE hProfile = 0;
	if (pProfile)
	{
		cmsUInt32Number len = ntohl(*static_cast<unsigned int*>(pProfile));
		hProfile = cmsOpenProfileFromMem(pProfile, len);
	}
	else hProfile = cmsCreate_sRGBProfile();
	if (hProfile) 
		return TransformToColor(buff, width, height, bps, hProfile, szMonitorProfile);
	return false;
}

// Преобразовать буфер из исходного цветового пространства в ЦП монитора
/*bool TransformToMonitorColor(void* buff, const size_t width, const size_t height, const int bps, const char* szMonitorProfile, const int iColorSpace, const double* gamm, const float(*mtx)[3], const char* szProfile)
{
	bool result = false;
	// не пытаться конвертировать из sRGB в sRGB
	if (!szMonitorProfile && iColorSpace == 1) return result;
	cmsHPROFILE hOutProfile = 0;
	cmsHPROFILE hInProfile = 0;
	if (szMonitorProfile)
		hOutProfile = cmsOpenProfileFromFile(szMonitorProfile, "r");
	else
		hOutProfile = cmsCreate_sRGBProfile();
	if (hOutProfile)
	{
		// fill structures for lab conversions
		cmsCIExyYTRIPLE raw_primaries;
		cmsCIExyY raw_whitepoint;
		if (iColorSpace == 0)
		{
			assert(mtx);
			CreateChromatisities(mtx, &raw_primaries, &raw_whitepoint);
			hInProfile = cmsCreateProfile(iColorSpace, gamm, &raw_primaries, &raw_whitepoint);
		}
		else if (iColorSpace > 0)
		{
			hInProfile = cmsCreateProfile(iColorSpace, gamm, nullptr, nullptr);
		}
		else if (szProfile)
		{
			hInProfile = cmsOpenProfileFromFile(szProfile, "r");
		}
		if (hInProfile)
		{
			result = TransformColor(buff, width, height, bps, hInProfile, hOutProfile);

			cmsCloseProfile(hInProfile);
		}
		cmsCloseProfile(hOutProfile);
	}

	return result;
}*/

std::string GetMonitorProfile(HWND hWnd)
{
#if defined WIN32 || defined WIN64
	// определяем только под Windows
	if (HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL))
	{
		MONITORINFOEX monInfoEx = {};
		monInfoEx.cbSize = sizeof(MONITORINFOEX);
		if (GetMonitorInfo(hMon, &monInfoEx))
		{
			if (HDC hDc = CreateDC(0, monInfoEx.szDevice, 0, 0))
			{
				TCHAR szProfileName[MAX_PATH] = {};
				DWORD pLen = MAX_PATH;
				GetICMProfile(hDc, &pLen, szProfileName);
				DeleteDC(hDc);
#ifdef _UNICODE
				using convert_type = std::codecvt_utf8<wchar_t>;
				std::wstring_convert<convert_type, wchar_t> converter;
				return converter.to_bytes(szProfileName);
#else
				return std::string(szProfileName);
#endif
			}
		}
	}
#endif
	return std::string();
}
