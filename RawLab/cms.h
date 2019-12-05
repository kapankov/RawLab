#ifndef RAWLAB_CMS_H
#define RAWLAB_CMS_H

// Color Management System

cmsHPROFILE cmsCreateAdobeRGBProfile();

bool TransformToMonitorColor(void* buff, size_t width, size_t height, int bps, cmsHPROFILE hInProfile, char* szMonitorProfile);

// Сконвертировать RGB буффер в цветовое пространство монитора
// buff - dword aligned rgb buffer
// width - image width
// height - image height
// bps - 8 or 16 bit
// szMonitorProfile - monitor profile file path
// если iColorSpace == 0, используется матрица mtx для построения исходного профиля + гамма gamm
// если iColorSpace == 1..6, используются встроенные матрицы для построения исходного профиля + гамма gamm
// если iColorSpace == -1, используется файл szProfile
bool TransformToMonitorColor(
	void* buff, 
	const size_t width, 
	const size_t height, 
	const int bps, 
	const char* szMonitorProfile = nullptr, 
	const int iColorSpace = 1, 
	const double* gamm = nullptr, 
	const float(*mtx)[3] = nullptr,
	const char* szProfile = nullptr);

std::string GetMonitorProfile(HWND hWnd);

#endif