/**
 *
 * @author Shelby Apps Team
 *
 * @attention
 *     BOSE CORPORATION.
 *     COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
 *     This program may not be reproduced, in whole or in part in any
 *     form or any means whatsoever without the written permission of:
 *         BOSE CORPORATION
 *         The Mountain,
 *         Framingham, MA 01701-9168
 */

#include "project.h"
#include "product_config.h"
#include "LightBarCapSenseTask.h"
#include "TaskDefs.h"
#include "TaskManager.h"
#include "PSoCComms.h"
#include "i2cMaster_API.h"
#include "RivieraLPM_IpcProtocol.h"
#include "IPCRouterTask.h"
#include "etap.h"
#include <cybtldr_parse.h>
#include <cybtldr_command.h>
#include <cybtldr_api.h>
#include "LightBarAnimationDB.h"

SCRIBE(lbcstask, ROTTEN_LOGLEVEL_NORMAL);

ManagedTask* lbcsTaskHandle = NULL;

//static variables
static I2C_BUS_HANDLE_TYPE lbcsI2CHandle = NULL;
static BOOL loggingEnabled = TRUE;
static uint8_t lbcsCmdBuffer[PSOC_COMMS_TO_PSOC_BUFFER_SIZE];

static BOOL psocSwVersionAvailable = FALSE;
static uint8_t psocSwMajorVersion = 0;
static uint8_t psocSwMinorVersion = 0;

static LBCSStartAnimation_t currentAnimation;
static LBCSStartAnimation_t nextAnimation;
static BOOL animationLoading = FALSE;
static BOOL animationRunning = FALSE;
static BOOL animationWaitingToBeStarted = FALSE;

static uint32_t currentAnimNumOfPatterns = 0;
static uint32_t currentAnimationOffset = 0;
static uint32_t currentAnimationSize = 0;
static uint32_t currentAnimPatternIdx = 0;

//static functions
static void LBCS_AnimationDBReadLoadPattern(uint32_t patternIdx);
static int SendCommand(uint8_t *cmd, int cmdLen);
static int ReadData(uint8_t *data, int dataLen);
static void HandleIpcPacket(IpcPacket_t* packet);
static void ReadEvent(void);
static BOOL LBCS_LookupAnimationFromDB(uint16_t animationId, uint32_t * numOfPatterns, uint32_t * animationOffset, uint32_t * animationSize);

//External functions
extern I2C_BUS_HANDLE_TYPE PSoCTask_ConfigureI2C(void);
extern void PSoCTask_ConfigureEXTI(void);

#define LBCS_I2C_ADDRESS 0x10

#define AUTOSTART_TRUE  (1)
#define AUTOSTART_FALSE (0)

/**
 * @brief initialize lightbar/capsense task
 * @param p
 */
void LightBarCapSense_TaskInit(void* p)
{
    lbcsTaskHandle = GetManagedTaskPointer("LBCSTask");
    debug_assert (NULL != lbcsTaskHandle);
    PSoCTask_ConfigureEXTI();
    lbcsI2CHandle = PSoCTask_ConfigureI2C();
    debug_assert(lbcsI2CHandle);
}

/**
 * @func LightBarCapSenseTask
 *
 * @brief The task, handles capsense events from PSoC and lightbar commands from elsewhere
 *
 * @param void *pvParameters
 *
 * @return n/a
 */
void LightBarCapSenseTask(void* pvParamaters)
{
    //Initialize Animations Database
    if(!AnimationDBInit())
    {
        if(!GetAnimationDBChecksumStatus())
        {
            LOG(lbcstask, ROTTEN_LOGLEVEL_NORMAL, "Animation Database corrupted. Checksum verification failed\n");
        }
        else
        {
            LOG(lbcstask, ROTTEN_LOGLEVEL_NORMAL, "Size of Animations DB index table is not enough, increase NUMBER_OF_ANIMATIONS_MAX accordingly\n");
        }
    }

    for (;;)
    {
        TaskManagerPollQueue(lbcsTaskHandle);
    }
}

//This function gets the current PSoC SW version from the PSoC 
BOOL GetPsocSwVersion (uint8_t * majorVersion, uint8_t * minorVersion)
{
    if(TRUE == psocSwVersionAvailable)
    {
        *majorVersion = psocSwMajorVersion;
        *minorVersion = psocSwMinorVersion;
        psocSwVersionAvailable = FALSE; //clear flag for the next version request
        return TRUE;
    }

    return FALSE;
}

//This function resets the PSoC by appropriately toggling the PSOC_RESET GPIO
void PsocReset(void)
{
    ConfigureGpioPin(PSOC_RESET_GPIO_BANK, PSOC_RESET_GPIO_PIN, GPIO_MODE_OUT_PP, 0);
    GPIO_WriteBit(PSOC_RESET_GPIO_BANK, 0x1 << PSOC_RESET_GPIO_PIN, Bit_RESET);
    vTaskDelay(TIMER_MSEC_TO_TICKS(200));
    GPIO_WriteBit(PSOC_RESET_GPIO_BANK, 0x1 << PSOC_RESET_GPIO_PIN, Bit_SET);
}

// Unfortunately, PSoC supplied bootloader code requires int instead of uintX_t
//This function sends commands + data to the PSoC (I2C writes)
static int SendCommand(uint8_t *cmd, int cmdLen)
{
    if (!i2cMaster_WriteNoRegister(lbcsI2CHandle, LBCS_I2C_ADDRESS, cmd, cmdLen))
    {
        //TODO: Handle the scenario whenever I2C errors occur or the LPM-PSoC interface is down
        LOG(lbcstask, ROTTEN_LOGLEVEL_NORMAL, "Could not write to PSoC");
        return 1;
    }

    return 0;
}

// Unfortunately, PSoC supplied bootloader code requires int instead of uintX_t
//This function receives commands responses + data from the PSoC (I2C reads)
static int ReadData(uint8_t *data, int dataLen)
{
    if (!i2cMaster_ReadNoRegister(lbcsI2CHandle, LBCS_I2C_ADDRESS, data, dataLen))
    {
         //TODO: Handle the scenario whenever I2C errors occur or the LPM-PSoC interface is down
        LOG(lbcstask, ROTTEN_LOGLEVEL_NORMAL, "Could not read from PSoC");
        return 1;
    }

    return 0;
}

//This function handles all lightbar-capsense related IPC messages from the SoC
static void HandleIpcPacket(IpcPacket_t* packet)
{
    debug_assert (NULL != packet);
    
    // Parse lightbar ipc packets
    switch (packet->s.opcode)
    {
        case IPC_LBCS_ANIM_DB_VERSION_REQ:
        {
            LBCSAnimationDBVersion_t versionResponse;

            LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nReceived Animation DB version request\n");

            if(!GetAnimationDBChecksumStatus())
            {
                versionResponse.major = 0;
                versionResponse.minor = 0;
                versionResponse.animDBChecksumPassed = FALSE;

                LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nAnimation DB checksum verification failed\n");
            }
            else
            {
                //Get Animation DB version
                GetAnimationDBVersion(&versionResponse.major, &versionResponse.minor);
                versionResponse.animDBChecksumPassed = TRUE;

                LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nAnimation DB checksum verification passed\n");
            }

            //Send Animation DB version response

            LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nSending Animation DB version (major:%d,minor:%d)\n",versionResponse.major,versionResponse.minor);

            versionResponse.major = REV_HALF_WORD_BYTES(versionResponse.major);
            versionResponse.minor = REV_HALF_WORD_BYTES(versionResponse.minor);

            //Send the response back to SoC
            IpcSendPacket_t response =
            {
                .dest = IPC_DEVICE_SOUNDTOUCH,
                .op = IPC_LBCS_ANIM_DB_VERSION_RESP,
                .sequence = packet->s.sequence,
                .connectionID = packet->s.connectionID,
                .data = (void*)&versionResponse,
                .length = sizeof(versionResponse),
            };
            IPCRouter_SendResponse(&response);
            break;
        }
        case IPC_LBCS_START_ANIM:
        {
            LBCSStartAnimation_t * startAnimation = NULL;
            LBCSResponse_t lbcsResponse;

            LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nReceived Start Animation request from the SoC\n");

            //Check if any animation is currently loading and not started
            if(animationLoading)
            {
                //Send failure response back to the SoC
                //It is not possible to send an animation, when already another animation
                //is still loading and not started
                lbcsResponse.lbcsResponseSuccess = FALSE;
            }
            else
            {
                //Get start animation request parameters from the IPC message
                startAnimation  = (LBCSStartAnimation_t*)(packet->s.data.b);
                
                debug_assert (NULL != startAnimation);

                //Get next to play animation details
                nextAnimation.animationId = REV_HALF_WORD_BYTES(startAnimation->animationId);
                nextAnimation.immediateFlag = startAnimation->immediateFlag;
                nextAnimation.repeatFlag = startAnimation->repeatFlag;

                if(!animationRunning)
                {
                    //Next animation will become current animation
                    currentAnimation = nextAnimation;

                    //Load and start animation from the database
                    LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nSending start animation to PSoC: Animation Id:%d, ImmediateFlag:%d, RepeatFlag:%d\n", currentAnimation.animationId, (uint8_t)(currentAnimation.immediateFlag), (uint8_t)(currentAnimation.repeatFlag));

                    if(!LBCS_AnimationLoadStartFromDB(currentAnimation.animationId, AUTOSTART_TRUE, (uint8_t)(currentAnimation.repeatFlag)))
                    {
                        lbcsResponse.lbcsResponseSuccess = FALSE;
                        animationLoading = FALSE;
                    }
                    else
                    {
                        lbcsResponse.lbcsResponseSuccess = TRUE;
                        animationLoading = TRUE;
                    }
                }
                else
                {
                    //Lookup animation from DB
                    if(!LBCS_LookupAnimationFromDB(nextAnimation.animationId, &currentAnimNumOfPatterns, &currentAnimationOffset, &currentAnimationSize))
                    {
                        lbcsResponse.lbcsResponseSuccess = FALSE;
                        animationLoading = FALSE;
                    }
                    else
                    {
                        //Stop currently running animation
                        if(nextAnimation.immediateFlag)
                        {
                            //stop the current animation immediately
                            LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nSend stop immediate Animation request to the PSoC\n");
                            LBCS_SendSimpleCommand(PSOC_COMMS_COMMAND_ANIMATION_STOPIMMEDIATE);
                        }
                        else
                        {
                            //stop the current animation after the current cycle is finished
                            LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nSend stop-at-end Animation request to the PSoC\n");
                            LBCS_SendSimpleCommand(PSOC_COMMS_COMMAND_ANIMATION_STOPATEND);
                        }

                        lbcsResponse.lbcsResponseSuccess = TRUE;
                        animationWaitingToBeStarted = TRUE;
                    }
                }
            }

            LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nSend %d response to SoC\n",(uint8_t)(lbcsResponse.lbcsResponseSuccess));

            //Send the response back to SoC
            IpcSendPacket_t response =
            {
                .dest = IPC_DEVICE_SOUNDTOUCH,
                .op = IPC_LBCS_START_ANIM,
                .sequence = packet->s.sequence,
                .connectionID = packet->s.connectionID,
                .data = (void*)&lbcsResponse,
                .length = sizeof(lbcsResponse),
            };
            IPCRouter_SendResponse(&response);

            break;
        }
        case IPC_LBCS_STOP_ANIM:
        {
            LBCSResponse_t lbcsResponse;

            LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nReceived Stop Animation request from the SoC\n");

            //Stop current animation after the last pattern is played
            LBCS_SendSimpleCommand(PSOC_COMMS_COMMAND_ANIMATION_STOPATEND);
            lbcsResponse.lbcsResponseSuccess = TRUE;

            LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nSend %d response to SoC\n",(uint8_t)(lbcsResponse.lbcsResponseSuccess));

            //Send the response back to SoC
            IpcSendPacket_t response =
            {
                .dest = IPC_DEVICE_SOUNDTOUCH,
                .op = IPC_LBCS_STOP_ANIM,
                .sequence = packet->s.sequence,
                .connectionID = packet->s.connectionID,
                .data = (void*)&lbcsResponse,
                .length = sizeof(lbcsResponse),
            };
            IPCRouter_SendResponse(&response);
            break;
        }
        case IPC_LBCS_ABORT_ANIM:
        {
            LBCSResponse_t lbcsResponse;

            LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nReceived Stop immediate Animation request from the SoC\n");

            //Stop current animation immediately
            LBCS_SendSimpleCommand(PSOC_COMMS_COMMAND_ANIMATION_STOPIMMEDIATE);
            lbcsResponse.lbcsResponseSuccess = TRUE;

            LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nSend %d response to SoC\n",(uint8_t)(lbcsResponse.lbcsResponseSuccess));

            //Send the response back to SoC
            IpcSendPacket_t response =
            {
                .dest = IPC_DEVICE_SOUNDTOUCH,
                .op = IPC_LBCS_ABORT_ANIM,
                .sequence = packet->s.sequence,
                .connectionID = packet->s.connectionID,
                .data = (void*)&lbcsResponse,
                .length = sizeof(lbcsResponse),
            };
            IPCRouter_SendResponse(&response);
            break;
        }
    }

    //Return IPC packet buffer
    IPCRouter_ReturnRxBuffer(packet);
}

/*
 * Process events from PSoC and send them to SOC via IPC
 */
static void ReadEvent(void)
{
    uint8_t rcvBuff[PSOC_COMMS_FROM_PSOC_BUFFER_SIZE];

    if (ReadData(rcvBuff, sizeof(rcvBuff)))
    {
        return;
    }

    if (loggingEnabled)
    {
        LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "PSoC sent response %02x: %02x,%02x,%02x,%02x,%02x", rcvBuff[0], rcvBuff[1], rcvBuff[2], rcvBuff[3], rcvBuff[4], rcvBuff[5]);
    }

    if (rcvBuff[0] >= PSOC_COMMS_RESPONSE_INVALID)
    {
        return;
    }

    PSoCCommsResponse_t r = (PSoCCommsResponse_t)rcvBuff[0];
    switch (r)
    {
        case PSOC_COMMS_RESPONSE_BUTTON:
        {
            IpcKeyInformation_t key_msg;

            //Populate the IPC key msg using the button event response received from PSoC
            key_msg.keyOrigin = (KeyOrigin_t)REV_WORD_BYTES(KEY_ORIGIN_CONSOLE_BUTTON);
            key_msg.keyId = REV_HALF_WORD_BYTES(rcvBuff[1]);
            key_msg.keyState = (KeyState_t)REV_WORD_BYTES(rcvBuff[2]);

            //Send Key IPC message to the SoC
            IPCRouter_Send(IPC_DEVICE_SOUNDTOUCH, IPC_KEY, NULL, &key_msg, sizeof(key_msg));
            break;
        }
        case PSOC_COMMS_RESPONSE_SLIDER:
        {
            LBCSSliderEvent_t slider_msg;

            //Populate the Slider IPC msg using the slider event response received from the PSoC
            slider_msg.sliderId = rcvBuff[1];
            slider_msg.position = REV_HALF_WORD_BYTES((uint16_t)(rcvBuff[2] | (rcvBuff[3] << 8))); //LSB First
            slider_msg.state = (LBCSSliderState_t)REV_WORD_BYTES(rcvBuff[4]);

            //Send Slider IPC message to the SoC
            IPCRouter_Send(IPC_DEVICE_SOUNDTOUCH, IPC_LBCS_SLIDER_MSG, NULL, &slider_msg, sizeof(slider_msg));
            break;
        }
        case PSOC_COMMS_RESPONSE_STATUS:
        {
            if(PSOC_COMMS_STATUS_FAILURE == rcvBuff[1])
            {
                LOG(lbcstask, ROTTEN_LOGLEVEL_NORMAL, "Received failure response from PSoC for Command %d", rcvBuff[2]);
            }

            if((PSOC_COMMS_STATUS_SUCCESS == rcvBuff[1]) &&
                    (PSOC_COMMS_COMMAND_ANIMATION_LOADSTART == (PSoCCommsCommand_t)rcvBuff[2]))
            {
                //Animation load start success response from PSoC
                //Send first pattern to PSoC
                currentAnimPatternIdx = 0;
                LBCS_AnimationDBReadLoadPattern(currentAnimPatternIdx);
            }

            if((PSOC_COMMS_STATUS_SUCCESS == rcvBuff[1]) &&
                    (PSOC_COMMS_COMMAND_ANIMATION_LOADPATTERN == (PSoCCommsCommand_t)rcvBuff[2]))
            {
                //Animation load pattern success response from PSoC
                //Send the next pattern
                currentAnimPatternIdx++;

                if(currentAnimPatternIdx < currentAnimNumOfPatterns)
                {
                    LBCS_AnimationDBReadLoadPattern(currentAnimPatternIdx);
                }
            }

            break;
        }
        case PSOC_COMMS_RESPONSE_ANIMATION_STARTED:
        {
            LBCSAnimationStateEvent_t startEvent;

            //Animation loading complete and started
            animationLoading = FALSE;
            animationRunning = TRUE;

            //Populate Animation started event IPC message
            startEvent.animationId = REV_HALF_WORD_BYTES(currentAnimation.animationId);
            startEvent.state = (LBCSAnimationState_t)REV_WORD_BYTES(ANIMATION_STARTED);

            LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nAnimation Started response from PSoC: AnimationId - %d\n", currentAnimation.animationId);

            //Send Animation started event IPC message to the SoC
            IPCRouter_Send(IPC_DEVICE_SOUNDTOUCH, IPC_LBCS_ANIM_STATE_EVENT, NULL, &startEvent, sizeof(startEvent));
            break;
        }
        case PSOC_COMMS_RESPONSE_ANIMATION_STOPPED:
        {
            LBCSAnimationStateEvent_t stopEvent;

            //Animation stopped
            animationRunning = FALSE;

            //Populate Animation stopped event IPC message
            stopEvent.animationId = REV_HALF_WORD_BYTES(currentAnimation.animationId);
            stopEvent.state = (LBCSAnimationState_t)REV_WORD_BYTES(ANIMATION_STOPPED);

            LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nAnimation Stopped response from PSoC: AnimationId - %d\n", currentAnimation.animationId);

            //Send Animation stopped event IPC message to the SoC
            IPCRouter_Send(IPC_DEVICE_SOUNDTOUCH, IPC_LBCS_ANIM_STATE_EVENT, NULL, &stopEvent, sizeof(stopEvent));

            if(animationWaitingToBeStarted)
            {
                //Next animation will become current animation
                currentAnimation = nextAnimation;

                //Load and start animation from the database
                LOG(lbcstask, ROTTEN_LOGLEVEL_VERBOSE, "\nSending start animation to PSoC: Animation Id:%d, ImmediateFlag:%d, RepeatFlag:%d\n", currentAnimation.animationId, (uint8_t)(currentAnimation.immediateFlag), (uint8_t)(currentAnimation.repeatFlag));

                //Send load start command to PSoC
                LBCS_AnimationLoadStart((uint8_t)currentAnimNumOfPatterns, AUTOSTART_TRUE, (uint8_t)(currentAnimation.repeatFlag));
                animationLoading = TRUE;

                animationWaitingToBeStarted = FALSE;
            }

            break;
        }
        case PSOC_COMMS_RESPONSE_VERSION:
        {
            psocSwMajorVersion = rcvBuff[1];
            psocSwMinorVersion = rcvBuff[2];
            psocSwVersionAvailable = TRUE;
            break;
        }
        default:
            LOG(lbcstask, ROTTEN_LOGLEVEL_NORMAL, "Unknown response received from PSoC, Response id: %d", rcvBuff[0]);
            break;
    }
}

/*
 * Update support (PSoC bootloading)
 */
int psocUpdateOpenConnection(void)
{
    return 0;
}

int psocUpdateCloseConnection(void)
{
    return 0;
}

// During bootload (only), the PSoC needs a delay between writes
int psocUpdateWrite(unsigned char* wrData, int byteCnt)
{
    int ret = SendCommand(wrData, byteCnt);
    vTaskDelay(TIMER_MSEC_TO_TICKS(25));

    return ret;
}

static CyBtldr_CommunicationsData psocUpdateCommsFuncs =
{
        .OpenConnection = &psocUpdateOpenConnection,
        .CloseConnection = &psocUpdateCloseConnection,
        .ReadData = &ReadData,
        .WriteData = &psocUpdateWrite,
        .MaxTransferSize = 64u, // PSoC bootloader can only do 64 bytes at a time DO NOT INCREASE THIS
};

void PsocStartBootload(void)
{
    // PSoC should go into the bootloader at reset (for some amount of time)
    PsocReset();
}

BOOL PsocBootloadHeader(unsigned char *header)
{
    unsigned long  siliconID;
    unsigned char siliconRev;
    unsigned char packetChkSumType;
    unsigned long blVer=0;

    /* Parse the first line(header) of cyacd file to extract siliconID, siliconRev and packetChkSumType */
    if (CyBtldr_ParseHeader(strlen((const char*)header), header , &siliconID , &siliconRev ,&packetChkSumType) != CYRET_SUCCESS)
    {
        TAP_Printf("PSoC Bootload header is invalid");
        return FALSE;
    }

    /* Set the packet checksum type for communicating with bootloader. The packet checksum type to be used
     * is determined from the cyacd file header information */
    CyBtldr_SetCheckSumType((CyBtldr_ChecksumType)packetChkSumType);

    /* Start Bootloader operation */
    if (CyBtldr_StartBootloadOperation(&psocUpdateCommsFuncs, siliconID, siliconRev ,&blVer, NULL) != CYRET_SUCCESS)
    {
        TAP_Printf("PSoC could not start bootload");
        return FALSE;
    }

    return TRUE;
}

BOOL PsocBootloadData(unsigned char *data)
{
    unsigned char arrayId;
    unsigned short rowNum;
    unsigned short rowSize;
    unsigned char checksum ;
    /* rowData buffer size should be equal to the length of data to be send for each flash row
    * Equals 128
    */
    unsigned char rowData[128];

    if (CyBtldr_ParseRowData(strlen((const char*)data),data, &arrayId, &rowNum, rowData, &rowSize, &checksum) != CYRET_SUCCESS)
    {
        TAP_Printf("PSoC invalid update data");
        return FALSE;
    }

    if (CyBtldr_ProgramRow(arrayId, rowNum, rowData, rowSize) != CYRET_SUCCESS)
    {
        TAP_Printf("PSoC could not program row");
        return FALSE;
    }

    return TRUE;
}

BOOL PsocEndBootload(void)
{
    // Verify the application
    if (CyBtldr_VerifyApplication() != CYRET_SUCCESS)
    {
        TAP_Printf("PSoC program did not verify successfully");
        return FALSE;
    }

    /* End Bootloader Operation - PSoC should reboot into user code */
    CyBtldr_EndBootloadOperation();

    // TODO verify whether we need to reset the PSoC ourselves or not
    return TRUE;
}

//A LBCS simple command comprises of only 1 byte which contains the command type
void LBCS_SendSimpleCommand(PSoCCommsCommand_t command)
{
    lbcsCmdBuffer[0] = command;
    SendCommand(lbcsCmdBuffer, 1);

    return;
}

//This function allows to set the lightbar with one pattern
void LBCS_LEDsSetAll(uint8_t * pattern, uint8_t pattern_size)
{
    lbcsCmdBuffer[0] = PSOC_COMMS_COMMAND_LEDS_SETALL;
    lbcsCmdBuffer[1] = 0;
    memcpy(&lbcsCmdBuffer[2], pattern, pattern_size);
    SendCommand(lbcsCmdBuffer, pattern_size);
    return;
}

//This function allows to set one LED within the lightbar
void LBCS_LEDsSetOne(uint8_t led_id, uint16_t led_intensity)
{
    lbcsCmdBuffer[0] = PSOC_COMMS_COMMAND_LEDS_SETONE;
    lbcsCmdBuffer[1] = led_id;
    lbcsCmdBuffer[2] = led_intensity & 0xFF;
    lbcsCmdBuffer[3] = (led_intensity >> 8) & 0xFF;
    SendCommand(lbcsCmdBuffer, 4);
}

//This function sends Animation load start command to the PSoC
void LBCS_AnimationLoadStart(uint8_t num_of_patterns, uint8_t autostart, uint8_t loop)
{
    lbcsCmdBuffer[0] = PSOC_COMMS_COMMAND_ANIMATION_LOADSTART;
    lbcsCmdBuffer[1] = num_of_patterns;
    lbcsCmdBuffer[2] = autostart;
    lbcsCmdBuffer[3] = loop;
    SendCommand(lbcsCmdBuffer, 4);
}

//This function loads one pattern, which is part of a animation. An animation comprises of one or more patterns  
void LBCS_AnimationLoadPattern(uint16_t duration, uint8_t * pattern, uint8_t pattern_size)
{
    lbcsCmdBuffer[0] = PSOC_COMMS_COMMAND_ANIMATION_LOADPATTERN;
    lbcsCmdBuffer[1] = 0;
    lbcsCmdBuffer[2] = duration & 0xFF;
    lbcsCmdBuffer[3] = (duration >> 8) & 0xFF;

    memcpy(&lbcsCmdBuffer[4], pattern, pattern_size);

    SendCommand(lbcsCmdBuffer, pattern_size+4);
}

//This function provides the number of patterns, animation data offset from the start of the animation DB image and animation data size 
static BOOL LBCS_LookupAnimationFromDB(uint16_t animationId, uint32_t * numOfPatterns, uint32_t * animationOffset, uint32_t * animationSize)
{
    if(!LookupAnimationFromIndexData(animationId, animationOffset, animationSize))
    {
        LOG(lbcstask, ROTTEN_LOGLEVEL_NORMAL, "\nAnimation Id %d not found\n", animationId);
        return FALSE;
    }

    //Get number of pattern in the animation
    *numOfPatterns = GetNumOfPatternsInAnimation(*animationSize);

    return TRUE;
}

//This function reads from animation DB, a particular pattern whose index is patternIdx and sends the pattern to the PSoC
static void LBCS_AnimationDBReadLoadPattern(uint32_t patternIdx)
{
    //Read and load pattern data
    uint8_t pattern[LIGHTBAR_PATTERN_SIZE];
    uint16_t duration;

    ReadPatternFromAnimationData(patternIdx, &pattern[0], currentAnimationOffset, currentAnimationSize);

    duration = *(uint16_t *)(&pattern[0]); //first 2 bytes of the pattern is duration
    LBCS_AnimationLoadPattern(duration, &pattern[LIGHTBAR_PATTERN_DURATION_SIZE], (LIGHTBAR_PATTERN_SIZE - LIGHTBAR_PATTERN_DURATION_SIZE));
}

//This function loads and starts an animation from the the animation DB
BOOL LBCS_AnimationLoadStartFromDB(uint16_t animationId, uint8_t autostart, uint8_t loop)
{
    //Lookup animation from DB
    if(!LBCS_LookupAnimationFromDB(animationId, &currentAnimNumOfPatterns, &currentAnimationOffset, &currentAnimationSize))
    {
        return FALSE;
    }

    //Send load start command to PSoC
    LBCS_AnimationLoadStart((uint8_t)currentAnimNumOfPatterns, autostart, loop);

    return TRUE;
}

/**
 * @brief Handle queue messages from other tasks
 * @param msg
 */
void LightBarCapSenseTask_HandleMessage(GENERIC_MSG_t* msg)
{
    if ((msg->msgID > LBCS_MSG_ID_Start) && (msg->msgID < LBCS_MSG_ID_End))
    {
        switch (msg->msgID)
        {
        case LBCS_MSG_ID_HandleIPC:
            HandleIpcPacket((IpcPacket_t*)msg->params[0]);
            break;

        case LBCS_MSG_ID_ReadEvent:
            ReadEvent();
            break;
        }
    }
}
