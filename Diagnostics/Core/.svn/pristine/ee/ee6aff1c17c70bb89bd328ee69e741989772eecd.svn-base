#include "unifySourceList.h"
#include "sourceInterface.h"

/* Logs
===============================================================================*/
SCRIBE_DECL(uitask);

//***USED FOR DISPLAYING SOURCE NAME IN TAP LOGS ONLY***//

#define GENERATE_STRING(STRING) #STRING,

#if defined(BARDEEN) || defined(GINGER) || defined(PROFESSOR)
static const char* const DEBUG_unify_source_name[] =
{
    FOREACH_SOURCE(GENERATE_STRING)
};
#endif

/*
===============================================================================
@func UITask_GetUnifySrcName
@brief returns name of unify source - FOR DEBUG ONLY !!!
===============================================================================
*/

const char* UITask_GetUnifySrcName(SOURCE_ID id)
{
#if defined(BARDEEN) || defined(GINGER) || defined(PROFESSOR)
  if(id < NUM_SOURCES)
  {
    return DEBUG_unify_source_name[id];
  }
  else
  {
    return DEBUG_unify_source_name[INVALID_SOURCE];
  }
#else
  return "";
#endif
}

  

/*
===============================================================================
@func UI_GetUnifySourceIndex
@brief returns unify index for source
===============================================================================
*/
UnifySourceIndex UI_GetUnifySourceIndex(SOURCE_ID id)
{
  UnifySourceIndex i=0;
  
  uint8_t maxUnifySources = UI_GetTotalNumSources();
  for(i=0; i<maxUnifySources ; i++)
  { 
    if (UITask_GetUnifySourceStruct(i)->sourceID == id)
    {
      return i;
    }
  }

  LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Unify source not found for id %d!", id);
  return (UnifySourceIndex)INVALID_SOURCE_LIST_INDEX;
}  

/*
===============================================================================
@func UI_GetSourceStructForSource
@brief returns pointer to source struct associated with source id
===============================================================================
*/
const UnifySourceStruct* UI_GetUnifyStructForSource(SOURCE_ID id)
{
  UnifySourceIndex unifyID= UI_GetUnifySourceIndex(id);
  
  if (unifyID<UI_GetTotalNumSources())
  {
    return UITask_GetUnifySourceStruct(unifyID);
  }

  LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Undefined source struct requested for id %d!", id);
  return NULL;
}  

/*
===============================================================================
@func UI_GetNextSourceStruct
@brief returns pointer to source struct associated with next source id
===============================================================================
*/
const UnifySourceStruct* UI_GetNextUnifyStruct(SOURCE_ID id)
{
  UnifySourceIndex unifyID = UI_GetUnifySourceIndex(id);

  if ((UI_GetTotalNumSources()>1) && (unifyID < UI_GetTotalNumSources()))
  {
    unifyID++;
    if (unifyID == UI_GetTotalNumSources())
    {
      unifyID = 0;
    }
    return UITask_GetUnifySourceStruct(unifyID);	
  }

  LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Undefined source struct requested for id %d!", id);
  return NULL;
}
