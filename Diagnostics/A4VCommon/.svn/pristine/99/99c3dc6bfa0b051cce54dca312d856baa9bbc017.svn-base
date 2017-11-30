/*
 * General functionality for controlling CC2640 board.
 * Exposed API is here
*/

#ifndef REMOTE_DRIVER_H
#define REMOTE_DRIVER_H


// Reset CC2640 using reset pin
void Remote_AssertReset(BOOL assert);
void Remote_HardReset(void);
void Remote_SoftReset(void);
void Remote_PrepForUpdate(void);
BOOL Remote_GetVersion(char* buf, uint8_t len); // copy into buffer
BOOL Remote_GetStatus(Remote_RxPktData_Status_t* status, uint16_t msTimeout);
BOOL Remote_StartScan(uint8_t* result, uint8_t secTimeout);
BOOL Remote_ClearPairing(uint8_t* result, uint16_t msTimeout);

#endif