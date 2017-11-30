/*
    File    :   WirelessAudioUpdateTransfer.h
    Title   :   
    Author  :   dr1005920
    Created :   01/05/16
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:  Header file for transfer/update over DARR data channel

===============================================================================
*/

#ifndef WA_UPDATE_TRANSFER_H
#define WA_UPDATE_TRANSFER_H

#include "project.h"
#include "WirelessAudioData.h"
#include "UpdateManagerStateDefinitions.h"
#include "product_config.h"

#define WA_CHUNK_NUM_OF_MAP_ENTRIES 64
#define WA_CHUNK_LEN  (WA_CHUNK_NUM_OF_MAP_ENTRIES * WA_UPDATE_PAYLOAD_SIZE)
#define WA_TRANSFER_MAX_RETRIES 30
#define WA_TRANSFER_RETRIES_BEFORE_RESET 10
#define WA_TRANSFER_MAP_LENGTH 8
#define WA_DATA_REQUEST_TIMER_INTERVAL TIMER_MSEC_TO_TICKS(5000)
#define BITS_PER_BYTE 8
#define WA_TRANSFER_PACKET_DELAY 10
#define WA_DEFAULT_POLLING_TIME (1000 / portTICK_PERIOD_MS)
#define WA_UPDATE_PAYLOAD_SIZE (WA_DATA_PKT_PAYLOAD_SIZE - 1)
#define NO_VERSION "0.0.0.0000"
#define NO_VERSION_LENGTH 10
#define WA_UPDATE_MU_WAIT_INSTALL_MS TIMER_MSEC_TO_TICKS(15000)
#define WA_UPDATE_MU_UPDATE_TIMEOUT_MS TIMER_MSEC_TO_TICKS(15000)
#define WA_UPDATE_MAX_RESETS_MU 10

#define WA_LOG_MAP() LOG(wa_update,ROTTEN_LOGLEVEL_INSANE,"Map : 0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x ", \
                                                                          myTransferState.myCurrentMap[0], myTransferState.myCurrentMap[1], \
                                                                          myTransferState.myCurrentMap[2], myTransferState.myCurrentMap[3], \
                                                                          myTransferState.myCurrentMap[4], myTransferState.myCurrentMap[5], \
                                                                          myTransferState.myCurrentMap[6], myTransferState.myCurrentMap[7]); \
                                                                            
#if defined(BARDEEN) || defined(GINGER) || defined(PROFESSOR)
typedef enum
{
    WA_UPDATE_IDLE = 0,
    WA_UPDATE_CHECK_VERSION,    // Check MUs .bos version to see if update required
    WA_UPDATE_STARTING_UPDATE,  // Tell MUs to enter update
    WA_UPDATE_CLEARING_MAP,     // Tell MUs to clear their maps
    WA_UPDATE_SENDING_CHUNK,    // Send data payloads to MUs
    WA_UPDATE_REQUESTING_MAP,   // Ask MUs for data map
    WA_UPDATE_RECTIFYING_CHUNK, // Resend any lost packets (holes in the received maps)
    WA_UPDATE_AUTHENTICATE,     // Ask MUs to authenticate the download
    WA_UPDATE_INSTALL,          // Tell MUs to apply the update
    WA_UPDATE_EXIT,             // Clean up, the MUs are on their own now
    WA_UPDATE_NUM_OF_STATES
}waUpdateState;
#endif

#if defined(SKIPPER) || defined(MAXWELL)
typedef enum
{
    WA_UPDATE_IDLE = 0,
    WA_RECIEVING_DATA,          // Downloading the update
    WA_UPDATE_WAIT_INSTALL,     // Wait for all MUs to get install signal
    WA_UPDATE_INSTALLING,       // Apply the update
    WA_UPDATE_NUM_OF_STATES
}waUpdateState;
#endif

typedef struct 
{
    waUpdateState state;
    BOOL updateRequired;
    uint8_t numConnectedMUs;
    uint32_t currentChunk;
    uint32_t dataOffsetStart;
    uint32_t transferLength;
    uint32_t numberOfChunks;
    uint8_t  msgsReceived;
    uint8_t  sendRetries;
    uint8_t  rectifyRetries;
    TimerHandle_t timer;
    uint8_t  myCurrentMap[WA_TRANSFER_MAP_LENGTH];
}waTransferState;

typedef struct
{
    void(*enterFunc)(void);
    void(*handleMessageFunc)(WA_DataMessage_t* msg);
    void(*handleTimerFunc)(void);
    void(*exitFunc)(void);
}waTransferStateStruct;

extern waTransferState myTransferState; 
extern const waTransferStateStruct myWaTransferStates[WA_UPDATE_NUM_OF_STATES];

// Headers for both 
void waTransfer_Init(void);
void waTransfer_Abort(UpdateResult_t result);
void waTransfer_Start(uint32_t imgOffset, uint32_t size);
void waTransfer_GoToNextState(waUpdateState state);
void waTransfer_HandleDarrMessage(WA_DataMessage_t* msg);
void waTransfer_HandleTimer(void);

// CU function headers
void waTransfer_CUSendCommand(WA_BoseCmd_t command, uint8_t param, TickType_t timerPeriod);
BOOL waTransfer_GetMapFieldFilled(uint8_t index, uint8_t* map);
void waTransfer_BuildBlobPayload(uint8_t* data,  uint8_t index, uint32_t dataOffset, uint8_t length);
BOOL waTransfer_IsMapFilled(uint8_t* map, uint32_t chunkLength);
void waTransfer_SendNextChunk(void);
void waTransfer_RectifyChunk(void);
void waTransfer_RecieveMap(WA_DataMessage_t* msg);
BOOL waTransfer_IsRectifyNeeded(void);
int waTransfer_GetProgress(void);

// MU function headers
void waTransfer_MUSendCommand(WA_BoseCmd_t command);
void waTransfer_SetMapFieldFilled(uint8_t index, uint8_t* map);
void waTransfer_HandleMapRequest(void);
void waTransfer_RecieveTransferData(WA_DataMessage_t* msg);
void waTransfer_MUReportVersion(void);
void waTransfer_MUSendAuthentication(void);
void waTransfer_DoWAMUpdate(BOOL forced);

// Heades for each state functions ( don't have to be ifdef'd but i did for reading ease )
#if defined(BARDEEN) || defined(GINGER) || defined(PROFESSOR)
void waTransfer_EnterFunc_WA_UPDATE_IDLE(void);

void waTransfer_EnterFunc_WA_CHECK_VERSION(void);
void waTransfer_HandleMessageFunc_WA_CHECK_VERSION(WA_DataMessage_t* msg);
void waTransfer_HandleTimerFunc_WA_CHECK_VERSION(void);

void waTransfer_EnterFunc_WA_UPDATE_STARTING_UPDATE(void);
void waTransfer_HandleMessageFunc_WA_UPDATE_STARTING_UPDATE(WA_DataMessage_t* msg);
void waTransfer_HandleTimerFunc_WA_UPDATE_STARTING_UPDATE(void);

void waTransfer_EnterFunc_WA_UPDATE_CLEARING_MAP(void);
void waTransfer_HandleMessageFunc_WA_UPDATE_CLEARING_MAP(WA_DataMessage_t* msg);
void waTransfer_HandleTimerFunc_WA_UPDATE_CLEARING_MAP(void);

void waTransfer_EnterFunc_WA_UPDATE_SENDING_CHUNK(void);
    
void waTransfer_EnterFunc_WA_UPDATE_REQUESTING_MAP(void);
void waTransfer_HandleMessageFunc_WA_UPDATE_REQUESTING_MAP(WA_DataMessage_t* msg);
void waTransfer_HandleTimerFunc_WA_UPDATE_REQUESTING_MAP(void);
    
void waTransfer_EnterFunc_WA_UPDATE_RECTIFYING_CHUNK(void);
    
void waTransfer_EnterFunc_WA_UPDATE_AUTHENTICATE(void);
void waTransfer_HandleMessageFunc_WA_UPDATE_AUTHENTICATE(WA_DataMessage_t* msg);
void waTransfer_HandleTimerFunc_WA_UPDATE_AUTHENTICATE(void);
   
void waTransfer_EnterFunc_WA_UPDATE_INSTALL(void);
void waTransfer_HandleMessageFunc_WA_UPDATE_INSTALL(WA_DataMessage_t* msg);
void waTransfer_HandleTimerFunc_WA_UPDATE_INSTALL(void);
    
void waTransfer_EnterFunc_WA_UPDATE_EXIT(void);
    
#endif

#if defined(SKIPPER) || defined(MAXWELL)
void waTransfer_HandleMessageFunc_WA_UPDATE_IDLE(WA_DataMessage_t* msg);
void waTransfer_ExitFunc_WA_UPDATE_IDLE(void);

void waTransfer_EnterFunc_WA_CLEARING_MAP(void);
void waTransfer_HandleMessageFunc_WA_CLEARING_MAP(WA_DataMessage_t* msg);

void waTransfer_EnterFunc_WA_RECIEVING_DATA(void);
void waTransfer_HandleMessageFunc_WA_RECIEVING_DATA(WA_DataMessage_t* msg);
void waTransfer_HandeTimerFunc_WA_RECIEVING_DATA(void);

void waTransfer_EnterFunc_WA_UPDATE_WAIT_INSTALL(void);
void waTransfer_HandleMessageFunc_WA_UPDATE_WAIT_INSTALL(WA_DataMessage_t* msg);
void waTransfer_HandleTimerFunc_WA_UPDATE_WAIT_INSTALL(void);

void waTransfer_EnterFunc_WA_UPDATE_INSTALL(void);
#endif

void waSetUpdating(BOOL state);
BOOL waGetUpdating(void);
waUpdateState waGetUpdateState(void);
#endif /* WA_UPDATE_TRANSFER_H */ 
