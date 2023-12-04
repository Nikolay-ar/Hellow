/*
 * Оrganization: GosNIIAS
 * Year        : 2023
 * Author      : Vorobev Alexey Vitalievich
 * EMail       : vorobev_av@gosniias.ru
 */

#ifndef GIGEVISION_PACKET_CONTROL_H
#define GIGEVISION_PACKET_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "GigEVision_DataTypes.h"

/*
 *
 */

#define GIGEVISION_SIZE_PACKET_CONTROL_DATA 512

/*
 *
 */

#pragma pack (push, 1)

typedef enum GigEVision_Packet_Control_Type_s {
    GIGEVISION_PACKET_CONTROL_TYPE_ACK           = 0x00,
    GIGEVISION_PACKET_CONTROL_TYPE_CMD           = 0x42,
    GIGEVISION_PACKET_CONTROL_TYPE_ERROR         = 0x80,
    GIGEVISION_PACKET_CONTROL_TYPE_UNKNOWN_ERROR = 0x8F
} GigEVision_Packet_Control_Type_t;

typedef enum GigEVision_Packet_Control_Flags_s {
    GIGEVISION_PACKET_CONTROL_FLAGS_NONE         = 0x00,
    GIGEVISION_PACKET_CONTROL_FLAGS_ACK_REQUIRED = 0x01,
    GIGEVISION_PACKET_CONTROL_FLAGS_EXTENDED_IDS = 0x10
} GigEVision_Packet_Control_Flags_t;

typedef enum GigEVision_Packet_Control_Discovery_Flags_s {
    GIGEVISION_PACKET_CONTROL_DISCOVERY_FLAGS_NONE                = 0x00,
    GIGEVISION_PACKET_CONTROL_DISCOVERY_FLAGS_ALLOW_BROADCAST_ACK = 0x10
} GigEVision_Packet_Control_Discovery_Flags_t;

typedef enum GigEVision_Packet_Control_Command_s {
    GIGEVISION_PACKET_CONTROL_COMMAND_DISCOVERY_CMD      = 0x0002,
    GIGEVISION_PACKET_CONTROL_COMMAND_DISCOVERY_ACK      = 0x0003,
    GIGEVISION_PACKET_CONTROL_COMMAND_BYE_CMD            = 0x0004,
    GIGEVISION_PACKET_CONTROL_COMMAND_BYE_ACK            = 0x0005,
    GIGEVISION_PACKET_CONTROL_COMMAND_PACKET_RESEND_CMD  = 0x0040,
    GIGEVISION_PACKET_CONTROL_COMMAND_PACKET_RESEND_ACK  = 0x0041,
    GIGEVISION_PACKET_CONTROL_COMMAND_READ_REGISTER_CMD  = 0x0080,
    GIGEVISION_PACKET_CONTROL_COMMAND_READ_REGISTER_ACK  = 0x0081,
    GIGEVISION_PACKET_CONTROL_COMMAND_WRITE_REGISTER_CMD = 0x0082,
    GIGEVISION_PACKET_CONTROL_COMMAND_WRITE_REGISTER_ACK = 0x0083,
    GIGEVISION_PACKET_CONTROL_COMMAND_READ_MEMORY_CMD    = 0x0084,
    GIGEVISION_PACKET_CONTROL_COMMAND_READ_MEMORY_ACK    = 0x0085,
    GIGEVISION_PACKET_CONTROL_COMMAND_WRITE_MEMORY_CMD   = 0x0086,
    GIGEVISION_PACKET_CONTROL_COMMAND_WRITE_MEMORY_ACK   = 0x0087,
    GIGEVISION_PACKET_CONTROL_COMMAND_PENDING_ACK        = 0x0089
} GigEVision_Packet_Control_Command_t;

typedef struct GigEVision_Packet_Control_Header_s {
    uint8_t  Packet_Type;
    uint8_t  Packet_Flags;
    uint16_t Command;
    uint16_t Size;
    uint16_t ID;
} GigEVision_Packet_Control_Header_t;

typedef struct GigEVision_Packet_Control_Data_WriteRegister_s {
    uint32_t Address;
    uint32_t Value;
} GigEVision_Packet_Control_Data_WriteRegister_t;

typedef struct GigEVision_Packet_Control_s {
    GigEVision_Packet_Control_Header_t Header;
    uint8_t                            Data [GIGEVISION_SIZE_PACKET_CONTROL_DATA];
} GigEVision_Packet_Control_t;

#pragma pack (pop)

#ifdef __cplusplus
}
#endif

#endif
