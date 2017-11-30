/*
  File  : EventDefinitions.c
  Title :
  Author  : bw11737
  Created : 11/23/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:  Reserve space for event name strings.  Event names can included
  conditionally using the define INCLUDE_EVENT_STRINGS
==============================================================================*/

#include "project.h"
#include "UpdateManagerStateDefinitions.h"
#include "stdio.h"

#ifdef INCLUDE_UPDATE_MANAGER_STRINGS

static const char* const updateManagerStateStrings[] =
{
    FOREACH_UPDATESTATE(GENERATE_STRING)
};

static const char* updateManagerResultStrings[] =
{
    FOREACH_UPDATERESULT(GENERATE_STRING)
};

const char* GetUpdateManagerStateString(UpdateBaseState_t id)
{
    if (id >= UPDATE_NUM_STATES)
    {
        return "";
    }
    return updateManagerStateStrings[id];
}

UpdateBaseState_t GetUpdateManagerStateFromString(const char* updateManagerStateString)
{
    for (UpdateBaseState_t i = (UpdateBaseState_t) 0; i < UPDATE_NUM_STATES; i++)
    {
        if (!strcasecmp(updateManagerStateString, GetUpdateManagerStateString(i)))
        {
            return i;
        }
    }
    return INVALID_UPDATE_STATE;
}

const char* GetUpdateManagerResultString(UpdateResult_t id)
{
    if (id >= UPDATE_NUM_RESULTS)
    {
        return "";
    }
    return updateManagerResultStrings[id];
}

UpdateResult_t GetUpdateManagerResultFromString(const char* updateManagerResultString)
{
    for (UpdateResult_t i = (UpdateResult_t) 0; i < UPDATE_NUM_RESULTS; i++)
    {
        if (!strcasecmp(updateManagerResultString, GetUpdateManagerResultString(i)))
        {
            return i;
        }
    }
    return INVALID_UPDATE_RESULT;
}

#else

#define NUM_STRING_BUFFERS 4
static char updateManagerStringBuffers[NUM_STRING_BUFFERS][3];
static uint8_t i = 0;

const char* GetUpdateManagerStateString(UpdateBaseState_t id)
{
    if (id >= UPDATE_NUM_STATES)
    {
        return "";
    }
    sprintf(updateManagerStringBuffers[i], "%d", id);
    i = (i + 1) % NUM_STRING_BUFFERS;
    return updateManagerStringBuffers[id];
}

UpdateBaseState_t GetUpdateManagerStateFromString(const char* updateManagerStateString)
{
    return INVALID_UPDATE_STATE;
}

const char* GetUpdateManagerResultString(UpdateResult_t id)
{
    if (id >= UPDATE_NUM_RESULTS)
    {
        return "";
    }
    sprintf(updateManagerStringBuffers[i], "%d", id);
    i = (i + 1) % NUM_STRING_BUFFERS;
    return updateManagerStringBuffers[id];
}

UpdateResult_t GetUpdateManagerResultFromString(const char* updateManagerResultString)
{
    return INVALID_UPDATE_RESULT;
}

#endif
