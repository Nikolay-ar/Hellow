/*
 * Оrganization: GosNIIAS
 * Year        : 2023
 * Author      : Vorobev Alexey Vitalievich
 * EMail       : vorobev_av@gosniias.ru
 */

#ifndef GIGEVISION_H
#define GIGEVISION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "GigEVision_DataTypes.h"
#include "GigEVision_DeviceNames.h"
#include "GigEVision_Ports.h"
#include "GigEVision_Sizes.h"
#include "GigEVision_Utilities.h"
#include "GigEVision_Version.h"

/*
 *
 */

GigEVision_Status_t GigEVision_Open (
    GigEVision_Device_t   Device,                                  /* IN  */
    char_t               *Address_Source,                          /* IN  */
    char_t               *Address_Destination,                     /* IN  */
    int64_t               Socket_ReadTimeout_Control_Seconds,      /* IN  */
    int64_t               Socket_ReadTimeout_Control_Microseconds, /* IN  */
    int64_t               Socket_ReadTimeout_Stream_Seconds,       /* IN  */
    int64_t               Socket_ReadTimeout_Stream_Microseconds,  /* IN  */
    int32_t               Socket_BufferSize_Control_Target,        /* IN  */
    int32_t               Socket_BufferSize_Stream_Target,         /* IN  */
    size_t                Socket_Poll_EmptyCycles_Maximum,         /* IN  */
    GigEVision_Context_t *Context                                  /* OUT */
);

const char_t *GigEVision_ToString_Status (
    GigEVision_Status_t Status /* IN */
);

const char_t *GigEVision_ToString_AcquisitionMode (
    GigEVision_AcquisitionMode_t AcquisitionMode /* IN */
);

const char_t *GigEVision_ToString_Device (
    GigEVision_Device_t Device /* IN */
);

GigEVision_Status_t GigEVision_Set_AcquisitionMode (
    GigEVision_Context_t         *Context,        /* IN */
    GigEVision_AcquisitionMode_t  AcquisitionMode /* IN */
);

GigEVision_Status_t GigEVision_Get_SingleFrame (
    GigEVision_Context_t  *Context,                                 /* IN/OUT */
    size_t                 Buffer_Size,                             /* IN     */
    uint8_t               *Buffer_Data,                             /* OUT    */
    size_t                *Payload_Size,                            /* OUT    */
    void                 (*Callback_Buffer_Lock)  (void *Argument), /* IN     */
    void                 (*Callback_Buffer_Ready) (void *Argument)  /* IN     */
);

GigEVision_Status_t GigEVision_Stream_Start (
    GigEVision_Context_t  *Context,                                 /* IN/OUT */
    size_t                 Buffer_Size,                             /* IN     */
    uint8_t               *Buffer_Data,                             /* OUT    */
    size_t                *Payload_Size,                            /* OUT    */
    void                 (*Callback_Buffer_Lock)  (void *Argument), /* IN     */
    void                 (*Callback_Buffer_Ready) (void *Argument)  /* IN     */
);

GigEVision_Status_t GigEVision_Stream_Stop (
    GigEVision_Context_t *Context /* IN */
);

GigEVision_Status_t GigEVision_Close (
    GigEVision_Context_t *Context /* IN */
);

#ifdef __cplusplus
}
#endif

#endif
