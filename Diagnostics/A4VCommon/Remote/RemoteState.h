#ifndef REMOTE_STATE_H
#define REMOTE_STATE_H

#include "project.h"

// todo: move non-funcitons to RemoteDefs?

typedef void (*Remote_EnterStateFunction)(void);
typedef void (*Remote_ExitStateFunction)(void);
typedef void (*Remote_RunFunction)(void);
typedef void (*Remote_HandleMessageFunction)(GENERIC_MSG_t*);


// Note that these must line up with the array indices of the states array!
typedef enum
{
    REMOTE_STATE_RUNNING = 0,
    REMOTE_STATE_UPDATING = 1,
    REMOTE_STATE_ERROR = 2
} Remote_StateID_t;


typedef struct
{
    Remote_StateID_t id;
    Remote_EnterStateFunction enterState;
    Remote_ExitStateFunction exitState;
    Remote_RunFunction run;
    Remote_HandleMessageFunction handleMessage;
} Remote_State_t;



// todo: if this isn't exposed, can probably remove the Remote_ prefix
extern Remote_State_t Remote_states[];
extern Remote_State_t* Remote_currentState;


void Remote_changeState(Remote_StateID_t nextState);


#endif