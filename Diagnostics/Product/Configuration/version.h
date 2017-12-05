// This file was generated with gen_version.pl at build time -- DO NOT EDIT BY HAND

#ifdef LIMIT_VERSION_H_INCLUDES
/*
 * This file should only be included by the version library.  If you want
 * version information get it from there...  The reason for this is to
 * avoid having to rebuild lots of code every time the version.h file
 * is recreated.
 */
#define VERSION_MAJOR "0"
#define VERSION_MINOR "2"
#define VERSION_PATCH "0"

#define VERSION_BUILD_NUMBER "1344M"
#define VERSION_BUILD_BRANCHNAME ""
#define VERSION_BUILD_DEVELOPER "jr1007965"
#define VERSION_BUILD_TYPE "Developer"
#define VERSION_BUILD_MACHINE "usma-6nddxw1"
#define VERSION_BUILD_TIME "2017-12-05T10:09:56"

#define SOFTWARE_COPYRIGHT "Copyright 2017 Bose Corporation"

#define BUFFER_FOR_PLACEMENT        versionShort

#define VERSION_BUFFER_SHORT    VERSION_MAJOR "." VERSION_MINOR "." VERSION_PATCH

#ifdef VERSION_SUPPORT_BUILD_NUMBER
    #define VERSION_BUFFER          VERSION_BUFFER_SHORT "." VERSION_BUILD_NUMBER
    #undef BUFFER_FOR_PLACEMENT
    #define BUFFER_FOR_PLACEMENT    version
#else
    #define VERSION_BUFFER          VERSION_BUFFER_SHORT
#endif

#ifdef VERSION_SUPPORT_EXTENDED_DATA
    #define VERSION_BUFFER_LONG     VERSION_BUFFER " " VERSION_BUILD_BRANCHNAME " (" VERSION_BUILD_TYPE ") " VERSION_BUILD_TIME
    #undef BUFFER_FOR_PLACEMENT
    #define BUFFER_FOR_PLACEMENT    versionLong
#else
    #define VERSION_BUFFER_LONG     VERSION_BUFFER
#endif

#ifdef VERSION_SUPPORT_BUFFERS
static const char versionShort[] = VERSION_BUFFER_SHORT;

  #ifdef VERSION_SUPPORT_BUILD_NUMBER
  static const char version[] = VERSION_BUFFER;
  #endif

  #ifdef VERSION_SUPPORT_EXTENDED_DATA
  static const char versionLong[] = VERSION_BUFFER_LONG;
  #endif
#endif

#ifdef __TMS320C6X__
#pragma DATA_SECTION( BUFFER_FOR_PLACEMENT, "VERSION" );
#endif

#else  // LIMIT_VERSION_H_INCLUDES
#error Please do not include this file.  Access version information through versionlib.h only.
#endif // LIMIT_VERSION_H_INCLUDES
