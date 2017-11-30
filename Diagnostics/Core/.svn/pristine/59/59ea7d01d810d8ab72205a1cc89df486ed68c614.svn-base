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
#include "EventDefinitions.h"
#include "stdio.h"

#ifdef INCLUDE_EVENT_STRINGS
static const char* const eventStrings[] =
{
    FOREACH_CORE_EVENT(GENERATE_STRING)
    FOREACH_VARIANT_EVENT(GENERATE_STRING)
};

const char* GetEventString(MESSAGE_ID_t id)
{
    if (id > NUM_EVENTS)
    {
        return "Invalid event";
    }
    return eventStrings[id];
}

MESSAGE_ID_t GetEventFromString(const char* eventString)
{
    for (MESSAGE_ID_t i = (MESSAGE_ID_t) 0; i < NUM_EVENTS; i++)
    {
        if (!strcasecmp(eventString, GetEventString(i)))
        {
            return i;
        }
    }
    return INVALID_EVENT;
}

#else
#define NUM_EVENT_STRING_BUFFERS 4
static char eventStrings[NUM_EVENT_STRING_BUFFERS][4];
static uint8_t i = 0;

const char* GetEventString(MESSAGE_ID_t id)
{
    if (id > NUM_EVENTS)
    {
        return "Invalid event";
    }
    sprintf(eventStrings[i], "%d", id);
    i = (i + 1) % NUM_EVENT_STRING_BUFFERS;
    return eventStrings[id];
}

MESSAGE_ID_t GetEventFromString(const char* eventString)
{
    return INVALID_EVENT;
}

#endif
