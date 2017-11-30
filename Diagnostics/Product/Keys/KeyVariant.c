#include "project.h"
#include "KeyVariant.h"
#include "IpcKeys.h"
#include "DiagsDispatcher.h"

SCRIBE_DECL(keys);

void Product_HandleKey(KEY_DATA_t *key)
{
    LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "%s, state %d", Key_Info[key->value].name, key->state);
    Ipc_Device_t dest = IPC_DEVICE_SOUNDTOUCH;
    IpcKeys_SendKey(dest, *key);
    diags_ltbar_control_save_key(*key);
}