//
// etapVersion.c - Print version info
//

#include "etap.h"
#include "etapVersion.h"
#include "versionlib.h"

TAPCommand(TAP_Version)
{
    TAP_Printf("%s\n", VARIANT_NAME);
    TAP_Printf("LPM: %s\n", VersionGetVersionLong());
}
