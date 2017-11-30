#include "RemoteUtility.h"
#include "buffermanager.h"
#include "etap.h"

// Returns TRUE if a block is all zeros, FALSE otherwise
BOOL AllZero(void* ptr, int numBytes){
    for(uint16_t i = 0; i < numBytes; i++)
        if( ((uint8_t*)ptr)[i] ) return FALSE;
    return TRUE;
}

// I'm sorry for the TAP usage outside of TAP. It bothers me too.
void printData(void* data, uint16_t len)
{
    uint16_t byte = 0;
    for(; byte < len; byte += 1)
    {
        if(byte % 8 == 0)
        {
            TAP_Printf("\n0x%03x:  ", byte);
        }
        TAP_Printf("%02x ", ((uint8_t*)data)[byte]);
    }
    TAP_Printf("\n");
}

/*
 * Flags and wakes any semaphores that are waiting for a 2640 response. This
 *  should be called before doing a reset.
*/
void Remote_ClearWaitingTasks()
{
    // to
    // do
}

