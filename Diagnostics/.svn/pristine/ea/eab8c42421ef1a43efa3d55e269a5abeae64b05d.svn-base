/**
 * @file UIHelper.h
 * @author lb1032816
 * @date 6/2/2017
 * @brief Product specific UI handling for professor
*/

#ifndef UNIHANDLER_H
#define UNIHANDLER_H

#include "RivieraLPM_IpcProtocol.h"

/**
 * @brief Product specific UI message handler
 * 
 * Invoked for the Core UITask. This function allows the product to handle any message 
 * sent to the UI task before the UITask's message handler. If the message cannot be 
 * handled by the product then the UITask will be responsible for fielding the message.
 * 
 * @param message: Incoming message
 * @return (BOOL): 0 if message can be handled, 1 otherwise (use UITask's handler)
*/
BOOL UIHelper_HandleMessage(GENERIC_MSG_t* message);

#endif //UNIHANDLER_H