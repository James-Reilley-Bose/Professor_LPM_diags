//
// UpdateVariant.h
//

#ifndef UPDATE_VARIANT_H
#define UPDATE_VARIANT_H

#include "project.h"
#include "UpdateTypes.h"
#include "EnumGenerator.h"
#include "product_ext_flash_map.h"

#define BLOB_ADDRESS EXT_FLASH_ADDR_UPDATE_STORAGE_START
#define MAX_BLOB_LENGTH EXT_FLASH_UPDATE_STORAGE_LENGTH

#define FOREACH_UPDATESTATE(STATE) \
  /* Not in update */ \
  STATE(UPDATE_INACTIVE) \
  \
  /* Waiting for the image to be downloaded */ \
  STATE(UPDATE_WAIT_DOWNLOAD) \
  \
  /* Update is currently downloading from SM2 */ \
  STATE(UPDATE_DOWNLOADING) \
  \
  /* Authenticate the downloaded image*/ \
  STATE(UPDATE_AUTHENTICATE) \
  \
  /* Wait for the installation command from SM2 */ \
  STATE(UPDATE_WAIT_INSTALL) \
  \
  /* Begin the Update */ \
  STATE(UPDATE_BEGIN) \
  \
  /* Install the DSP update */ \
  STATE(UPDATE_DO_DSP) \
  \
  /* Update the WAM */ \
  STATE(UPDATE_DO_WAM) \
  \
  /* Update the F0*/ \
  STATE(UPDATE_DO_F0) \
  \
  /* Update the BleRadio (remote)*/ \
  STATE(UPDATE_DO_BLE_RADIO) \
  \
  /* set the LPM update bootloader flag */ \
  STATE(UPDATE_DO_MICRO) \
  \
  STATE(UPDATE_SM2_WAIT_EXT_PERIPH) \
  \
  /* Reboot */ \
  STATE(UPDATE_REBOOT) \
  \
  /* Number of Update States */ \
  STATE(UPDATE_NUM_STATES)

typedef enum
{
    FOREACH_UPDATESTATE(GENERATE_ENUM)
} UpdateBaseState_t;

void UpdateVariant_Init(void);
BOOL UpdateVariant_IsUpdateAllowed(void);
void UpdateVariant_FlashWrite(unsigned int address, const void* data, unsigned int length);
void UpdateVariant_FlashRead(unsigned int address, void* data, unsigned int length);
void UpdateVariant_FlashErase(unsigned int startAddress, unsigned int endAddress);


extern const UpdateStateHandler_t UpdateStateHandlers[];

#endif // UPDATE_VARIANT_H
