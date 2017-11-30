/*************************************************************************//**
 * @file            versionlib.c
 * @brief           Version data library routines.
 *
 * @date            $Date: 2012-02-03 12:08:15 -0500 (Fri, 03 Feb 2012) $
 * @author          $Author: cv13058 $
 * @version         $Id: versionlib.c 1022 2012-02-03 17:08:15Z cv13058 $
 *
 * Description ***************************************************************
 *
 * Brian Lingard, Alfred von Campe and Chris Vincent
 *
 * Revs:
 *   1.0   2011-11-14  Initial release
 *   1.1   2011-12-19  Change a bunch.  A lot.
 *
 *****************************************************************************
 *      Copyright Bose Corporation, Framingham, MA
 *****************************************************************************/

/*** Include Files ***********************************************************/
#ifndef VERSION_INTERNAL_STRLEN
    #include <string.h>
#endif

#ifndef VERSION_INTERNAL_ATOI
    #include <stdlib.h>
#endif
#include <limits.h>  // defines INT_MAX

#include "versionlib.h"

#define LIMIT_VERSION_H_INCLUDES
#include "version.h"

/*** External Function Prototypes ********************************************/

/*** External Variable References ********************************************/

/*** Module Macros ***********************************************************/
#ifndef NULL
    #define NULL ((void *) 0)
#endif

#ifndef TRUE
    #define TRUE (1 == 1)
#endif

#ifndef FALSE
    #define FALSE (1 == 0)
#endif

#ifdef VERSION_INTERNAL_ATOI
    #define atoi versionlib_internalatoi
#endif

#ifdef VERSION_INTERNAL_STRLEN
    #define strlen versionlib_internalstrlen
#endif

/*** Module Types ************************************************************/

/*** Module Function Prototypes **********************************************/
#ifdef VERSION_SUPPORT_COMPARE
  static int isnum( const char s );
  static int compareIntegers(int a, int b);
  static int countNonExtendedDots(const char *version);
  static const char* getField( const char *version, int field );
  #ifdef VERSION_INTERNAL_ATOI
  static unsigned int versionlib_internalatoi( const char *string );
  #endif
  #ifdef VERSION_INTERNAL_STRLEN
  static unsigned int versionlib_internalstrlen( const char *string );
  #endif
#endif //VERSION_SUPPORT_COMPARE

/*** Module Variables ********************************************************/

/*** Module Functions ********************************************************/

/*  implement required functions
 */

/**
 * Get the major software revision number
 *
 * @return Pointer to a string containing the major revision number
 */
const char *VersionGetMajor( void )
{
    return(VERSION_MAJOR);
}

/**
 * Get the minor software revision number
 *
 * @return Pointer to a string containing the minor revision number
 */
const char *VersionGetMinor( void )
{
    return(VERSION_MINOR);
}

/**
 * Get the software patch revision number
 *
 * @return Pointer to a string containing the patch number.
 */
const char *VersionGetPatch( void )
{
    return(VERSION_PATCH);
}


/*  implement optional functions
 */

#ifdef VERSION_SUPPORT_BUILD_NUMBER
/**
 * Get the software build number.  This is formatted XX[M] where XX is the build number
 * optionally followed by "M" indicating local modifications.
 *
 * @return Pointer to a string contianing the build number.
 */
const char *VersionGetBuildNumber( void )
{
    return(VERSION_BUILD_NUMBER);
}
#endif

#ifdef VERSION_SUPPORT_EXTENDED_DATA
/**
 * Get the name of the user that built the software.
 *
 * @return Pointer to a string containing the user name.
 */
const char *VersionGetBuildDeveloper(void)
{
    return(VERSION_BUILD_DEVELOPER);
}

/**
 * Get the name of the computer used to build the software.
 *
 * @return Pointer to a string containing the build machine's name.
 */
const char *VersionGetBuildMachine(void)
{
    return(VERSION_BUILD_MACHINE);
}

/**
 * Get the time of the software build.  Format YYYY-MM-DDTHH:MM:SS.
 *
 * @return Pointer to a string with the time of the build.
 */
const char *VersionGetBuildTime(void)
{
    return(VERSION_BUILD_TIME);
}
#endif

#ifdef VERSION_SUPPORT_COPYRIGHT
/**
 * Get the copyright string.
 *
 * @return Pointer to a string containing the software copyright info.
 */
const char *VersionGetSoftwareCopyright(void)
{
    return(SOFTWARE_COPYRIGHT);
}
#endif

#ifdef VERSION_SUPPORT_BUFFERS
/**
 * Get the short version string.  Contains major, minor and patch.
 *
 * MAJOR.MINOR.PATCH
 *
 * @return Pointer to a string with the short version information.
 */
const char *VersionGetVersionShort(void)
{
    return(versionShort);
}

/**
 * Get the version string.  Contains major, minor and patch and build revision.
 *
 * MAJOR.MINOR.PATCH[.BUILD[M]]
 *
 * @return Pointer to a string with the version information.
 */
const char *VersionGetVersion(void)
{
  #ifdef VERSION_SUPPORT_BUILD_NUMBER
    return(version);
  #else
    return(VersionGetVersionShort());
  #endif
}

/**
 * Get the verbose version information.  Contains major, minor and patch, build
 * revision, build user, build machine and build time.
 *
 * MAJOR.MINOR.PATCH[.REV[:REV2][M]][ USER.MACHINE.TIME]
 *                                   ^-- note the space here!
 *
 * @return Pointer to a string containing the verbose version information.
 */
const char *VersionGetVersionLong(void)
{
  #ifdef VERSION_SUPPORT_EXTENDED_DATA
    return(versionLong);
  #else
    return(VersionGetVersion());
  #endif
}
#endif

#ifdef VERSION_SUPPORT_COMPARE
/**
 * Compare two integers in our context.
 *
 * @param a First integer
 * @param b Second integer
 *
 * @return Result based on the equality.  VERSION_COMPARE_(DOWNGRADE|UPGRADE|SAME_VERSION).
 */
static int compareIntegers(int a, int b)
{
    if (a > b)
        return(VERSION_COMPARE_DOWNGRADE);
    else if (a < b)
        return(VERSION_COMPARE_UPGRADE);
    else
        return(VERSION_COMPARE_SAME_VERSION);
}

/**
 * Count the number of dots in the non extended version information.
 *
 * @return Number of dots (fields - 1) in the version information.
 */
static int countNonExtendedDots(const char *version)
{
    int i;
    int count = 0;

    for (i = 0; (version[i] != '\0') && (version[i] != ' '); i++)
    {
        if (version[i] == '.')
            count++;
    }

    return(count);
}

/**
 * Get a pointer to the start of a particular field in the version information.
 *
 * @param version Pointer to  a string that has the version information
 * @param field The field number to find.  Starts at zero.
 *
 * @return Pointer to the start of the field within the param 'version'.
 */
static const char* getField( const char *version, int field )
{
    const char *retVal = NULL;
    int count = 0;
    int i;

    if( version != NULL )
    {
        for (i = 0; version[i] != '\0'; i++)
        {
            if( count == field )
            {
                retVal = &version[i];
                break;
            }

            if( (version[i] == '.') || (version[i] == ' ') )
            {
                count++;
            }
        }
    }

    return retVal;
}

#ifdef VERSION_INTERNAL_ATOI
/**
 * Convert a string into an unsigned 32 bit number
 *
 * @author cv13058
 *
 * @param string A decimal or hex formatted (starts with 0x) number
 */
static unsigned int versionlib_internalatoi( const char *string )
{
    unsigned int retVal = 0;
    int parsing = TRUE;
    int hex = FALSE;
    char i;

    // check for hex
    if( strlen(string) > 2 )
    {
        if( string[1] == 'x' )
        {
            string += 2;
            hex = TRUE;
        }
    }

    while( (*string != '\0') && parsing )
    {
        i = *string++;

        if( hex )
        {
            if( (i >= 'a') && (i <= 'f') )
            {
                // convert to uppercase
                i -= 0x20;
            }

            if( (i >= 'A') && (i <= 'F') )
            {
                // convert to decimal
                i -= 0x37;
                retVal = (retVal << 4) + i;
                continue;
            }
        }

        if( (i >= '0') && (i <= '9') )
        {
            if( hex )
            {
                retVal = (retVal << 4) + (i-0x30);
            }
            else
            {
                retVal = (retVal * 10) + (i-0x30);
            }
        }
        else
        {
            // Not captured by the hex or by digits...see ya!
            parsing = FALSE;
        }
    }

    return retVal;
}
#endif //VERSION_INTERNAL_ATOI

#ifdef VERSION_INTERNAL_STRLEN
/**
 * Get the length of a string.
 *
 * @param string The string to get the length of.
 *
 * @return the Length of the string.  0 could also indicate NULL argument.
 */
static unsigned int versionlib_internalstrlen( const char *string )
{
    int retVal = 0;

    if( string != NULL )
    {
        // Note: use INT_MAX here to be more processor independent
        while( (*string++ != '\0') && (retVal < INT_MAX) )
        {
            retVal++;
        }
    }

    return retVal;
}
#endif //VERSION_INTERNAL_STRLEN

/**
 * Determine if a character is a number
 *
 * @param s The character to examine.
 *
 * @return TRUE if the character is a number, FALSE otherwise.
 */
static int isnum( const char s )
{
    return (s >= '0') && (s <= '9');
}

/**
 * Compare two version strings.
 *
 * @param v1 The first version string
 * @param v2 The second version string
 *
 * @return A status indicating if v1 relative to v2 is an invalid comparison, upgrade, downgrade or is the same version.
 *         VERSION_COMPARE_* values are available in versionlib.h.
 */
int VersionCompareVersions( const char *v1, const char *v2 )
{
    int result = 0;
    int currentField;
    int maxField;

    // Get the number of fields to compare from the first version string
    maxField = countNonExtendedDots( v1 );

    // If there aren't the same number of comparable fields, bomb out.
    if( maxField != countNonExtendedDots(v2) )
    {
        result = VERSION_COMPARE_INVALID_COMPARE;
    }

    // Loop through until we hit the maximum field count or we run into the build rev block
    for( currentField = 0; (currentField <= maxField) ; currentField++ )
    {
        const char *fieldLeft, *fieldRight;
        int fieldLeftVal, fieldRightVal;

        fieldLeft = getField( v1, currentField );
        fieldRight = getField( v2, currentField );

        // This is a terminal error and also ensures that the minimum dot count is checked
        if( (fieldLeft == NULL) || (fieldRight == NULL) )
        {
            result = VERSION_COMPARE_INVALID_COMPARE;
            break;
        }

        // If it isn't an integer, bomb out.
        if( !isnum(*fieldLeft) || !isnum(*fieldRight) )
        {
            result = VERSION_COMPARE_INVALID_COMPARE;
        }

        fieldLeftVal = atoi( fieldLeft );
        fieldRightVal = atoi( fieldRight );

        // Do a compare of the field value if we don't already have a result
        if( result == VERSION_COMPARE_SAME_VERSION )
        {
           result = compareIntegers( fieldLeftVal, fieldRightVal );
        }
    }

    // If we got through the whole string but they are the same, check for possible 'M' flag at the end of the field
    if( (result == VERSION_COMPARE_SAME_VERSION) )
    {
        const char *v1Rev = getField( v1, currentField - 1 );
        const char *v2Rev = getField( v2, currentField - 1 );

        int v1Mod = FALSE, v2Mod = FALSE;

        if( (v1Rev == NULL) || (v2Rev == NULL) )
        {
            result = VERSION_COMPARE_INVALID_COMPARE;
        }
        else
        {
            while( (*v1Rev != ' ') && (*v1Rev != '.') && (*v1Rev != '\0') )
            {
                if( (*v1Rev == 'M') || (*v1Rev == ':') )
                {
                    v1Mod = TRUE;
                }

                v1Rev++;
            }

            while( (*v2Rev != ' ') && (*v2Rev != '.') && (*v2Rev != '\0') )
            {
                if( (*v2Rev == 'M') || (*v2Rev == ':') )
                {
                    v2Mod = TRUE;
                }

                v2Rev++;
            }

            // If they aren't the same, modified indicates a newer revision
            if( v1Mod != v2Mod )
            {
                result = v1Mod ? VERSION_COMPARE_DOWNGRADE : VERSION_COMPARE_UPGRADE;
            }
        }
    }

    return(result);
}
#endif //VERSION_SUPPORT_COMPARE

/* end of versionlib.c */
