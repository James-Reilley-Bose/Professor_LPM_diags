
#ifndef REMOTE_UTILITY_H
#define REMOTE_UTILITY_H

#include "project.h"

// Returns TRUE if a block is all zeros, FALSE otherwise
BOOL AllZero(void* ptr, int numBytes);
void printData(void* data, uint16_t len);
void Remote_ClearWaitingTasks();
#endif