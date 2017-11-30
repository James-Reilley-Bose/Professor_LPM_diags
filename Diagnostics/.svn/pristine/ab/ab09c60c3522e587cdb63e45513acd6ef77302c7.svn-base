/****************************************************************************
 * Copyright:  (C) 2017 Bose Corporation, Framingham, MA                    *
 ****************************************************************************/

/**
 * @file Sii9437Task.c
 * @author lb1032816
 * @date 5/26/2017
 * @brief Implementation of the Sii9437Task.
 *
 * Defined within is the implementation of the Sii9437Task whose main purpose is to
 * handle interrupts trigger by the Sii9437 chip as well as service incoming requests
 * from the HDMI task.
 *
 * This task primarily interfaces with the Sii9437Driver which is a wrapper around
 * the drivers provided my Lattice (the manufacturers of the Sii9437 chip).
 *
 */

#include "project.h"
#include "Sii9437Task.h"
#include "Sii9437Driver.h"
#include "BMutex.h"
#include "BufferManager.h"
#include "IpcRouterTask.h"
#include "UpdateManagerTask.h"
#include "nvram.h"
#include "genTimer.h"
#include "product_macros.h"
#include "edidapi.h"

#include "sii9437_regs.h"
#include "sii9437_drv.h"
#include "sii_lib_data.h"
#include "sii_platform_api.h"

/* Logs
===============================================================================*/
SCRIBE(sii9437, ROTTEN_LOGLEVEL_NORMAL);

/* Global Variable
===============================================================================*/
ManagedTask* Sii9437TaskHandle = NULL;
I2C_BUS_HANDLE_TYPE SiiI2CBusHandle;

struct sii9437_config Sii9437Config =
{
    .dev_id = 0,
    .callback_func = Sii9437_EventCallback,
    .log_fifo_size = 256,
    .i2c_addr_1 = SII9437_INST_1_I2C_ADDR_PAGE1,
    .i2c_addr_2 = SII9437_INST_1_I2C_ADDR_PAGE2,
    .erx_latency = {40}
};

sii_inst_t Sii9437Instance = SII_INST_NULL;

//TODO: Temporary edid for development
uint8_t audEdid[128] =
{
    0x23, 0x09, 0x07, 0x01, 0x88, 0x9e, 0x00, 0x00,
    0xDC, 0x20, 0x00, 0x20, 0xE0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/* Local Defines
===============================================================================*/
static SemaphoreHandle_t devicePresenceMutex;

/* Local Variable
===============================================================================*/

/* Local Prototypes
===============================================================================*/

static void Sii9437_EnableI2C(void);
static void Sii9437_Init(void);
static void driver_reset_callback(void);


void Sii9437Task(void* pvParamaters)
{
    debug_assert(NULL != Sii9437TaskHandle);

    for (;;)
    {
        TaskManagerPollQueue(Sii9437TaskHandle);
    }
}

void Sii9437_TaskInit(void* p)
{
    Sii9437Instance = SII_INST_NULL;

    devicePresenceMutex = xSemaphoreCreateMutex();
    debug_assert(devicePresenceMutex);

    Sii9437TaskHandle = GetManagedTaskPointer("Sii9437Task");
}

void Sii9437_HandleMessage(GENERIC_MSG_t* message)
{
    switch (message->msgID)
    {
        case SII9437_MESSAGE_ID_Init:
            Sii9437_Init();
            break;
        case SII9437_MESSAGE_ID_SetPowerState:
            Sii9437Driver_SetPowerState(message->params[0]);
            break;
        case SII9437_MESSAGE_ID_GetPowerState:
            {
                BOOL power_state = FALSE;
                Sii9437Driver_GetPowerState(&power_state);
                break;
            }
        case SII9437_MESSAGE_ID_SetHPD:
            Sii9437Driver_SetHotPlugDetected(message->params[0]);
            break;
        case SII9437_MESSAGE_ID_GetHPD:
            Sii9437_GetHotPlugDetected();
            break;
        case SII9437_MESSAGE_ID_SetArcMode:
            Sii9437Driver_SetArcMode((Sii9437_ArcMode_t)message->params[0]);
            break;
        case SII9437_MESSAGE_ID_GetArcMode:
            {
                Sii9437_ArcMode_t arc_mode;
                Sii9437Driver_GetArcMode(&arc_mode);
                break;
            }
        case SII9437_MESSAGE_ID_ServiceInterrupt:
            sii9437_handle(Sii9437Instance);
            break;
        default:
            break;
    }
}

void Sii9437_EventCallback(uint32_t* pEventFlags)
{
    Sii9437_ErxLatency_t erx_latency_req;
    Sii9437_ExtractMode_t extraction_mode;

    if (*pEventFlags & SII9437_EVENT_FLAGS__LOG_FIFO_CHNG)
    {
        debug_assert(0);
        LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "LOG FIFO change."); //This should not happen
    }

    if (*pEventFlags & SII9437_EVENT_FLAGS__CHAN_STAT_CHNG)
    {
        // TODO (lb1032816): This has implications on the DSP
        LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "Incoming audio channel status changed.")
        (void)Sii9437Driver_UpdateAudioChannel();
        Sii9437Driver_GetExtractionMode(&extraction_mode);
        LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "Current extraction mode is %d", extraction_mode);
    }

    if (*pEventFlags & SII9437_EVENT_FLAGS__HPD_CHNG)
    {
        bool_t hpd_status;
        (void)Sii9437Driver_GetHotPlugDetected(&hpd_status);
        LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "HPD changed to %s\n", hpd_status ? "hi" : "lo");
    }

    if (*pEventFlags & SII9437_EVENT_FLAGS__GPIO_CHNG)
    {
        //This should never happen since none of the GPIOs are being used
        uint16_t gpio_status;
        sii9437_gpio_query(Sii9437Instance, &gpio_status);
        LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "GPIO input status changed - 0x%02x\n",
            gpio_status);
    }

    if (*pEventFlags & SII9437_EVENT_FLAGS__ERX_LATENCY_REQ_CHNG)
    {
        sii9437_erx_latency_req_query(Sii9437Instance,
                                      &erx_latency_req);
        LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "Audio Latency Req from Tx: %d msec\n", erx_latency_req.data);
    }

    if (*pEventFlags & SII9437_EVENT_FLAGS__EARC_LINK_CHNG)
    {
        Sii9437Driver_EarcLinkStatus();
    }

    if (*pEventFlags & SII9437_EVENT_FLAGS__EARC_DISC_TIMEOUT)
    {
        LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "eARC Discovery Failed: TIMEOUT\n");
    }

    if (*pEventFlags & SII9437_EVENT_FLAGS__ASSERT) //TODO: Should reset eARC chip after
    {
        Sii9437Driver_AssertionQuery();
    }
}

void Sii9437_GetHotPlugDetected(void)
{
    BOOL hpd_state = FALSE;
    Sii9437Driver_GetHotPlugDetected(&hpd_state);
    LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "eARC HPD state is: %s", hpd_state ? "High" : "Low");
}

static void Sii9437_Init(void)
{
    Sii9437_EnableEXTI();
    Sii9437_EnableI2C();

    if (Sii9437Instance == NULL)
    {
        if (i2cMaster_Ping(SiiI2CBusHandle, SII9437_TOP_REGISTER_ADDR))
        {
            Sii9437Driver_CreateInstance(driver_reset_callback);
            Sii9437PostMsg(SII9437_MESSAGE_ID_ServiceInterrupt, NOP_CALLBACK, 0);
        }
        else
        {
            Sii9437Instance = SII_INST_NULL;
            LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "Unable to connect to Sii9437");
        }
    }
    else
    {
        LOG(sii9437, ROTTEN_LOGLEVEL_NORMAL, "A task tried to instantiate the Sii9437 Driver but an instance was already running");
    }

    Sii9437_PreferedExtractMode_t extration_mode = SII9437_PREF_EXTRACTION_MODE__I2S;
    Sii9437Driver_SetPreferedExtractionMode(extration_mode);
    edid_t* edid = NULL;
    edid = EDID_GetA4VEdid();
    
    Sii9437Driver_SetEDID(&(edid->edidData[129]), 128); //TODO: Temporary solution in order to work with Devkit transmitter (Sii9438)
}

static void Sii9437_EnableI2C(void)
{
    I2C_Master_Config_t sii9437I2CConfig = {.busNumber = HDMI_I2C_BUS, .masterClockFrequency = HDMI_I2C_SPEED, .disableDMA = 1};

    // Turn on I2C for the chips.
    if (SiiI2CBusHandle == NULL)
    {
        I2C_API_ERROR_CODE pError = I2C_API_NO_ERROR;
        SiiI2CBusHandle = i2cMaster_Init(&sii9437I2CConfig, &pError);
        debug_assert(I2C_API_NO_ERROR == pError);
    }
    else
    {
        i2cMaster_EnableBus(HDMI_I2C_BUS);
    }
    debug_assert(NULL != SiiI2CBusHandle);
}

static void driver_reset_callback(void)
{
    //TODO: Need to impliment Reset that cordinates with the rest of the system
    Sii9437PostMsg(SII9437_MESSAGE_ID_ResetDriver, NOP_CALLBACK, 0);
}

