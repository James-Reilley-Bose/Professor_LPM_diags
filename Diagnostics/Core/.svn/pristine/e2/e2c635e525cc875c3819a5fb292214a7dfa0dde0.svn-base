#include "project.h"
#include "UITask.h"
#include "TaskDefs.h"
#include "TaskManager.h"
#include "SourceChangeTask.h"
#include "unifySourceList.h"
#include "SourceRecipe.h"
#include "SystemBehaviors.h"
#include "volumeModel.h"
#include "viewNotify.h"

/* Logs
===============================================================================*/
SCRIBE_DECL(uitask);

void TestViewNotify(uint32_t notificationEvent)
{
    //Should post a message to the View's Task Module
    if (notificationEvent & SOURCE_CHANGE_START)
    {
        //In task context, use access functions to get additional detail about what changed
        SOURCE_ID next = SourceChangeAPI_GetNextSource();
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Test View Notified: SOURCE_CHANGE_START: %s", UITask_GetUnifySrcName(next));
    }
    if (notificationEvent & SOURCE_CHANGE_FINISH)
    {
        //In task context, use access functions to get additional detail about what changed
        SOURCE_ID current = SourceChangeAPI_GetCurrentSource();
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Test View Notified: SOURCE_CHANGE_FINISH: %s", UITask_GetUnifySrcName(current));
    }
}

//This could expand to a task or have custom tap commands for ATG