#ifndef RAWLAB_CMS_H
#define RAWLAB_CMS_H

// Color Management System

cmsHPROFILE cmsCreate_AdobeRGBProfile();
void* getAdobeProfile();
void* getsRGBProfile();

bool TransformToMonitorColor(void* buff, size_t width, size_t height, int bps, void* pProfile, char* szMonitorProfile);

std::string GetMonitorProfile(HWND hWnd);

#endif