//
// BoseCRC.c - Calculate a CRC.
//

#include "project.h"
#include "BCRC.h"
#include "stm32f2xx_crc.h"

// length is number of words (32-bit)
uint32_t BCRC_CalculateCRC(void* data, unsigned int length)
{
    debug_assert(data);
    CRC_ResetDR();
    return CRC_CalcBlockCRC((uint32_t*) data, length);
}
