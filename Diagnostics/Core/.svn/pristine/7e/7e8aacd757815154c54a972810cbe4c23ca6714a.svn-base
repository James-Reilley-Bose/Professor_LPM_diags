#ifndef SOURCE_INTERFACE_H
#define SOURCE_INTERFACE_H

#include "KeyData.h"
#include "IpcProtocolLpm.h"

//TODO - If adding or deleting a source from this list, make sure to verify that
//  the IPC enum A4V_IPC_SOURCE_ID is updated as needed.
#define FOREACH_SOURCE(SOURCE) \
  SOURCE(SOURCE_STANDBY) \
  SOURCE(SOURCE_HDMI_1) \
  SOURCE(SOURCE_HDMI_2) \
  SOURCE(SOURCE_HDMI_3) \
  SOURCE(SOURCE_HDMI_4) \
  SOURCE(SOURCE_HDMI_5) \
  SOURCE(SOURCE_HDMI_6) \
  SOURCE(SOURCE_SIDE_AUX) \
  SOURCE(SOURCE_TV) \
  SOURCE(SOURCE_SHELBY) \
  SOURCE(SOURCE_BLUETOOTH) \
  SOURCE(SOURCE_ADAPTIQ) \
  SOURCE(SOURCE_DEMO) \
  SOURCE(SOURCE_PTS) \
  SOURCE(SOURCE_UPDATE) \
  SOURCE(SOURCE_ANALOG_FRONT) \
  SOURCE(SOURCE_ANALOG1) \
  SOURCE(SOURCE_ANALOG2) \
  SOURCE(SOURCE_COAX1) \
  SOURCE(SOURCE_COAX2) \
  SOURCE(SOURCE_OPTICAL1) \
  SOURCE(SOURCE_OPTICAL2) \
  SOURCE(SOURCE_UNIFY) \
  SOURCE(SOURCE_ASOC_INTERNAL) \
  SOURCE(SOURCE_DARR) \
  SOURCE(SOURCE_LFE) \
  SOURCE(SOURCE_WIRED) \
  SOURCE(SOURCE_COLDBOOT) \
  SOURCE(SOURCE_FACTORY_DEFAULT) \
  SOURCE(NUM_SOURCES) \
  SOURCE(INVALID_SOURCE) \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum
{
    FOREACH_SOURCE (GENERATE_ENUM)
} SOURCE_ID;

typedef BOOL (*KeyConsumerFunc)(KEY_DATA_t);

typedef struct _source_interface_t
{
    hdmi_port               hdmiInput;    //NONE or input
    DSPSource_t             dspInput;
    BOOL                    unMuteAudioAfterSourceChange;
    BOOL                    unMuteVideoAfterSourceChange;

    void (*activate)(void);
    void (*deactivate)(void);
    BOOL (*handleSystemMessage)(GENERIC_MSG_t* msgPtr);
    KeyConsumerFunc handleKey; // Returns TRUE if handled, FALSE if not
    KeyConsumerFunc handleKeyFirst;  // Allow the source to handle the key if it needs to before anything else (i.e. special case for AIQ, etc to handle before volume model does, and i.e. update needs to ignore specific keys)
    BOOL (*isSourceChangeAllowed)(SOURCE_ID reqSource);
    uint16_t (*audioPathPresentationLatencyPtr)(SOURCE_ID reqSource); // a function pointer can be used a flag as well
} SourceInterface;

#define SOURCE_UNUSED \
    .hdmiInput = HDMI_PORT_NONE, \
    .dspInput = DSP_SOURCE_NONE, \
    .activate = NULL, \
    .deactivate = NULL, \
    .handleSystemMessage = NULL, \
    .handleKey = NULL, \
    .handleKeyFirst = FALSE, \
    .isSourceChangeAllowed = NULL, \
    .audioPathPresentationLatencyPtr = NULL

BOOL IsValidForVariant(SOURCE_ID);
const SourceInterface*  GetSourceInterface(SOURCE_ID id);

void SourceInterface_ActivateGoToFullPowerOnly(); //used by sources who don't need to override activate

BOOL SourceInterface_IsTVSource(SOURCE_ID sourceID);

BOOL SourceInterface_IsInternalSource(SOURCE_ID sourceID);
BOOL SourceInterface_IsStandbySource(SOURCE_ID sourceID);
BOOL SourceInterface_IsColdbootSource(SOURCE_ID sourceID);
BOOL SourceInterface_IsPTSSource(SOURCE_ID sourceID);
BOOL SourceInterface_IsUpdateSource(SOURCE_ID sourceID);
BOOL SourceInterface_IsUnifySource(SOURCE_ID sourceID);
BOOL SourceInterface_IsFactoryDefaultSource(SOURCE_ID sourceID);
BOOL SourceInterface_IsAdaptIQSource(SOURCE_ID sourceID);
BOOL SourceInterface_IsValidLastSource(SOURCE_ID sourceID);
BOOL SourceInterface_IsAudioSource(SOURCE_ID sourceID);
BOOL SourceInterface_IsSM2Source(SOURCE_ID sourceID);
BOOL SourceInterface_IsASOCSource(SOURCE_ID sourceID);
BOOL SourceInterface_IsHDMISource(SOURCE_ID sourceID);
BOOL SourceInterface_IsTVSource(SOURCE_ID sourceID);

hdmi_port SourceInterface_GetHDMIPortFromSourceID(SOURCE_ID sourceID);
#endif //SOURCE_INTERFACE_H
