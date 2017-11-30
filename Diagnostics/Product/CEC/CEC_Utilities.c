#include "project.h"
#include "CEC_Utilities.h"
#include "CECTask.h"
#include "IpcRouterTask.h"
#include "CEC_ControlInterface.h"
#include <stdio.h>

SCRIBE_DECL(cec);

static const char* const cecOpcodeStrings[] =
{
    FOR_EACH_CEC_OPCODE(GENERATE_OP_STRING)
};

static const char* const cecDeviceStrings[] =
{
    "TV",
    "RECORDER_1" ,
    "RECORDER_2" ,
    "TUNER_1" ,
    "PLAYBACK_1" ,
    "AUDIO" ,
    "TUNER_2" ,
    "TUNER_3" ,
    "PLAYBACK_2" ,
    "RECORDER_3" ,
    "TUNER_4" ,
    "PLAYBACK_3" ,
    "",
    "",
    "FREE_USE",
    "UNREG_BCAST",
    "NUM_DEVICES"
};


/*
===============================================================================
@fn CEC_SetLogicalAddress
@brief Mutator for current logical address
===============================================================================
*/
void CEC_SetLogicalAddress(uint8_t addr)
{
    debug_assert(addr <= CEC_DEVICE_NUM_DEVICES);

    cecState.logicalAddress = addr;
}

/*
===============================================================================
@fn CEC_GetLogicalAddress
@brief Accessor for current logical address
===============================================================================
*/
uint8_t CEC_GetLogicalAddress(void)
{
    return cecState.logicalAddress;
}


/*
===============================================================================
@func CEC_SetPhysicalAddress
@brief
===============================================================================
*/
void CEC_SetPhysicalAddress(uint16_t newAddress)
{
    cecState.physicalAddress = newAddress;
    LOG(cec, ROTTEN_LOGLEVEL_VERBOSE, "New Physical Address from EDID: %X", CEC_GetPhysicalAddress());
}


/*
===============================================================================
@func CEC_GetPhysicalAddress
@brief
===============================================================================
*/
uint16_t CEC_GetPhysicalAddress(void)
{
    return cecState.physicalAddress;
}

/*
===============================================================================
@fn CEC_GetDeviceListRemoveEntry
@brief
===============================================================================
*/
void CEC_DeviceListRemoveEntry(uint8_t logical)
{
    debug_assert(logical < CEC_DEVICE_NUM_DEVICES);

    CEC_SetDeviceListPhysicalAddress(logical, DEVICE_DISABLED_PHYSICAL_ADDRESS);
    CEC_SetDeviceListOSD(logical, "", 0);
    CEC_SetDeviceListCECVersion(logical, CEC_VERSION_UNKNOWN);
    CEC_SetDeviceListVendorID(logical, 0xFF, 0xFF, 0xFF);
}

/*
===============================================================================
@fn CEC_GetDeviceListPhysicalAddress
@brief
===============================================================================
*/
uint16_t CEC_GetDeviceListPhysicalAddress(uint8_t logical)
{
    debug_assert(logical < CEC_DEVICE_NUM_DEVICES);

    return cecState.cec_devices[logical].physicalAddress;
}

/*
===============================================================================
@fn CEC_SetDeviceListPhysicalAddress
@brief Save logical and physical address from CEC report.
===============================================================================
*/
void CEC_SetDeviceListPhysicalAddress(uint8_t logical, uint16_t physical)
{
    if (logical < CEC_DEVICE_NUM_DEVICES)
    {
        cecState.cec_devices[logical].physicalAddress = physical;
    }
}

/*
===============================================================================
@fn    CEC_GetDeviceListLogicalAddressFromPort
@brief  Provides the logical address of the device at the specified port.
        If no CEC device is available at the port, CEC_DEVICE_DISABLED is
        returned
===============================================================================
*/
uint8_t CEC_GetDeviceListLogicalAddress(hdmi_port port)
{
    debug_assert(HDMI_PORT_START <= port && port < HDMI_PORT_NONE);

    uint16_t phyAddr = CEC_BuildPhysicalAddressForPort(port);
    for (uint8_t i = 0; i < CEC_DEVICE_NUM_DEVICES; ++i)
    {
        if (phyAddr == cecState.cec_devices[i].physicalAddress)
        {
            return i;
        }
    }
    return CEC_DEVICE_DISABLED;
}

/*
===============================================================================
@fn  CEC_SetDeviceListOSD
@brief When receiving an OSD name from the network, save it off.
===============================================================================
*/
void CEC_SetDeviceListOSD(uint8_t logical, const char* osd, uint8_t length)
{
    debug_assert(logical < CEC_DEVICE_NUM_DEVICES);
    debug_assert(length <= CEC_MAX_OSD_LEN);

    memcpy(cecState.cec_devices[logical].osd, osd, length);
    cecState.cec_devices[logical].osd[length] = '\0';
}

/*
===============================================================================
@fn  CEC_GetDeviceListOSD
@brief Given a logical address, return the OSD name if we have it.
===============================================================================
*/
uint8_t* CEC_GetDeviceListOSD(uint8_t logical)
{
    debug_assert(logical < CEC_DEVICE_NUM_DEVICES);

    return cecState.cec_devices[logical].osd;
}

/*
===============================================================================
@fn CEC_SetDeviceListVendorID
@brief Record this devices IEEE OUI / Vendor ID
===============================================================================
*/
void CEC_SetDeviceListVendorID(uint8_t logical, uint8_t vendorID0, uint8_t vendorID1, uint8_t vendorID2)
{
    debug_assert(logical < CEC_DEVICE_NUM_DEVICES);

    cecState.cec_devices[logical].vendorID[0] = vendorID0;
    cecState.cec_devices[logical].vendorID[1] = vendorID1;
    cecState.cec_devices[logical].vendorID[2] = vendorID2;
}

/*
===============================================================================
@fn CEC_GetDeviceListVendorID
@brief Given a logical addres, return the device's Vendor ID/IEEE OUI
===============================================================================
*/
uint8_t* CEC_GetDeviceListVendorID(uint8_t logical)
{
    debug_assert(logical < CEC_DEVICE_NUM_DEVICES);

    return cecState.cec_devices[logical].vendorID;
}

/*
===============================================================================
@fn CEC_GetDeviceListVendorID32BitWord
@brief returns the vendor ID in a single 32-bit word.
===============================================================================
*/
uint32_t CEC_GetDeviceListVendorID32BitWord(uint8_t logical)
{
    debug_assert(logical < CEC_DEVICE_NUM_DEVICES);

    return (cecState.cec_devices[logical].vendorID[0] << 16) | (cecState.cec_devices[logical].vendorID[1] << 8) | cecState.cec_devices[logical].vendorID[2];
}

/*
===============================================================================
@fn  CEC_SetDeviceListCECVersion
@brief Save of CEC version report
===============================================================================
*/
void CEC_SetDeviceListCECVersion(uint8_t logical, CEC_VERSION version)
{
    debug_assert(logical < CEC_DEVICE_NUM_DEVICES);

    cecState.cec_devices[logical].cecVersion = version;
}

/*
===============================================================================
@fn  CEC_GetDeviceListCECVersion
@brief Given a logical address, get its CEC version.
===============================================================================
*/
CEC_VERSION CEC_GetDeviceListCECVersion(uint8_t logical)
{
    debug_assert(logical < CEC_DEVICE_NUM_DEVICES);

    return cecState.cec_devices[logical].cecVersion;
}

/*
===============================================================================
@func CEC_PrintToLog
@brief Dump CEC messages to the log in a readable format.
===============================================================================
*/
void CEC_PrintToLog(CEC_t* cecMsg)
{
    LOG(cec, ROTTEN_LOGLEVEL_VERBOSE, "%s->%s : %s",
        cecDeviceStrings[cecMsg->initiator],
        cecDeviceStrings[cecMsg->destination],
        cecOpcodeStrings[cecMsg->payload.opcode]);

    uint32_t* msg = (uint32_t*)cecMsg;
    LOG(cec, ROTTEN_LOGLEVEL_INSANE, "%08lX %08lX %08lX %08lX",
        REV_WORD_BYTES(msg[0]), REV_WORD_BYTES(msg[1]),
        REV_WORD_BYTES(msg[2]), REV_WORD_BYTES(msg[3]));
}

/*
===============================================================================
@fn CEC_BuildPhysicalAddressForPort
@brief Convert a port into a source physical address. This is used to populate the
       addresses when we get a new physical address. Otherwise use the cec_devices
       array to get the physical addresses.

    // In order to be able to increment a CEC address, it needs to have at least
    // one zero at the end. Therefore, we can only increment addresses of the form:
    // X.0.0.0
    // X.Y.0.0
    // X.Y.Z.0
    // Otherwise we have to assign the address F.F.F.F to the RX ports.
===============================================================================
*/
uint16_t CEC_BuildPhysicalAddressForPort(hdmi_port port)
{
    debug_assert(HDMI_PORT_START <= port && port < HDMI_PORT_NONE);

    uint8_t a_addr[4] = {0, 0, 0, 0};
    CEC_MakePhyAddrArray(CEC_GetPhysicalAddress(), a_addr);

    if (TRUE == CEC_CanIncrementPhysicalAddress(CEC_GetPhysicalAddress()))
    {
        // Find the first zero and insert the port number.
        for (uint8_t i = 1; i < 4; ++i)
        {
            if (a_addr[i - 1] != 0 && a_addr[i] == 0)
            {
                a_addr[i] = port;
                break;
            }
        }

        // collapse into uint16_t
        uint16_t address = 0;
        for (uint8_t i = 0; i < 4; ++i)
        {
            address = (address << 4);
            address += a_addr[i];
        }

        return address;
    }
    else
    {
        return DEVICE_DISABLED_PHYSICAL_ADDRESS;
    }
}

/*
===============================================================================
@fn CEC_MakePhyAddrArray
@brief Takes a uint8_t array and dumps our physical address in by nibble
       ex: phy:  1. 2. 3. 0
         array: [1][2][3][0]
===============================================================================
*/
void CEC_MakePhyAddrArray(uint16_t addr, uint8_t a_addr[])
{
    // You could loop this, but this makes it more easy to follow.
    // Split up each nibble into a byte.
    a_addr[0] = (addr & 0xF000) >> 12;
    a_addr[1] = (addr & 0x0F00) >> 8;
    a_addr[2] = (addr & 0x00F0) >> 4;
    a_addr[3] = (addr & 0x000F);
}

/*
===============================================================================
@fn  CEC_GetPortFromPhyAddress
@brief If the address is UPSTREAM, get the port to which it is connected.
===============================================================================
*/
hdmi_port CEC_GetPortFromPhyAddress(uint16_t addr)
{
    if (FALSE == CEC_IsUpstreamDevice(CEC_GetPhysicalAddress(), addr) ||  FALSE == CEC_IsValidAddress(addr))
    {
        return HDMI_PORT_NONE;
    }
    else
    {
        uint8_t a_addr_us[4] = {0, 0, 0, 0};
        uint8_t a_addr_them[4] = {0, 0, 0, 0};
        CEC_MakePhyAddrArray(CEC_GetPhysicalAddress(), a_addr_us);
        CEC_MakePhyAddrArray(addr, a_addr_them);

        // Find the first zero from left and return the port number.
        for (uint8_t i = 1; i < 4; ++i)
        {
            if (a_addr_us[i - 1] != 0 && a_addr_us[i] == 0)
            {
                return (hdmi_port)a_addr_them[i];
            }
        }
    }

    return HDMI_PORT_NONE;
}

/*
===============================================================================
@fn CEC_IsValidAddress

@brief Check validity of an HDMI physical address (probably an operand for a CEC message)
===============================================================================
*/
BOOL CEC_IsValidAddress(uint16_t address)
{
    uint8_t a_addr[4] = {0, 0, 0, 0};
    CEC_MakePhyAddrArray(address, a_addr);

    for (uint8_t i = 0; i < 4; ++i)
    {
        a_addr[i] = (BOOL)(a_addr[i] != 0);
    }
    // Valid CEC addresses cannot have a zero between digits, they must be of the form:
    // X.0.0.0
    // X.Y.0.0
    // X.Y.Z.0
    // X.Y.Z.W
    if ((!a_addr[0] && !a_addr[1] && !a_addr[2] && !a_addr[3]) ||
            (a_addr[0] && !a_addr[1] && !a_addr[2] && !a_addr[3]) ||
            (a_addr[0] &&  a_addr[1] && !a_addr[2] && !a_addr[3]) ||
            (a_addr[0] &&  a_addr[1] &&  a_addr[2] && !a_addr[3]) ||
            (a_addr[0] &&  a_addr[1] &&  a_addr[2] &&  a_addr[3]))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CEC_CanIncrementPhysicalAddress(uint16_t address)
{
    uint8_t a_addr[4] = {0, 0, 0, 0};
    CEC_MakePhyAddrArray(address, a_addr);

    for (uint8_t i = 0; i < 4; ++i)
    {
        a_addr[i] = (BOOL)(a_addr[i] != 0);
    }

    // In order to be able to increment a CEC address, it needs to have at least
    // one zero at the end. Therefore, we can only increment addresses of the form:
    // X.0.0.0
    // X.Y.0.0
    // X.Y.Z.0
    // Otherwise we have to assign the address F.F.F.F to the RX ports.
    if ((!a_addr[0] && !a_addr[1] && !a_addr[2] && !a_addr[3]) ||
            (a_addr[0] && !a_addr[1] && !a_addr[2] && !a_addr[3]) ||
            (a_addr[0] &&  a_addr[1] && !a_addr[2] && !a_addr[3]) ||
            (a_addr[0] &&  a_addr[1] &&  a_addr[2] && !a_addr[3]))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL isValidCECMode(uint8_t mode)
{
    NV_WaitForInitialization();
    return CEC_MODE_DISABLED  == mode ||
           CEC_MODE_COMPLIANCE == mode ||
           CEC_MODE_USABILITY_1 == mode ||
           CEC_MODE_USABILITY_2 == mode ||
           CEC_MODE_USABILITY_3 == mode;
}
