#ifndef CEC_UTILITIES_H
#define CEC_UTILITIES_H

#include "project.h"
#include "CECTask.h"

BOOL CEC_IsValidAddress(uint16_t address);
CEC_VERSION CEC_GetDeviceListCECVersion(uint8_t logical);
hdmi_port CEC_GetPortFromPhyAddress(uint16_t addr);
uint16_t CEC_BuildPhysicalAddressForPort(hdmi_port port);
BOOL CEC_CanIncrementPhysicalAddress(uint16_t address);
uint16_t CEC_GetDeviceListPhysicalAddress(uint8_t logical);
uint16_t CEC_GetPhysicalAddress(void);
uint8_t CEC_GetLogicalAddress(void);
uint8_t* CEC_GetDeviceListOSD(uint8_t logical);
uint8_t* CEC_GetDeviceListVendorID(uint8_t logical);
void CEC_MakePhyAddrArray(uint16_t addr, uint8_t a_addr[]);
void CEC_PrintToLog(CEC_t* cecMsg);
void CEC_SetDeviceListCECVersion(uint8_t logical, CEC_VERSION version);
void CEC_SetDeviceListOSD(uint8_t logical, const char* osd, uint8_t length);
void CEC_SetDeviceListPhysicalAddress(uint8_t logical, uint16_t physical);
void CEC_SetDeviceListVendorID(uint8_t logical, uint8_t vendorID0, uint8_t vendorID1, uint8_t vendorID2);
void CEC_SetLogicalAddress(uint8_t addr);
void CEC_SetPhysicalAddress(uint16_t newAddress);
uint8_t CEC_GetDeviceListLogicalAddress(hdmi_port port);
uint32_t CEC_GetDeviceListVendorID32BitWord(uint8_t logical);
void CEC_DeviceListRemoveEntry(uint8_t logical);
BOOL isValidCECMode(uint8_t mode);
#endif
