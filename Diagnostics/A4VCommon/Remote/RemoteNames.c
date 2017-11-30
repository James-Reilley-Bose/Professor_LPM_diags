/*
 * Conversions from enum to string. Useful for debugging, may disable this if
 *  space is an issue
 *
*/

#include "RemoteDefs.h"
#ifdef USE_REMOTE_NAMES

#include "project.h"
#include "RemoteNames.h"


const char* remoteHIDNames[] = {
    "REMOTE_HIDTYPE_NONE",
    "REMOTE_HIDTYPE_UEI",
    "REMOTE_HIDTYPE_TI",
};
                                        
const char* remoteStateNames[] = {
    "REMOTE_STATE_IDLE",
    "REMOTE_STATE_WAIT_INIT_DONE",
    "REMOTE_STATE_LINKING",
    "REMOTE_STATE_SCAN_WAIT_HOST",
    "REMOTE_STATE_SCAN_SCANNING",
    "REMOTE_STATE_BONDED",
    "REMOTE_STATE_RUN",
    "REMOTE_STATE_ERR",
};

const char* remoteScanResultNames[] = {
    "REMOTE_NORESULT",
    "REMOTE_FOUND",
    "REMOTE_NOTFOUND",
};

const char* remoteGapStateNames[] = {
    "REMOTE_GAPSTATE_IDLE",
    "REMOTE_GAPSTATE_SCANNING",
    "REMOTE_GAPSTATE_FOUND1",
    "REMOTE_GAPSTATE_LINK_START",
    "REMOTE_GAPSTATE_LINK_ESTABLISHED",
    "REMOTE_GAPSTATE_LINK_PAIRING",
    "REMOTE_GAPSTATE_LINK_BONDED",
    "REMOTE_GAPSTATE_TERM_LINK",
};

const char* remoteGapScanResultNames[] = {
    "REMOTE_GAPSCAN_NORESULT",
    "REMOTE_GAPSCAN_FOUND",
    "REMOTE_GAPSCAN_STARTERR",
    "REMOTE_GAPSCAN_TIMEOUT",
    "REMOTE_GAPSCAN_LINKERR",
    "REMOTE_GAPSCAN_BONDERR",
};

const char* remoteGattStateNames[] = {
    "REMOTE_GATTSTATE_IDLE",
    "REMOTE_GATTSTATE_DISC_SVCS",
    "REMOTE_GATTSTATEE_DISC_CHARS",
    "REMOTE_GATTSTATE_DISC_DESCS",
    "REMOTE_GATTSTATE_DISC_HIDCHARS",
    "REMOTE_GATTSTATE_DISC_NOTIF",
    "REMOTE_GATTSTATE_RUN",
    "REMOTE_GATTSTATE_READ_CHAR",
    "REMOTE_GATTSTATE_WRITE_CHAR",
};

const char* remoteGattDiscoverResultNames[] = {
    "REMOTE_GATTRESULT_NORESULT",
    "REMOTE_GATTRESULT_DONE",
    "REMOTE_GATTRESULT_ERR",
    "REMOTE_GATTRESULT_SVCSTARTERR",
    "REMOTE_GATTRESULT_SVCCHKERR",
    "REMOTE_GATTRESULT_SVCCBERR",
    "REMOTE_GATTRESULT_CHARSTARTERR",
    "REMOTE_GATTRESULT_CHARCHKERR",
    "REMOTE_GATTRESULT_CHARCBERR",
    "REMOTE_GATTRESULT_DESCSTARTERR",
    "REMOTE_GATTRESULT_DESCCHKERR",
    "REMOTE_GATTRESULT_DESCCBERR",
    "REMOTE_GATTRESULT_RREFSTARTERR",
    "REMOTE_GATTRESULT_RREFCHKERR",
    "REMOTE_GATTRESULT_RREFCBERR",
    "REMOTE_GATTRESULT_NOTIFSTARTERR",
    "REMOTE_GATTRESULT_NOTIFCBERR",
};


const char* Remote_HIDName(uint8_t val)
{
    return remoteHIDNames[val];
}

const char* Remote_StateName(uint8_t val)
{
    return remoteStateNames[val];
}

const char* Remote_ScanResultName(uint8_t val)
{
    return remoteScanResultNames[val];
}

const char* Remote_GapStateName(uint8_t val)
{
    return remoteGapStateNames[val];
}

const char* Remote_GapScanResultName(uint8_t val)
{
    return remoteGapScanResultNames[val];
}

const char* Remote_GattStateName(uint8_t val)
{
    return remoteGattStateNames[val];
}

const char* Remote_GattDiscoverResultName(uint8_t val)
{
    return remoteGattDiscoverResultNames[val];
}






// Using NameEntry
/*
typedef struct
{
    uint8_t id;
    const char* name;
} NameEntry;

NameEntry remoteHIDNames[] = {
    {REMOTE_HIDTYPE_NONE,           "REMOTE_HIDTYPE_NONE"},
    {REMOTE_HIDTYPE_UEI,            "REMOTE_HIDTYPE_UEI"},
    {REMOTE_HIDTYPE_TI,             "REMOTE_HIDTYPE_TI"},
};
uint8_t remoteHIDNamesSize = sizeof(remoteHIDNames)/sizeof(NameEntry);
                                        
NameEntry remoteStateNames[] = {
    {REMOTE_STATE_IDLE,            "REMOTE_STATE_IDLE"},
    {REMOTE_STATE_WAIT_INIT_DONE,  "REMOTE_STATE_WAIT_INIT_DONE"},
    {REMOTE_STATE_LINKING,         "REMOTE_STATE_LINKING"},
    {REMOTE_STATE_SCAN_WAIT_HOST,  "REMOTE_STATE_SCAN_WAIT_HOST"},
    {REMOTE_STATE_SCAN_SCANNING,   "REMOTE_STATE_SCAN_SCANNING"},
    {REMOTE_STATE_BONDED,          "REMOTE_STATE_BONDED"},
    {REMOTE_STATE_RUN,             "REMOTE_STATE_RUN"},
    {REMOTE_STATE_ERR,             "REMOTE_STATE_ERR"},
};
uint8_t remoteStateNamesSize = sizeof(remoteHIDNames)/sizeof(NameEntry);

NameEntry remoteScanResultNames[] = {
    {REMOTE_NORESULT,         "REMOTE_NORESULT"},
    {REMOTE_FOUND,            "REMOTE_FOUND"},
    {REMOTE_NOTFOUND,         "REMOTE_NOTFOUND"},
};
uint8_t remoteScanResultNamesSize = sizeof(remoteHIDNames)/sizeof(NameEntry);

NameEntry remoteGapStateNames[] = {
    {REMOTE_GAPSTATE_IDLE,            "REMOTE_GAPSTATE_IDLE"},
    {REMOTE_GAPSTATE_SCANNING,        "REMOTE_GAPSTATE_SCANNING"},
    {REMOTE_GAPSTATE_FOUND1,          "REMOTE_GAPSTATE_FOUND1"},
    {REMOTE_GAPSTATE_LINK_START,      "REMOTE_GAPSTATE_LINK_START"},
    {REMOTE_GAPSTATE_LINK_ESTABLISHED,"REMOTE_GAPSTATE_LINK_ESTABLISHED"},
    {REMOTE_GAPSTATE_LINK_PAIRING,    "REMOTE_GAPSTATE_LINK_PAIRING"},
    {REMOTE_GAPSTATE_LINK_BONDED,     "REMOTE_GAPSTATE_LINK_BONDED"},
    {REMOTE_GAPSTATE_TERM_LINK,       "REMOTE_GAPSTATE_TERM_LINK"},
};
uint8_t remoteGapStateNamesSize = sizeof(remoteHIDNames)/sizeof(NameEntry);

NameEntry remoteGapScanResultNames[] = {
    {REMOTE_GAPSCAN_NORESULT,      "REMOTE_GAPSCAN_NORESULT"},
    {REMOTE_GAPSCAN_FOUND,         "REMOTE_GAPSCAN_FOUND"},
    {REMOTE_GAPSCAN_STARTERR,      "REMOTE_GAPSCAN_STARTERR"},
    {REMOTE_GAPSCAN_TIMEOUT,       "REMOTE_GAPSCAN_TIMEOUT"},
    {REMOTE_GAPSCAN_LINKERR,       "REMOTE_GAPSCAN_LINKERR"},
    {REMOTE_GAPSCAN_BONDERR,       "REMOTE_GAPSCAN_BONDERR"},
};
uint8_t remoteGapScanResultNamesSize = sizeof(remoteHIDNames)/sizeof(NameEntry);

NameEntry remoteGattStateNames[] = {
    {REMOTE_GATTSTATE_IDLE,           "REMOTE_GATTSTATE_IDLE"},
    {REMOTE_GATTSTATE_DISC_SVCS,      "REMOTE_GATTSTATE_DISC_SVCS"},
    {REMOTE_GATTSTATEE_DISC_CHARS,    "REMOTE_GATTSTATEE_DISC_CHARS"},
    {REMOTE_GATTSTATE_DISC_DESCS,     "REMOTE_GATTSTATE_DISC_DESCS"},
    {REMOTE_GATTSTATE_DISC_HIDCHARS,  "REMOTE_GATTSTATE_DISC_HIDCHARS"},
    {REMOTE_GATTSTATE_DISC_NOTIF,     "REMOTE_GATTSTATE_DISC_NOTIF"},
    {REMOTE_GATTSTATE_RUN,            "REMOTE_GATTSTATE_RUN"},
    {REMOTE_GATTSTATE_READ_CHAR,      "REMOTE_GATTSTATE_READ_CHAR"},
    {REMOTE_GATTSTATE_WRITE_CHAR,     "REMOTE_GATTSTATE_WRITE_CHAR"},
};
uint8_t remoteGattStateNamesSize = sizeof(remoteHIDNames)/sizeof(NameEntry);

NameEntry remoteGattDiscoverResultNames[] = {
    {REMOTE_GATTRESULT_NORESULT,      "REMOTE_GATTRESULT_NORESULT"},
    {REMOTE_GATTRESULT_DONE,          "REMOTE_GATTRESULT_DONE"},
    {REMOTE_GATTRESULT_ERR,           "REMOTE_GATTRESULT_ERR"},
    {REMOTE_GATTRESULT_SVCSTARTERR,   "REMOTE_GATTRESULT_SVCSTARTERR"},
    {REMOTE_GATTRESULT_SVCCHKERR,     "REMOTE_GATTRESULT_SVCCHKERR"},
    {REMOTE_GATTRESULT_SVCCBERR,      "REMOTE_GATTRESULT_SVCCBERR"},
    {REMOTE_GATTRESULT_CHARSTARTERR,  "REMOTE_GATTRESULT_CHARSTARTERR"},
    {REMOTE_GATTRESULT_CHARCHKERR,    "REMOTE_GATTRESULT_CHARCHKERR"},
    {REMOTE_GATTRESULT_CHARCBERR,     "REMOTE_GATTRESULT_CHARCBERR"},
    {REMOTE_GATTRESULT_DESCSTARTERR,  "REMOTE_GATTRESULT_DESCSTARTERR"},
    {REMOTE_GATTRESULT_DESCCHKERR,    "REMOTE_GATTRESULT_DESCCHKERR"},
    {REMOTE_GATTRESULT_DESCCBERR,     "REMOTE_GATTRESULT_DESCCBERR"},
    {REMOTE_GATTRESULT_RREFSTARTERR,  "REMOTE_GATTRESULT_RREFSTARTERR"},
    {REMOTE_GATTRESULT_RREFCHKERR,    "REMOTE_GATTRESULT_RREFCHKERR"},
    {REMOTE_GATTRESULT_RREFCBERR,     "REMOTE_GATTRESULT_RREFCBERR"},
    {REMOTE_GATTRESULT_NOTIFSTARTERR, "REMOTE_GATTRESULT_NOTIFSTARTERR"},
    {REMOTE_GATTRESULT_NOTIFCBERR,    "REMOTE_GATTRESULT_NOTIFCBERR"},
};
uint8_t remoteGattDiscoverResultNamesSize = sizeof(remoteHIDNames)/sizeof(NameEntry);


static const char* Remote_GetNameFromArray(NameEntry* arr, uint32_t arrSize, uint8_t id)
{
    int i=0;
    for(; i < arrSize; i++)
    {
        if(arr[i].id == id)
            return arr[i].name;
    }
    return REMOTE_NAME_ERROR;
}

const char* Remote_HIDName(Remote_HID_t val)
{
    return Remote_GetNameFromArray(remoteHIDNames, remoteHIDNamesSize, val);
}

const char* Remote_StateName(Remote_State_t val)
{
    return Remote_GetNameFromArray(remoteStateNames, remoteStateNamesSize, val);
}

const char* Remote_ScanResultName(Remote_ScanResult_t val)
{
    return Remote_GetNameFromArray(remoteScanResultNames, remoteScanResultNamesSize, val);
}

const char* Remote_GapStateName(Remote_GapState_t val)
{
    return Remote_GetNameFromArray(remoteGapStateNames, remoteGapStateNamesSize, val);
}

const char* Remote_GapScanResultName(Remote_GapScanResult_t val)
{
    return Remote_GetNameFromArray(remoteGapScanResultNames, remoteGapScanResultNamesSize, val);
}

const char* Remote_GattStateName(Remote_GattState_t val)
{
    return Remote_GetNameFromArray(remoteGattStateNames, remoteGattStateNamesSize, val);
}

const char* Remote_GattDiscoverResultName(Remote_GattDiscoverResult_t val)
{
    return Remote_GetNameFromArray(remoteGattDiscoverResultNames, remoteGattDiscoverResultNamesSize, val);
}
*/ // Using NameEntry

#endif