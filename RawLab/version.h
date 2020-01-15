#ifndef APP_DEFINES_H
#define APP_DEFINES_H

#define MAJOR_VER        0
#define MINOR_VER        6
#define RELEASE_VER      4
#define BUILD_VER        33
#define APP_VER          MAJOR_VER,MINOR_VER,RELEASE_VER,BUILD_VER
#define VER_STR_(VER)    #VER
#define VER_STR(VER)     VER_STR_(VER) "\0"

#define  ORIGFILENAME    "rawlab.exe"
#define  APPNAME         "RawLab"
#define  FILEDESCRIPTION "RAW Laboratory"
#define  LEGALCOPYRIGHT  "2017-2020, Konstantin A. Pankov"
#define  PRODUCTSUPPORT  "admin@kapankov.ru"

#endif
