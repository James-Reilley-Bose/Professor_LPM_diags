/*****************************************************************************
 *
 *  Version data library header.
 *
 *  File:   versionlib.h
 *
 *  Author: Brian Lingard & Alfred von Campe
 *
 *  Revs:
 *    1.0   2011-11-14  Initial release
 *
 *****************************************************************************/

#ifndef _VERSIONLIB_H
#define _VERSIONLIB_H
/*
 *  Define feature controls
 *  - comment out defines for features that you don't want in your project
 */
#define VERSION_SUPPORT_BUILD_NUMBER
#define VERSION_SUPPORT_EXTENDED_DATA
#define VERSION_SUPPORT_COPYRIGHT
#define VERSION_SUPPORT_BUFFERS
#define VERSION_SUPPORT_COMPARE

#ifdef VERSION_SUPPORT_COMPARE
    #define VERSION_INTERNAL_ATOI
    #define VERSION_INTERNAL_STRLEN
#endif

#define VERSION_COMPARE_INVALID_COMPARE   -2
#define VERSION_COMPARE_UPGRADE           -1
#define VERSION_COMPARE_SAME_VERSION      0
#define VERSION_COMPARE_DOWNGRADE         1

/*  declare prototypes for all functions
 */

#ifdef  __cplusplus
extern "C" {
#endif

extern const char *VersionGetMajor( void );
extern const char *VersionGetMinor( void );
extern const char *VersionGetPatch( void );

#ifdef VERSION_SUPPORT_BUILD_NUMBER
extern const char *VersionGetBuildNumber( void );
#endif

#ifdef VERSION_SUPPORT_EXTENDED_DATA
extern const char *VersionGetBuildDeveloper( void );
extern const char *VersionGetBuildMachine( void );
extern const char *VersionGetBuildTime( void );
#endif

#ifdef VERSION_SUPPORT_COPYRIGHT
extern const char *VersionGetSoftwareCopyright( void );
#endif

#ifdef VERSION_SUPPORT_BUFFERS
extern const char *VersionGetVersionShort( void );
extern const char *VersionGetVersion( void );
extern const char *VersionGetVersionLong( void );
#endif

#ifdef VERSION_SUPPORT_COMPARE
extern int VersionCompareVersions(const char *v1, const char *v2);
#endif

#ifdef  __cplusplus
}
#endif

#endif  /* _VERSIONLIB_H */
