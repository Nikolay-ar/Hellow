/*
 * Оrganization: GosNIIAS
 * Year        : 2023
 * Author      : Vorobev Alexey Vitalievich
 * EMail       : vorobev_av@gosniias.ru
 */

#ifndef GIGEVISION_DATA_TYPES_H
#define GIGEVISION_DATA_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
#if 1
#define _GNU_SOURCE
#endif
#endif

#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <time.h>

#include "GigEVision_Sizes.h"
#include "GigEVision_Utilities.h"

/*
 *
 */

#ifdef GIGEVISION_PLATFORM_LINUX
#include <stddef.h>
#else
typedef unsigned long int size_t;
#ifdef __cplusplus
#if 0
#define NULL (static_cast <void *> (0))
#endif
#else
#define NULL ((void *)0)
#endif
#endif

/*
 *
 */

typedef enum boolean_e {
    FALSE = 0,
    TRUE  = 1
} boolean_t;

/*
 *
 */

typedef char               char_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef int                int32_t;
typedef unsigned int       uint32_t;
typedef long               int64_t;
typedef float              float32_t;
typedef double             float64_t;
typedef struct sockaddr_in sockaddr_in_t;
typedef struct sockaddr    sockaddr_t;
typedef struct timeval     timeval_t;
typedef struct timespec    timespec_t;

/*
 *
 */

#pragma pack (push, 1)

typedef enum GigEVision_Status_e {
    GIGEVISION_STATUS_UNKNOWN = -1,
    GIGEVISION_STATUS_FAILED  =  0,
    GIGEVISION_STATUS_OK      =  1
} GigEVision_Status_t;

typedef enum GigEVision_Device_e {
    GIGEVISION_DEVICE_UNKNOWN            = 0,
    GIGEVISION_DEVICE_CREVIS_MG_A320K_35 = 1,
    GIGEVISION_DEVICE_CREVIS_MG_A500M_22 = 2,
    GIGEVISION_DEVICE_PRIMA_IVS_70       = 3
} GigEVision_Device_t;

typedef enum GigEVision_AcquisitionMode_e {
    GIGEVISION_ACQUISITION_MODE_UNKNOWN      = 0,
    GIGEVISION_ACQUISITION_MODE_SINGLE_FRAME = 1,
    GIGEVISION_ACQUISITION_MODE_CONTINUOUS   = 2
} GigEVision_AcquisitionMode_t;

typedef struct GigEVision_Context_s {

    GigEVision_Device_t Device;

    char_t Address_Source      [GIGEVISION_SIZE_ADDRESS];
    char_t Address_Destination [GIGEVISION_SIZE_ADDRESS];

    int32_t       Socket_Handler_Control;
    int32_t       Socket_Handler_Stream;

    timeval_t     Socket_ReadTimeout_Control;
    timeval_t     Socket_ReadTimeout_Stream;

    int32_t       Socket_BufferSize_Control_Initial;
    int32_t       Socket_BufferSize_Control_Target;
    int32_t       Socket_BufferSize_Control_Result;

    int32_t       Socket_BufferSize_Stream_Initial;
    int32_t       Socket_BufferSize_Stream_Target;
    int32_t       Socket_BufferSize_Stream_Result;

    size_t        Socket_Poll_EmptyCycles_Maximum;

    sockaddr_in_t Socket_Address_Control_Source_Data_Parameter;
    socklen_t     Socket_Address_Control_Source_Size_Parameter;
    sockaddr_in_t Socket_Address_Control_Source_Data_Result;
    socklen_t     Socket_Address_Control_Source_Size_Result;
    sockaddr_in_t Socket_Address_Control_Destination_Data_Parameter;
    socklen_t     Socket_Address_Control_Destination_Size_Parameter;
    sockaddr_in_t Socket_Address_Control_Response_Data_Result;
    socklen_t     Socket_Address_Control_Response_Size_Result;

    sockaddr_in_t Socket_Address_Stream_Destination_Data_Parameter;
    socklen_t     Socket_Address_Stream_Destination_Size_Parameter;
    sockaddr_in_t Socket_Address_Stream_Destination_Data_Result;
    socklen_t     Socket_Address_Stream_Destination_Size_Result;

    size_t   Counter_Requests_Total_High;
    uint16_t Counter_Requests_Total_Low;

    size_t   Counter_Requests_Discovery;
    size_t   Counter_Requests_WriteRegister_ExclusiveAccess_On;
    size_t   Counter_Requests_WriteRegister_ExclusiveAccess_Off;
    size_t   Counter_Requests_WriteRegister_HeartbeatDisable_On;
    size_t   Counter_Requests_WriteRegister_HeartbeatDisable_Off;
    size_t   Counter_Requests_WriteRegister_BalanceWhiteAuto_Continuous;
    size_t   Counter_Requests_WriteRegister_AcquisitionMode_Continous;
    size_t   Counter_Requests_WriteRegister_AcquisitionMode_SingleFrame;
    size_t   Counter_Requests_WriteRegister_StreamDestination_Address;
    size_t   Counter_Requests_WriteRegister_StreamDestination_Port;
    size_t   Counter_Requests_WriteRegister_AcquisitionEnable_On;
    size_t   Counter_Requests_WriteRegister_AcquisitionEnable_Off;

    size_t   Counter_Responses_Discovery;
    size_t   Counter_Responses_WriteRegister_ExclusiveAccess_On;
    size_t   Counter_Responses_WriteRegister_ExclusiveAccess_Off;
    size_t   Counter_Responses_WriteRegister_HeartbeatDisable_On;
    size_t   Counter_Responses_WriteRegister_HeartbeatDisable_Off;
    size_t   Counter_Responses_WriteRegister_BalanceWhiteAuto_Continuous;
    size_t   Counter_Responses_WriteRegister_AcquisitionMode_Continous;
    size_t   Counter_Responses_WriteRegister_AcquisitionMode_SingleFrame;
    size_t   Counter_Responses_WriteRegister_StreamDestination_Address;
    size_t   Counter_Responses_WriteRegister_StreamDestination_Port;
    size_t   Counter_Responses_WriteRegister_AcquisitionEnable_On;
    size_t   Counter_Responses_WriteRegister_AcquisitionEnable_Off;

    size_t   Counter_Data_Image_Total;
    size_t   Counter_Data_Image_Leader;
    size_t   Counter_Data_Image_Payload_Total;
    size_t   Counter_Data_Image_Payload_Current;
    size_t   Counter_Data_Image_Trailer;
    size_t   Counter_Data_Image_Unknown;

    size_t   Counter_Cycles_Receive_Image_Total;
    size_t   Counter_Cycles_Receive_Image_Empty;

    size_t   Counter_Cycles_Receive_Stream_Total;
    size_t   Counter_Cycles_Receive_Stream_Empty;

    GigEVision_AcquisitionMode_t AcquisitionMode;

    timeval_t Time_Leader;
    timeval_t Time_Trailer;
    timeval_t Time_Image;
    timeval_t Time_Gap;

    size_t Buffer_Offset;
    size_t Payload_Size;

    uint8_t *Buffer_Data;

    void *UserData;

} GigEVision_Context_t;

#pragma pack (pop)

#ifdef __cplusplus
}
#endif

#endif
