/*
 * Оrganization: GosNIIAS
 * Year        : 2023
 * Author      : Vorobev Alexey Vitalievich
 * EMail       : vorobev_av@gosniias.ru
 */

#ifndef GIGEVISION_PACKET_STREAM_H
#define GIGEVISION_PACKET_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "GigEVision_DataTypes.h"

/*
 *
 */

#define GIGEVISION_SIZE_PACKET_STREAM_DATA 65536

/*
 *
 */

#pragma pack (push, 1)

/*
 * Standard values
 */

typedef enum GigEVision_Packet_Stream_Status_s {
    GIGEVISION_PACKET_STREAM_STATUS_OK          = 0x0000,
    GIGEVISION_PACKET_STREAM_STATUS_RESEND      = 0x0100,
    GIGEVISION_PACKET_STREAM_STATUS_UNAVAILABLE = 0x800c
} GigEVision_Packet_Stream_Status_t;

typedef enum GigEVision_Packet_Stream_Type_s {
    GIGEVISION_PACKET_STREAM_TYPE_LEADER    = 0x01,
    GIGEVISION_PACKET_STREAM_TYPE_TRAILER   = 0x02,
    GIGEVISION_PACKET_STREAM_TYPE_PAYLOAD   = 0x03,
    GIGEVISION_PACKET_STREAM_TYPE_ALL_IN    = 0x04,
    GIGEVISION_PACKET_STREAM_TYPE_H264      = 0x05,
    GIGEVISION_PACKET_STREAM_TYPE_MULTIZONE = 0x06,
    GIGEVISION_PACKET_STREAM_TYPE_MULTIPART = 0x07,
    GIGEVISION_PACKET_STREAM_TYPE_GENDC     = 0x08
} GigEVision_Packet_Stream_Type_t;

typedef union GigEVision_Packet_Stream_TypeAndPacketID_u {
    uint8_t  Type;
    uint32_t PacketID;
} GigEVision_Packet_Stream_TypeAndPacketID_t;

typedef struct GigEVision_Packet_Stream_Header_s {
    uint16_t                                   Status;
    uint16_t                                   BlockID;
    GigEVision_Packet_Stream_TypeAndPacketID_t TypeAndPacketID;
} GigEVision_Packet_Stream_Header_t;

typedef struct GigEVision_Packet_Stream_s {
    GigEVision_Packet_Stream_Header_t Header;
    uint8_t                           Data [GIGEVISION_SIZE_PACKET_STREAM_DATA];
} GigEVision_Packet_Stream_t;

/*
 * Prima IVS-70 custom values
 */

typedef enum GigEVision_Packet_Stream_Header_Prima_ChannelNumber_s {
    GIGEVISION_PACKET_STREAM_HEADER_PRIMA_CHANNEL_NUMBER_VISIBLE   = 0x00,
    GIGEVISION_PACKET_STREAM_HEADER_PRIMA_CHANNEL_NUMBER_INFRARED  = 0x01,
    GIGEVISION_PACKET_STREAM_HEADER_PRIMA_CHANNEL_NUMBER_COMPLEXED = 0x02
} GigEVision_Packet_Stream_Header_Prima_ChannelNumber_t;

typedef enum GigEVision_Packet_Stream_Header_Prima_MessageType_s {
    GIGEVISION_PACKET_STREAM_HEADER_PRIMA_MESSAGE_TYPE_LEADER  = 0x81,
    GIGEVISION_PACKET_STREAM_HEADER_PRIMA_MESSAGE_TYPE_TRAILER = 0x82,
    GIGEVISION_PACKET_STREAM_HEADER_PRIMA_MESSAGE_TYPE_PAYLOAD = 0x83
} GigEVision_Packet_Stream_Header_Prima_MessageType_t;

typedef struct GigEVision_Packet_Stream_Header_Prima_s {
    uint16_t Reserved_01;   /* 0x0000 */
    uint8_t  ChannelNumber;
    uint8_t  Reserved_02;   /* 0x00   */
    uint8_t  MessageType;
    uint16_t Reserved_03;   /* 0x0000 */
    uint8_t  Reserved_04;   /* 0x00   */
    uint64_t ID_Block;
    uint32_t ID_Message;
} GigEVision_Packet_Stream_Header_Prima_t;

typedef struct GigEVision_Packet_Stream_Prima_s {
    GigEVision_Packet_Stream_Header_Prima_t Header;
    uint8_t                                 Data [GIGEVISION_SIZE_PACKET_STREAM_DATA];
} GigEVision_Packet_Stream_Prima_t;

typedef struct GigEVision_Packet_Stream_Data_Prima_Leader_s {
    uint16_t  Reserved_01; /* 0x0000          */
    uint16_t  PayloadType; /* 0x8000 - ALWAYS */
    uint64_t  Timestamp;
    uint64_t  TimerFrequency;
    uint64_t  MaximumPayloadSize;
    float32_t Roll;
    float32_t Pitch;
} GigEVision_Packet_Stream_Data_Prima_Leader_t;

typedef struct GigEVision_Packet_Stream_Data_Prima_Trailer_s {
    uint16_t Reserved_01; /* 0x0000          */
    uint16_t PayloadType; /* 0x8000 - ALWAYS */
} GigEVision_Packet_Stream_Data_Prima_Trailer_t;

/*
 *
 */

#pragma pack (pop)

#ifdef __cplusplus
}
#endif

#endif
