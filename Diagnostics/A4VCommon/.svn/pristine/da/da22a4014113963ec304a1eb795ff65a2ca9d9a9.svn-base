
#include "project.h"

#include "WiredIDUtilities.h"
#include "WiredIDConsoleTask.h"
#include "WiredIDTask.h"
#include "WiredIDConsoleBase.h"

SCRIBE_DECL(wired_id);

void WiredIDConsoleBase_HandleMessage(GENERIC_MSG_t *msg)
{
   switch (msg->msgID)
   {
   //If BASS_DISABLE is received in any state, go WAITRETRY   
   case WIREDID_CONSOLE_MESSAGE_ID_BASS_DISABLE:            
       WiredIDConsole_GoToState(WIRED_ID_CONSOLE_STATE_WAITRETRY);
       break;
   default:
       break;
   }
}
