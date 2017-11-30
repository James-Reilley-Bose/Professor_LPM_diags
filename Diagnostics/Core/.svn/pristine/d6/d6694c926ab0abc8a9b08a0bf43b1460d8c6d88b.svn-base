#include "project.h"
#include "UITask.h"
#include "TaskDefs.h"
#include "TaskManager.h"
#include "SourceChangeTask.h"
#include "SourceRecipe.h"
#include "nv_system.h"
#include "UIHelper.h"
#include "ProductSystemParams.h"

#if defined(LPM_HAS_SPI_FLASH)
#include "nvram.h"
#endif


/* Logs
===============================================================================*/
SCRIBE(uitask, ROTTEN_LOGLEVEL_NORMAL);
SCRIBE_DECL(demo);

/* Global Variable
===============================================================================*/
ManagedTask* uiTaskHandle = NULL;

#if 0 // TODO - do we still need this
static UnifySourceIndex currentUnifySourceIndex = 0;
#endif

#if 0 // DO WE WANT KEYHOOK
/// @brief From Triode - we want to be able to echo specific keys to tap, ignore specific keys (via tap), and I have no idea what ECHO_HOLD_IGNORE is
// There are 256 possible key values, so to make them into a bitmask we need 8 uint32_ts
static uint32_t KHHookMap[KH_NUM_HOOKMAPS][KH_MASK_WORDS];

#define KH_KEYMASK_ENTRY(key) (7 - (key/32))
#define KH_KEYMASK_BIT(key) ( 1 << (key % 32) )
#define KH_TAPWantsToEcho(key) ( KHHookMap[KH_HOOK_MAP_ECHO]  [KH_KEYMASK_ENTRY(key)] & KH_KEYMASK_BIT(key) )
#define KH_TAPWantsToIgnore(key)  ( KHHookMap[KH_HOOK_MAP_IGNORE][KH_KEYMASK_ENTRY(key)] & KH_KEYMASK_BIT(key) )
#endif // DO WE WANT KEYHOOK

/* Local Prototypes
===============================================================================*/

/* Local Variables
===============================================================================*/

#define KEY_CADENCE_ERROR 2


/*
===============================================================================
@func UITask
@brief
===============================================================================
*/
void UITask(void* pvParamaters)
{

    vTaskPrioritySet(uiTaskHandle->TaskHandle, configNORMAL_TASK_PRIORITY);
#if 0 // TODO - do we need these still
    SystemBehavior_UIInit();
    memset(KHHookMap, 0, sizeof(KHHookMap));


#ifdef HAS_SYSPARAMS_NVRAM
    SystemBehavior_UIColdBoot(NV_GetFirstTimeInit() == TRUE);
#else
    SystemBehavior_UIColdBoot(FALSE);
#endif
#endif

    for (;;)
    {
        TaskManagerPollQueue(uiTaskHandle);
    }
}

/*
===============================================================================
@func UI_TaskInit
@brief
===============================================================================
*/
void UI_TaskInit(void* p)
{
    uiTaskHandle = GetManagedTaskPointer("UITask");
    debug_assert (NULL != uiTaskHandle);
}

/*
===============================================================================
@func UI_SetUnifySource
@brief Set the source based on the Unify source list index
       returns false if the index was invalid or if the source change is not allowed
===============================================================================
*/
#if 0 // TODO - bring in, delete, or move
BOOL UI_SetUnifySource(UnifySourceIndex index)
{
    if ((index >= UI_GetTotalNumSources()) ||
            (INVALID_SOURCE_LIST_INDEX == index))
    {
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Bad src idx: %d", index);
        return FALSE;
    }
    else if (!IsValidForVariant(UITask_GetUnifySourceStruct(index)->sourceID))
    {
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "UI src invalid: %d", index);
        return FALSE;
    }

    SOURCE_ID reqSource = UITask_GetUnifySourceStruct(index)->sourceID;
    SOURCE_ID curSource = SourceChangeAPI_GetCurrentSource();

#if defined(BARDEEN) || defined(GINGER)
    if (!Hospitality_IsSourceChangeAllowed(reqSource))
    {
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Src change %d -> %d not allowed in hospitality.", curSource, reqSource);
        return FALSE;
    }
#endif

    if (!GetSourceInterface(curSource)->isSourceChangeAllowed(reqSource))
    {
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Src change %d -> %d not allowed.", curSource, reqSource);
        return FALSE;
    }

#ifdef HAS_SYSPARAMS_NVRAM
    // Don't remember sources like update or standby.
    if (UI_IsValidLastSource(SourceChangeAPI_GetCurrentSource()) && (reqSource != curSource))
    {
        NV_SetLastSource(SourceChangeAPI_GetCurrentSource());
    }
#endif

    UIPostMsg(UI_MESSAGE_ID_RequestSourceChange, NOP_CALLBACK, index);

    return TRUE;
}
#endif

#if 0 // TODO - bring in, delete, or move
/*
===============================================================================
@func UI_HandleSourceChangeRequest
@brief
===============================================================================
*/
void UI_HandleSourceChangeRequest(UnifySourceIndex index)
{
    debug_assert(index < UI_GetTotalNumSources());
    const UnifySourceStruct* unifyStruct = UITask_GetUnifySourceStruct(index);
    LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "source change to: %s", UITask_GetUnifySrcName(unifyStruct->sourceID));
    currentUnifySourceIndex = index;
    SourceChangeAPI_SetCurrentSource(unifyStruct->sourceID);
    SystemTimeout_Reset(TIMEOUT_ALL);
}

/*
===============================================================================
@func UI_GetCurrentUnifySourceIndex
@brief
===============================================================================
*/
UnifySourceIndex UI_GetCurrentUnifySourceIndex()
{
    return currentUnifySourceIndex;
}
#endif

/*
===============================================================================
@func UI_HandleMessage
@brief
===============================================================================
*/
void UI_HandleMessage(GENERIC_MSG_t* message)
{
    if(UIHelper_HandleMessage(message)) //Attempt to handle the message in the product first
    {
        switch (message->msgID)
        {
            default:
                LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "\tUI\t%s invl msg", GetEventString(message->msgID));
                break;
        }
    }
}

/*
============================================================================================
@func UI_GetLastSource
@brief Get last source based on out of box and cold boot rules.
============================================================================================
*/
SOURCE_ID UI_GetLastSource(void)
{
#if 0 // TODO - do we need this?
    if (NV_GetFirstTimeInit())
    {
        return SystemBehavior_GetDefaultSource();
    }
    else
    {
        return (SOURCE_ID)NV_GetLastSource();
    }
#endif
    return (SOURCE_ID) NV_GetLastSource();
}

#if 0
void UI_HookMapClearAll( KH_HOOK_MAP_ID mapid )
{
    /// @note this only ever gets called by tap, who cares if there's a race condition
    debug_assert(mapid < KH_NUM_HOOKMAPS);
    for (int i = 0; i < KH_MASK_WORDS; i++)
    {
        KHHookMap[mapid][i] = 0;
    }
}

void UI_HookMapSetAll(KH_HOOK_MAP_ID mapid)
{
    /// @note this only ever gets called by tap, who cares if there's a race condition
    debug_assert( mapid < KH_NUM_HOOKMAPS );
    for (int i = 0; i < KH_MASK_WORDS; i++)
    {
        KHHookMap[mapid][i] = 0xffffffff;
    }
}

void UI_SetKeyHook(KH_HOOK_MAP_ID mapid, KEY_VALUE key)
{
    /// @note this only ever gets called by tap, who cares if there's a race condition
    KHHookMap[mapid][KH_KEYMASK_ENTRY(key)] |=  KH_KEYMASK_BIT(key);
}

void UI_ClearKeyHook(KH_HOOK_MAP_ID mapid, KEY_VALUE key)
{
    /// @note this only ever gets called by tap, who cares if there's a race condition
    KHHookMap[mapid][KH_KEYMASK_ENTRY(key)] &= ~KH_KEYMASK_BIT(key);
}

/// @brief Get the bitmap of the keys currently being echoed/ignored
/// @param OUT keyMask an array of KH_MASK_WORDS uint32s
void UI_GetKeyMask(KH_HOOK_MAP_ID mapid, uint32_t* keyMask)
{
    /// @note this only ever gets called by tap, who cares if there's a race condition
    memcpy(keyMask, KHHookMap[mapid], sizeof(KHHookMap[mapid]));
}
#endif
