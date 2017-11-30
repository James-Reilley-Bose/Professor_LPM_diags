//
// WirelessAudioData.h
//

#ifndef WIRELESS_AUDIO_DATA_H
#define WIRELESS_AUDIO_DATA_H

#define WA_COCO_LEN 3
#define WA_SERIAL_NO_LENGTH 24
#define WA_INVALID_VERSION 0xFF

typedef enum
{
    WA_BCMD_ID = 0x01,
    WA_BCMD_SERIAL_NO = 0x02,
    WA_BCMD_TELEMETRY = 0x03,
    WA_BCMD_DSP_ACTION = 0x04,
    WA_BCMD_LINKED = 0x05,
    WA_BCMD_ID_ACK = 0x06,
    WA_BCMD_TEST = 0x07,
    WA_BCMD_TELEMETRY_REQUEST = 0x08,
    WA_BCMD_REBOOT_MU = 0x09,

    WA_BCMD_UPD_DATA = 0x20,
    WA_BCMD_ENTER_UPDATE = 0x21,
    WA_BCMD_READY_TO_UPDATE = 0x22,
    WA_BCMD_MAP_REQUEST = 0x23,
    WA_BCMD_MAP_RESPONSE = 0x24,
    WA_BCMD_CLEAR_MAP = 0x25,
    WA_BCMD_MAP_CLEARED = 0x26,
    WA_BCMD_UPD_AUTHENTICATE = 0x27,
    WA_BCMD_UPD_INSTALL = 0x28,
    WA_BCMD_UPD_VERSION = 0x29,
} WA_BoseCmd_t;

typedef enum
{
    WA_CHANNEL_INVALID = 0,
    WA_CHANNEL_BASS_SKIPPER = 0x01,
    WA_CHANNEL_BASS_PBB  = 0x02,
    WA_CHANNEL_SURROUND_LEFT = 0x28,
    WA_CHANNEL_SURROUND_RIGHT = 0x20,
    WA_NUM_CHANNELS = 4,
} WA_ChannelID_t;

#define WA_CHANNEL_IS_SURROUND(channel) (((channel) == WA_CHANNEL_SURROUND_LEFT) || \
                                         ((channel) == WA_CHANNEL_SURROUND_RIGHT))
#define WA_CHANNEL_IS_BASS(channel) (((channel) == WA_CHANNEL_BASS_SKIPPER) || ((channel) == WA_CHANNEL_BASS_PBB))

#define WA_CHANNEL_IS_BASS(channel) (((channel) == WA_CHANNEL_BASS_SKIPPER) || ((channel) == WA_CHANNEL_BASS_PBB))


#define WA_CHANNEL_BASS_BIT (1 << 0)
#define WA_CHANNEL_SURROUND_LEFT_BIT (1 << 1)
#define WA_CHANNEL_SURROUND_RIGHT_BIT (1 << 2)
#define WA_CHANNEL_ALL (WA_CHANNEL_BASS_BIT | \
                        WA_CHANNEL_SURROUND_LEFT_BIT | \
                        WA_CHANNEL_SURROUND_RIGHT_BIT)

typedef enum
{
    WA_SN_SEND = 0x01,
    WA_SN_ACK = 0x02,
} WA_SN_SubCmd_t;

#define WA_BOSE_COMMAND_ID 0x20 /* Bose command identifier */

#define WA_DATA_PKT_PAYLOAD_SIZE 28
typedef struct
{
    uint8_t boseID; // always set this to a unique value to indicate this is a "bose command"
    uint8_t opcode;
    uint8_t data[WA_DATA_PKT_PAYLOAD_SIZE];
} WA_DataMessage_t;

typedef struct 
{
    uint8_t channel;
    uint8_t do_not_use0; // deprecated, do not use
    uint8_t darr_ver;
    uint8_t lpm_major;
    uint8_t lpm_minor;
    uint8_t lpm_patch;
    uint16_t lpm_build;
    uint16_t do_not_use1; // deprecated, do not use
    uint8_t coco[WA_COCO_LEN];
    uint32_t serialNumberCRC;
} WAPacket_ID_t;

typedef struct
{
    uint8_t reserved;
    uint8_t channel;
} WAPacket_IDAck_t;

typedef struct
{
    uint8_t subCmd; // kept for reverse compatibility
    uint8_t channel;
    uint8_t serialNumber[WA_SERIAL_NO_LENGTH];
    uint16_t cocoSum;
} WAPacket_SerialNum_t;

typedef struct
{
    BOOL ampFault;
    BOOL dcOffset;
    uint8_t channel;
    uint8_t do_not_use; // deprecated, do not use
    uint32_t ampTemp;
    uint32_t serialNumberCRC;
} WAPacket_Telemetry_t;

#endif // WIRELESS_AUDIO_DATA_H
