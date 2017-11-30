#ifndef VIEW_NOTIFY_H
#define VIEW_NOTIFY_H

#include "IpcProtocolLpm.h"
#include "TaskManager.h"

// UI Notification events (max 32)
#define SOURCE_CHANGE_START        (1<<0)
#define SOURCE_CHANGE_FINISH       (1<<1)
#define LAYER_STATE_CHANGE         (1<<2)
#define USER_VOLUME_CHANGE         (1<<3)
#define AUDIO_PARAMETER_CHANGE     (1<<4)
#define NETWORK_STATUS_CHANGE      (1<<5)
#define BLUETOOTH_STATUS_CHANGE    (1<<6)
#define SYSTEM_TIMEOUT             (1<<7)
#define SYSTEM_ERROR               (1<<8)
#define DSP_ENERGY_PRESENT         (1<<9)
#define LLW_STATUS_CHANGE          (1<<10)
#define USER_MUTE_CHANGE           (1<<11)
#define KEY_STATUS_CHANGE          (1<<12)
#define VID_OUT_RES_CHANGE         (1<<13)
#define UPDATE_STATUS_CHANGE       (1<<14)
#define LANGUAGE_CHANGE            (1<<15)
#define SOURCE_LIST_CHANGE         (1<<16)
#define POWER_STATE_CHANGE         (1<<17)
#define SM2_BOOTED                 (1<<18)
#define ASOC_BOOTED                (1<<19)
#define CEC_MODE_CHANGED           (1<<20)
#define SPEAKER_CONFIG_CHANGED     (1<<21)
#define PARTY_MODE_CHANGED         (1<<22)

#define NOTIFY_ALL                 0xFFFFFFFF

typedef void (*NotificationFuncPtr)(uint32_t notificationEvent);

typedef struct ui_notify_entry_t
{
    NotificationFuncPtr   notificationFunc;
    ManagedTask*          notificationTask;
    MESSAGE_ID_t          notificationMsg;
    uint32_t              notifyEventMask;
} UIViewNotifyEntry;

BOOL UI_ViewFunctionRegistration(NotificationFuncPtr notify, uint32_t events);
BOOL UI_ViewTaskRegistration(ManagedTask* task, MESSAGE_ID_t message, uint32_t events);
void UI_ViewDeregistration(NotificationFuncPtr notify);
void UI_ViewUpdateNotifyEvent(NotificationFuncPtr notify, uint32_t events);
uint32_t UI_ViewGetCurrEventMask(NotificationFuncPtr notify);
void UI_ViewNotify(uint32_t notifyEvent);

#endif //VIEW_NOTIFY_H
