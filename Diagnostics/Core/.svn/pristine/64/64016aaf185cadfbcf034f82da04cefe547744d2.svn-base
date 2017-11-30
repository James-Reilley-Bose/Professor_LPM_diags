/*
 * TapStore implements Rotten_Store as a
 * connection to etap.
 * Brought to you from the keyboard of Nate Bragg
 */
#include <stdio.h>
#include "tapstore.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "etap.h"

/*
 * Procedure prototypes
 */
#define TAP_SCRATCH_BUFFER_SIZE (128)
static void tap_store_write(TapStore *this, Rotten_Record *r);
static void tap_store_dump(TapStore *from, Rotten_Store *to);
static void tap_store_flush(TapStore *this);
static char tap_scratch[TAP_SCRATCH_BUFFER_SIZE];

/*
 * Public interface
 */
void tap_store_init(TapStore *this)
{
    this->base.write = (void(*)(Rotten_Store*,Rotten_Record*))tap_store_write;
    this->base.dump = (void(*)(Rotten_Store*,Rotten_Store*))tap_store_dump;
    this->base.flush = (void(*)(Rotten_Store*))tap_store_flush;
}

/*
 * Implementation
 */
static void tap_store_write(TapStore *this, Rotten_Record *r)
{
    char frame_start[TAP_STORE_TERM_FRAME_LENGTH] = "";
    char frame_end[TAP_STORE_TERM_FRAME_LENGTH] = "";
    Rotten_Logger *l = rotten();
    Rotten_StoreEnable en = ROTTEN_STORE_DISABLED;

    if (l != NULL)
    {
        (void)rotten_storeenable_get(l, "tap", &en);
        if (en == ROTTEN_STORE_FRAMED)
        {
            strncpy(frame_start, TAP_STORE_FRAME_START_STR, sizeof(frame_start));
            strncpy(frame_end, TAP_STORE_FRAME_END_STR, sizeof(frame_end));
        }
    }

    if(!(r->msg->userdata&TAP_STORE_NO_ANNOTATION || r->msg->scribe->userdata == TAP_STORE_NO_ANNOTATION)) {
        TAP_Printf("%s[%010u] [%s - %s][%s] %s", frame_start, r->time, r->msg->scribe->name, r->task, rotten_loglevel_print((Rotten_LogLevel)r->msg->level), frame_end);
    }

    char crlf[3] = "";
    if(!(r->msg->userdata&TAP_STORE_NO_CRLF || r->msg->scribe->userdata == TAP_STORE_NO_CRLF)) {
        strncpy(crlf, "\r\n", sizeof(crlf));
    }

    snprintf(tap_scratch, TAP_SCRATCH_BUFFER_SIZE, "%s%s%s%s",
            frame_start, r->msg->text, frame_end, crlf);

    TAP_Printf(tap_scratch, r->params[0], r->params[1], r->params[2], r->params[3], r->params[4], r->params[5], r->params[6], r->params[7], r->params[8], r->params[9], r->params[10], r->params[11]);


}

static void tap_store_dump(TapStore *from, Rotten_Store *to)
{
    //No-op.  What did you *think* could happen here?
}

static void tap_store_flush(TapStore *this)
{
    //NOP
}

