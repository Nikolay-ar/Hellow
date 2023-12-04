/*
 * Оrganization: GosNIIAS
 * Year        : 2023
 * Author      : Vorobev Alexey Vitalievich
 * EMail       : vorobev_av@gosniias.ru
 */

#if 0
#define GIGEVISION_SUPPRESS_WARNING_RESERVED_ID_MACRO
#endif

#ifdef GIGEVISION_SUPPRESS_WARNING_RESERVED_ID_MACRO
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreserved-id-macro"
#endif

#if 1
#define _GNU_SOURCE
#endif

#ifdef GIGEVISION_SUPPRESS_WARNING_RESERVED_ID_MACRO
#pragma GCC diagnostic pop
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <math.h>

#include "GigEVision.h"
#include "GigEVision_Packet_Control.h"
#include "GigEVision_Packet_Stream.h"
#include "GigEVision_Commands.h"

/*
 *
 */

#ifdef GIGEVISION_TRACE
#ifndef GIGEVISION_DEBUG
#define GIGEVISION_DEBUG
#endif
#endif

/*
 *
 */

#define GIGEVISION_DEFAULT_SOCKET_HANDLER  (-1)
#define GIGEVISION_DEFAULT_SOCKET_PROTOCOL ( 0)

#define GIGEVISION_FLAGS_SEND    0
#define GIGEVISION_FLAGS_RECEIVE 0

#define GIGEVISION_HANDLER_IS_CLOSED_SUCCESSFULLY 0

#if 0
#define GIGEVISION_USE_ANY_SOURCE_ADDRESS
#endif

#define GIGEVISION_PRINT_CHUNK_SIZE 16

#define GIGEVISION_REQUEST_SIZE_DISCOVERY      0
#define GIGEVISION_REQUEST_SIZE_WRITE_REGISTER (sizeof (GigEVision_Packet_Control_Data_WriteRegister_t))

#define GIGEVISION_STREAM_PACKETS_NUMBER_BAYER_RG8_2064x1544_LEADER  1
#define GIGEVISION_STREAM_PACKETS_NUMBER_BAYER_RG8_2064x1544_PAYLOAD 2277
#define GIGEVISION_STREAM_PACKETS_NUMBER_BAYER_RG8_2064x1544_TRAILER 1

#define GIGEVISION_STREAM_PACKETS_NUMBER_MONO8_2464x2056_LEADER  1
#define GIGEVISION_STREAM_PACKETS_NUMBER_MONO8_2464x2056_PAYLOAD 3619
#define GIGEVISION_STREAM_PACKETS_NUMBER_MONO8_2464x2056_TRAILER 1

/*
 *
 */

#if 1
#define GIGEVISION_TRACE_PRINT_RAW_PACKET_BYTES
#endif

/*
 *
 */

#if 1
#define GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
#endif

#if 1
#define GIGEVISION_SUPPRESS_WARNING_COVERED_SWITCH_DEFAULT
#endif

/*
 *
 */

static volatile sig_atomic_t Receive_Stream = 1;
static volatile sig_atomic_t Receive_Image  = 1;

/*
 *
 */

#if 1
#define GIGEVISION_PRINT_SIGNAL_NUMBER          /* НОМЕР СИГНАЛА ПЕЧАТИ GIGEVISION */
#endif

static void SignalHandler (int32_t Signal) {    /* Обработчик сигналов */
#ifdef GIGEVISION_PRINT_SIGNAL_NUMBER
    fprintf (stdout, "Signal: %d\n", Signal);
#else
    (void) Signal;
#endif
    Receive_Stream = 0;
    Receive_Image  = 0;
}

/*
 *
 */

#if 0

static void Print_Line (void) {
    fprintf (stdout, "--------------------------------------------------------------------------------\n");
}

#endif

/*
 *
 */

static void Print_Bytes (       /* Печатать байты */
    const uint8_t *Data,
    size_t         Size,
    boolean_t      PrintLineEnd
) {

    size_t Index = 0;

    for (
        Index = 0;
        Index < Size;
        Index++
    ) {

        fprintf (stdout, "%02X", *(Data + Index));

        if ((Index + 1) < Size) {
            fprintf (stdout, " ");
        }

    }

    if (PrintLineEnd == TRUE) {
        fprintf (stdout, "\n");
    }

}

/*
 *
 */

static void Print_Characters (       /* Печатать символы */
    const uint8_t *Data,
    size_t         Size,
    boolean_t      PrintLineEnd
) {

    size_t Index = 0;

    for (
        Index = 0;
        Index < Size;
        Index++
    ) {
        if (
            *(Data + Index) >= 0x20 && /* " " */
            *(Data + Index) <= 0x7E    /* "~" */
        ) {
            fprintf (stdout, "%c", *(Data + Index));
        }
        else {
            fprintf (stdout, ".");
        }
    }

    if (PrintLineEnd == TRUE) {
        fprintf (stdout, "\n");
    }

}

/*
 *
 */

#if 0

static void Print_BytesAndCharacters (
    const uint8_t *Data,
    size_t         Size,
    boolean_t      PrintLineEnd
) {

    Print_Bytes (Data, Size, FALSE);

    fprintf (stdout, " ");

    Print_Characters (Data, Size, FALSE);

    if (PrintLineEnd == TRUE) {
        fprintf (stdout, "\n");
    }

}

#endif

/*
 *
 */

static void Print_Counters (            /* Счетчики печати */
    const GigEVision_Context_t *Context, /* IN */
    const char                 *Tag      /* IN */
) {

    fprintf (stdout, "[%s] Context->Counter_Requests_Total_High                                : %lu\n", Tag, Context->Counter_Requests_Total_High);
    fprintf (stdout, "[%s] Context->Counter_Requests_Total_Low                                 : %u\n",  Tag, Context->Counter_Requests_Total_Low);

    fprintf (stdout, "[%s] Context->Counter_Requests_Discovery                                 : %lu\n", Tag, Context->Counter_Requests_Discovery);
    fprintf (stdout, "[%s] Context->Counter_Requests_WriteRegister_ExclusiveAccess_On          : %lu\n", Tag, Context->Counter_Requests_WriteRegister_ExclusiveAccess_On);
    fprintf (stdout, "[%s] Context->Counter_Requests_WriteRegister_ExclusiveAccess_Off         : %lu\n", Tag, Context->Counter_Requests_WriteRegister_ExclusiveAccess_Off);
    fprintf (stdout, "[%s] Context->Counter_Requests_WriteRegister_HeartbeatDisable_On         : %lu\n", Tag, Context->Counter_Requests_WriteRegister_HeartbeatDisable_On);
    fprintf (stdout, "[%s] Context->Counter_Requests_WriteRegister_HeartbeatDisable_Off        : %lu\n", Tag, Context->Counter_Requests_WriteRegister_HeartbeatDisable_Off);
    fprintf (stdout, "[%s] Context->Counter_Requests_WriteRegister_BalanceWhiteAuto_Continuous : %lu\n", Tag, Context->Counter_Requests_WriteRegister_BalanceWhiteAuto_Continuous);
    fprintf (stdout, "[%s] Context->Counter_Requests_WriteRegister_AcquisitionMode_Continous   : %lu\n", Tag, Context->Counter_Requests_WriteRegister_AcquisitionMode_Continous);
    fprintf (stdout, "[%s] Context->Counter_Requests_WriteRegister_AcquisitionMode_SingleFrame : %lu\n", Tag, Context->Counter_Requests_WriteRegister_AcquisitionMode_SingleFrame);
    fprintf (stdout, "[%s] Context->Counter_Requests_WriteRegister_StreamDestination_Address   : %lu\n", Tag, Context->Counter_Requests_WriteRegister_StreamDestination_Address);
    fprintf (stdout, "[%s] Context->Counter_Requests_WriteRegister_StreamDestination_Port      : %lu\n", Tag, Context->Counter_Requests_WriteRegister_StreamDestination_Port);
    fprintf (stdout, "[%s] Context->Counter_Requests_WriteRegister_AcquisitionEnable_On        : %lu\n", Tag, Context->Counter_Requests_WriteRegister_AcquisitionEnable_On);
    fprintf (stdout, "[%s] Context->Counter_Requests_WriteRegister_AcquisitionEnable_Off       : %lu\n", Tag, Context->Counter_Requests_WriteRegister_AcquisitionEnable_Off);

    fprintf (stdout, "[%s] Context->Counter_Responses_Discovery                                : %lu\n", Tag, Context->Counter_Responses_Discovery);
    fprintf (stdout, "[%s] Context->Counter_Responses_WriteRegister_ExclusiveAccess_On         : %lu\n", Tag, Context->Counter_Responses_WriteRegister_ExclusiveAccess_On);
    fprintf (stdout, "[%s] Context->Counter_Responses_WriteRegister_ExclusiveAccess_Off        : %lu\n", Tag, Context->Counter_Responses_WriteRegister_ExclusiveAccess_Off);
    fprintf (stdout, "[%s] Context->Counter_Responses_WriteRegister_HeartbeatDisable_On        : %lu\n", Tag, Context->Counter_Responses_WriteRegister_HeartbeatDisable_On);
    fprintf (stdout, "[%s] Context->Counter_Responses_WriteRegister_HeartbeatDisable_Off       : %lu\n", Tag, Context->Counter_Responses_WriteRegister_HeartbeatDisable_Off);
    fprintf (stdout, "[%s] Context->Counter_Responses_WriteRegister_BalanceWhiteAuto_Continuous: %lu\n", Tag, Context->Counter_Responses_WriteRegister_BalanceWhiteAuto_Continuous);
    fprintf (stdout, "[%s] Context->Counter_Responses_WriteRegister_AcquisitionMode_Continous  : %lu\n", Tag, Context->Counter_Responses_WriteRegister_AcquisitionMode_Continous);
    fprintf (stdout, "[%s] Context->Counter_Responses_WriteRegister_AcquisitionMode_SingleFrame: %lu\n", Tag, Context->Counter_Responses_WriteRegister_AcquisitionMode_SingleFrame);
    fprintf (stdout, "[%s] Context->Counter_Responses_WriteRegister_StreamDestination_Address  : %lu\n", Tag, Context->Counter_Responses_WriteRegister_StreamDestination_Address);
    fprintf (stdout, "[%s] Context->Counter_Responses_WriteRegister_StreamDestination_Port     : %lu\n", Tag, Context->Counter_Responses_WriteRegister_StreamDestination_Port);
    fprintf (stdout, "[%s] Context->Counter_Responses_WriteRegister_AcquisitionEnable_On       : %lu\n", Tag, Context->Counter_Responses_WriteRegister_AcquisitionEnable_On);
    fprintf (stdout, "[%s] Context->Counter_Responses_WriteRegister_AcquisitionEnable_Off      : %lu\n", Tag, Context->Counter_Responses_WriteRegister_AcquisitionEnable_Off);

    fprintf (stdout, "[%s] Context->Counter_Data_Image_Total                                   : %ld\n", Tag, Context->Counter_Data_Image_Total);
    fprintf (stdout, "[%s] Context->Counter_Data_Image_Leader                                  : %ld\n", Tag, Context->Counter_Data_Image_Leader);
    fprintf (stdout, "[%s] Context->Counter_Data_Image_Payload_Total                           : %ld\n", Tag, Context->Counter_Data_Image_Payload_Total);
    fprintf (stdout, "[%s] Context->Counter_Data_Image_Payload_Current                         : %ld\n", Tag, Context->Counter_Data_Image_Payload_Current);
    fprintf (stdout, "[%s] Context->Counter_Data_Image_Trailer                                 : %ld\n", Tag, Context->Counter_Data_Image_Trailer);
    fprintf (stdout, "[%s] Context->Counter_Data_Image_Unknown                                 : %ld\n", Tag, Context->Counter_Data_Image_Unknown);

    fprintf (stdout, "[%s] Context->Counter_Cycles_Receive_Image_Total                         : %ld\n", Tag, Context->Counter_Cycles_Receive_Image_Total);
    fprintf (stdout, "[%s] Context->Counter_Cycles_Receive_Image_Empty                         : %ld\n", Tag, Context->Counter_Cycles_Receive_Image_Empty);

    fprintf (stdout, "[%s] Context->Counter_Cycles_Receive_Stream_Total                        : %ld\n", Tag, Context->Counter_Cycles_Receive_Stream_Total);
    fprintf (stdout, "[%s] Context->Counter_Cycles_Receive_Stream_Empty                        : %ld\n", Tag, Context->Counter_Cycles_Receive_Stream_Empty);

}

/*
 *
 */

static void GigEVision_Increment_Counter_Requests_Totals (            /* Общее количество запросов счетчика увеличения GigEVision */
    size_t   *Counter_Requests_Total_High, /* IN/OUT */
    uint16_t *Counter_Requests_Total_Low   /* IN/OUT */
) {
    if (
        (*Counter_Requests_Total_Low + 1) == (GIGEVISION_SIZE_COUNTER_REQUESTS_TOTAL_LOW_MAXIMUM + 1)
    ) {

        *Counter_Requests_Total_High = *Counter_Requests_Total_High + 1;
        *Counter_Requests_Total_Low  = 0;

        fprintf (stdout, "[GigEVision_Increment_Counter_Requests_Totals] *Counter_Requests_Total_High: %lu\n", *Counter_Requests_Total_High);
        fprintf (stdout, "[GigEVision_Increment_Counter_Requests_Totals] *Counter_Requests_Total_Low : %d\n",  *Counter_Requests_Total_Low);

    }
    else {
        *Counter_Requests_Total_Low = *Counter_Requests_Total_Low + 1;
    }
}

/*
 *
 */

static GigEVision_Status_t GigEVision_SendCommand_Discovery (            /* Обнаружение команды отправки GigEVision */
    GigEVision_Context_t *Context /* IN/OUT */
) {

    GigEVision_Packet_Control_t Packet_Control_Request_Data;

    size_t  Packet_Control_Request_Size_Data = 0;
    ssize_t Packet_Control_Request_Size_Sent = 0;

    /*
     *
     */

    memset (&Packet_Control_Request_Data, 0, sizeof (Packet_Control_Request_Data));

    /*
     *
     */

    GigEVision_Increment_Counter_Requests_Totals (
        &(Context->Counter_Requests_Total_High),
        &(Context->Counter_Requests_Total_Low)
    );

    Context->Counter_Requests_Discovery++;

    Packet_Control_Request_Data.Header.Packet_Type  = GIGEVISION_PACKET_CONTROL_TYPE_CMD;
    Packet_Control_Request_Data.Header.Packet_Flags = GIGEVISION_PACKET_CONTROL_FLAGS_ACK_REQUIRED | GIGEVISION_PACKET_CONTROL_DISCOVERY_FLAGS_ALLOW_BROADCAST_ACK;
    Packet_Control_Request_Data.Header.Command      = htons (GIGEVISION_PACKET_CONTROL_COMMAND_DISCOVERY_CMD);
    Packet_Control_Request_Data.Header.Size         = htons (GIGEVISION_REQUEST_SIZE_DISCOVERY);
    Packet_Control_Request_Data.Header.ID           = htons (Context->Counter_Requests_Total_Low);

    Packet_Control_Request_Size_Data = sizeof (Packet_Control_Request_Data.Header);

    Packet_Control_Request_Size_Sent =
        sendto (
             Context->Socket_Handler_Control,
            (const void *) &Packet_Control_Request_Data,
             Packet_Control_Request_Size_Data,
             GIGEVISION_FLAGS_SEND,
            (sockaddr_t *) &(Context->Socket_Address_Control_Destination_Data_Parameter),
             Context->Socket_Address_Control_Destination_Size_Parameter
        );

    if (
        Packet_Control_Request_Size_Sent < 0
    ) {
        perror ("[GigEVision_SendCommand_Discovery] sendto()");
        return (GIGEVISION_STATUS_FAILED);
    }

#ifdef GIGEVISION_TRACE
    fprintf (stdout, "[GigEVision_SendCommand_Discovery] Packet_Control_Request_Size_Data: %lu\n", Packet_Control_Request_Size_Data);
    fprintf (stdout, "[GigEVision_SendCommand_Discovery] Packet_Control_Request_Size_Sent: %lu\n", Packet_Control_Request_Size_Sent);
#endif

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

static GigEVision_Status_t GigEVision_ReceiveAcknowledge_Discovery (            /* GigEVision Получает подтверждение обнаружения */
    GigEVision_Context_t *Context /* IN/OUT */
) {

    GigEVision_Packet_Control_t Packet_Control_Response_Data;

    size_t  Packet_Control_Response_Size_Data     = 0;
    ssize_t Packet_Control_Response_Size_Received = 0;

    uint32_t  Index_Chunks     = 0;
    uint32_t  Index_Spaces     = 0;
    float64_t Chunks_Number    = 0;
    uint32_t  Chunks_Remainder = 0;
    size_t    Chunk_Size       = 0;

    /*
     *
     */

    memset (&Packet_Control_Response_Data,  0, sizeof (Packet_Control_Response_Data));

    Packet_Control_Response_Size_Data = sizeof (Packet_Control_Response_Data);

    Packet_Control_Response_Size_Received =
        recvfrom (
              Context->Socket_Handler_Control,
             (void *) &Packet_Control_Response_Data,
              Packet_Control_Response_Size_Data,
              GIGEVISION_FLAGS_RECEIVE,
             (sockaddr_t *) &(Context->Socket_Address_Control_Response_Data_Result),
            &(Context->Socket_Address_Control_Response_Size_Result)
        );

    if (
        Packet_Control_Response_Size_Received < 0
    ) {
        if (
            errno != EAGAIN &&
            errno != EINTR
        ) {
            perror ("[GigEVision_ReceiveAcknowledge_Discovery] recvfrom()");
            return (GIGEVISION_STATUS_FAILED);
        }
        else {
            fprintf (stdout, "[GigEVision_ReceiveAcknowledge_Discovery] Packet_Control_Response_Size_Received: %lu\n", Packet_Control_Response_Size_Received);
        }
    }

#ifdef GIGEVISION_DEBUG
    fprintf (stdout, "[GigEVision_ReceiveAcknowledge_Discovery] Packet_Control_Response_Size_Data    : %lu\n", Packet_Control_Response_Size_Data);
    fprintf (stdout, "[GigEVision_ReceiveAcknowledge_Discovery] Packet_Control_Response_Size_Received: %ld\n", Packet_Control_Response_Size_Received);
#endif

#ifdef GIGEVISION_TRACE

    fprintf (stdout, "[GigEVision_ReceiveAcknowledge_Discovery] Context->Socket_Address_Control_Response_Size_Result: %u\n", Context->Socket_Address_Control_Response_Size_Result);
    fprintf (stdout, "[GigEVision_ReceiveAcknowledge_Discovery] Context->Socket_Address_Control_Response_Data_Result: ");

    Print_Bytes (
        (const uint8_t *) &(Context->Socket_Address_Control_Response_Data_Result),
         Context->Socket_Address_Control_Response_Size_Result,
         TRUE
    );

#endif

    if (
        Packet_Control_Response_Size_Received == 0
    ) {
        fprintf (stdout, "[GigEVision_ReceiveAcknowledge_Discovery] Packet_Control_Response_Size_Received: 0\n");
        return (GIGEVISION_STATUS_FAILED);
    }

    if (
        Packet_Control_Response_Size_Received > 0
    ) {

        Context->Counter_Responses_Discovery++;

        Chunks_Number    = ceil (((float64_t) Packet_Control_Response_Size_Received) / GIGEVISION_PRINT_CHUNK_SIZE);
        Chunks_Remainder = Packet_Control_Response_Size_Received % GIGEVISION_PRINT_CHUNK_SIZE;

#ifdef GIGEVISION_DEBUG
        fprintf (stdout, "[GigEVision_ReceiveAcknowledge_Discovery] GIGEVISION_PRINT_CHUNK_SIZE: %u\n", GIGEVISION_PRINT_CHUNK_SIZE);
        fprintf (stdout, "[GigEVision_ReceiveAcknowledge_Discovery] Chunks_Number              : %f\n", Chunks_Number);
        fprintf (stdout, "[GigEVision_ReceiveAcknowledge_Discovery] Chunks_Remainder           : %u\n", Chunks_Remainder);
#endif

#ifdef GIGEVISION_TRACE
        fprintf (stdout, "[GigEVision_ReceiveAcknowledge_Discovery] Address: %s\n", inet_ntoa (Context->Socket_Address_Control_Response_Data_Result.sin_addr));
        fprintf (stdout, "[GigEVision_ReceiveAcknowledge_Discovery] Port   : %u\n", ntohs (Context->Socket_Address_Control_Response_Data_Result.sin_port));
#endif

        for (
            Index_Chunks = 0;
            Index_Chunks < Chunks_Number;
            Index_Chunks++
        ) {

            if (
                (Index_Chunks + 1) < Chunks_Number
            ) {
                Chunk_Size = GIGEVISION_PRINT_CHUNK_SIZE;
            }
            else {
                if (Chunks_Remainder != 0) {
                    Chunk_Size = Chunks_Remainder;
                }
                else {
                    Chunk_Size = GIGEVISION_PRINT_CHUNK_SIZE;
                }
            }

            fprintf (stdout, "[GigEVision_ReceiveAcknowledge_Discovery] %04u 0x%04X ", Index_Chunks + 1, Index_Chunks * GIGEVISION_PRINT_CHUNK_SIZE);

            Print_Bytes (
                ((const uint8_t *) &Packet_Control_Response_Data) + Index_Chunks * GIGEVISION_PRINT_CHUNK_SIZE,
                 Chunk_Size,
                 FALSE
            );

            if (Chunk_Size != GIGEVISION_PRINT_CHUNK_SIZE) {
                for (
                    Index_Spaces = 0;
                    Index_Spaces < GIGEVISION_PRINT_CHUNK_SIZE - Chunk_Size;
                    Index_Spaces++
                ) {
                    fprintf (stdout, "   "); /* 2 spaces for byte value + 1 space as separator -> see Print_Bytes() */
                }
            }

            fprintf (stdout, " ");

            Print_Characters (
                ((const uint8_t *) &Packet_Control_Response_Data) + Index_Chunks * GIGEVISION_PRINT_CHUNK_SIZE,
                 Chunk_Size,
                 TRUE
            );

        }

    }

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

static GigEVision_Status_t GigEVision_SendCommand_WriteRegister (           /* GigEVision Отправляет команду на запись в регистр */
    GigEVision_Context_t *Context, /* IN/OUT */
    uint32_t              Address, /* IN     */
    uint32_t              Value,   /* IN     */
    const char           *Tag,     /* IN     */
    size_t               *Counter  /* IN/OUT */
) {

    GigEVision_Packet_Control_t Packet_Control_Request_Data;

    size_t  Packet_Control_Request_Size_Data = 0;
    ssize_t Packet_Control_Request_Size_Sent = 0;

    GigEVision_Packet_Control_Data_WriteRegister_t Packet_Control_Data_WriteRegister;

    /*
     *
     */

    memset (&Packet_Control_Request_Data, 0, sizeof (Packet_Control_Request_Data));

    memset (&Packet_Control_Data_WriteRegister, 0, sizeof (Packet_Control_Data_WriteRegister));

    GigEVision_Increment_Counter_Requests_Totals (
        &(Context->Counter_Requests_Total_High),
        &(Context->Counter_Requests_Total_Low)
    );

    *Counter = *Counter + 1;

    Packet_Control_Request_Data.Header.Packet_Type  = GIGEVISION_PACKET_CONTROL_TYPE_CMD;
    Packet_Control_Request_Data.Header.Packet_Flags = GIGEVISION_PACKET_CONTROL_FLAGS_ACK_REQUIRED;
    Packet_Control_Request_Data.Header.Command      = htons (GIGEVISION_PACKET_CONTROL_COMMAND_WRITE_REGISTER_CMD);
    Packet_Control_Request_Data.Header.Size         = htons (GIGEVISION_REQUEST_SIZE_WRITE_REGISTER);
    Packet_Control_Request_Data.Header.ID           = htons (Context->Counter_Requests_Total_Low);

    Packet_Control_Data_WriteRegister.Address = htonl (Address);
    Packet_Control_Data_WriteRegister.Value   = htonl (Value);

    memcpy (&(Packet_Control_Request_Data.Data [0]), &Packet_Control_Data_WriteRegister, GIGEVISION_REQUEST_SIZE_WRITE_REGISTER);

    Packet_Control_Request_Size_Data = sizeof (Packet_Control_Request_Data.Header) + GIGEVISION_REQUEST_SIZE_WRITE_REGISTER;

    Packet_Control_Request_Size_Sent =
        sendto (
             Context->Socket_Handler_Control,
            (const void *) &Packet_Control_Request_Data,
             Packet_Control_Request_Size_Data,
             GIGEVISION_FLAGS_SEND,
            (sockaddr_t *) &(Context->Socket_Address_Control_Destination_Data_Parameter),
             Context->Socket_Address_Control_Destination_Size_Parameter
        );

    if (
        Packet_Control_Request_Size_Sent < 0
    ) {
        fprintf (stdout, "[GigEVision_SendCommand_WriteRegister] [%s] [0x%08X 0x%08X] Packet_Control_Request_Size_Sent: %lu\n", Tag, Address, Value, Packet_Control_Request_Size_Sent);
        perror ("[GigEVision_SendCommand_WriteRegister] sendto()");
        return (GIGEVISION_STATUS_FAILED);
    }

#ifdef GIGEVISION_TRACE
    fprintf (stdout, "[GigEVision_SendCommand_WriteRegister] [%s] [0x%08X 0x%08X] Packet_Control_Request_Size_Data: %lu\n", Tag, Address, Value, Packet_Control_Request_Size_Data);
    fprintf (stdout, "[GigEVision_SendCommand_WriteRegister] [%s] [0x%08X 0x%08X] Packet_Control_Request_Size_Sent: %lu\n", Tag, Address, Value, Packet_Control_Request_Size_Sent);
#endif

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

static GigEVision_Status_t GigEVision_ReceiveAcknowledge_WriteRegister (           /* GigEVision Получает подтверждение записи Регистрируется */
    GigEVision_Context_t *Context, /* IN     */
    const char           *Tag,     /* IN     */
    size_t               *Counter  /* IN/OUT */
) {

    GigEVision_Packet_Control_t Packet_Control_Response_Data;

    size_t  Packet_Control_Response_Size_Data     = 0;
    ssize_t Packet_Control_Response_Size_Received = 0;

    /*
     *
     */

    memset (&Packet_Control_Response_Data,  0, sizeof (Packet_Control_Response_Data));

    Packet_Control_Response_Size_Data = sizeof (Packet_Control_Response_Data);

    Packet_Control_Response_Size_Received =
        recvfrom (
              Context->Socket_Handler_Control,
             (void *) &Packet_Control_Response_Data,
              Packet_Control_Response_Size_Data,
              GIGEVISION_FLAGS_RECEIVE,
             (sockaddr_t *) &(Context->Socket_Address_Control_Response_Data_Result),
            &(Context->Socket_Address_Control_Response_Size_Result)
        );

    if (
        Packet_Control_Response_Size_Received < 0
    ) {
        if (
            errno != EAGAIN &&
            errno != EINTR
        ) {
            fprintf (stdout, "[GigEVision_ReceiveAcknowledge_WriteRegister] [%s] Packet_Control_Response_Size_Received: %lu\n", Tag, Packet_Control_Response_Size_Received);
            perror ("[GigEVision_ReceiveAcknowledge_WriteRegister] recvfrom()");
            return (GIGEVISION_STATUS_FAILED);
        }
        else {
            fprintf (stdout, "[GigEVision_ReceiveAcknowledge_WriteRegister] [%s] Packet_Control_Response_Size_Received: %lu\n", Tag, Packet_Control_Response_Size_Received);
        }
    }

#ifdef GIGEVISION_TRACE
    fprintf (stdout, "[GigEVision_ReceiveAcknowledge_WriteRegister] [%s] Packet_Control_Response_Size_Data    : %lu\n", Tag, Packet_Control_Response_Size_Data);
    fprintf (stdout, "[GigEVision_ReceiveAcknowledge_WriteRegister] [%s] Packet_Control_Response_Size_Received: %ld\n", Tag, Packet_Control_Response_Size_Received);
#endif

#ifdef GIGEVISION_TRACE

    fprintf (stdout, "[GigEVision_ReceiveAcknowledge_WriteRegister] [%s] Context->Socket_Address_Control_Response_Size_Result: %u\n", Tag, Context->Socket_Address_Control_Response_Size_Result);
    fprintf (stdout, "[GigEVision_ReceiveAcknowledge_WriteRegister] [%s] Context->Socket_Address_Control_Response_Data_Result: ", Tag);

    Print_Bytes (
        (const uint8_t *) &(Context->Socket_Address_Control_Response_Data_Result),
         Context->Socket_Address_Control_Response_Size_Result,
         TRUE
    );

#endif

    if (
        Packet_Control_Response_Size_Received == 0
    ) {
        fprintf (stdout, "[GigEVision_ReceiveAcknowledge_WriteRegister] [%s] Packet_Control_Response_Size_Received: 0\n", Tag);
        return (GIGEVISION_STATUS_FAILED);
    }

    if (
        Packet_Control_Response_Size_Received > 0
    ) {

        *Counter = *Counter + 1;

        fprintf (stdout, "[GigEVision_ReceiveAcknowledge_WriteRegister] [%s]: ", Tag);

        Print_Bytes (
            (const uint8_t *) &(Packet_Control_Response_Data),
            (size_t) Packet_Control_Response_Size_Received,
             TRUE
        );

    }

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

static GigEVision_Status_t GigEVision_AcquisitionEnable_On (           /* Включение приема GigEVision на */
    GigEVision_Context_t *Context /* IN/OUT */
) {

    /*
     * WRITEREG_CMD: AcquisitionEnable_On
     */

    if (
        GigEVision_SendCommand_WriteRegister (
              Context,
              GIGEVISION_COMMAND_ACQUISITION_ENABLE_ON_ADDRESS,
              GIGEVISION_COMMAND_ACQUISITION_ENABLE_ON_VALUE,
              GIGEVISION_COMMAND_ACQUISITION_ENABLE_ON_TAG,
            &(Context->Counter_Requests_WriteRegister_AcquisitionEnable_On)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_ACK: AcquisitionEnable_On
     */

    if (
        GigEVision_ReceiveAcknowledge_WriteRegister (
              Context,
              GIGEVISION_COMMAND_ACQUISITION_ENABLE_ON_TAG,
            &(Context->Counter_Responses_WriteRegister_AcquisitionEnable_On)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

static GigEVision_Status_t GigEVision_AcquisitionEnable_Off (           /* Включение/выключение сбора данных GigEVision */
    GigEVision_Context_t *Context /* IN/OUT */
) {

    /*
     * WRITEREG_CMD: AcquisitionEnable_Off
     */

    if (
        GigEVision_SendCommand_WriteRegister (
              Context,
              GIGEVISION_COMMAND_ACQUISITION_ENABLE_OFF_ADDRESS,
              GIGEVISION_COMMAND_ACQUISITION_ENABLE_OFF_VALUE,
              GIGEVISION_COMMAND_ACQUISITION_ENABLE_OFF_TAG,
            &(Context->Counter_Requests_WriteRegister_AcquisitionEnable_Off)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_ACK: AcquisitionEnable_Off
     */

    if (
        GigEVision_ReceiveAcknowledge_WriteRegister (
              Context,
              GIGEVISION_COMMAND_ACQUISITION_ENABLE_OFF_TAG,
            &(Context->Counter_Responses_WriteRegister_AcquisitionEnable_Off)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

static GigEVision_Status_t GigEVision_Open_Crevis (           /* Открытый кревис GigEVision */
    GigEVision_Context_t *Context /* IN/OUT */
) {

    /*
     * DISCOVERY_CMD
     */

    if (
        GigEVision_SendCommand_Discovery (
            Context
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * DISCOVERY_ACK
     */

    if (
        GigEVision_ReceiveAcknowledge_Discovery (
            Context
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_CMD: ExclusiveAccess_On
     */

    if (
        GigEVision_SendCommand_WriteRegister (
              Context,
              GIGEVISION_COMMAND_EXCLUSIVE_ACCESS_ON_ADDRESS,
              GIGEVISION_COMMAND_EXCLUSIVE_ACCESS_ON_VALUE,
              GIGEVISION_COMMAND_EXCLUSIVE_ACCESS_ON_TAG,
            &(Context->Counter_Requests_WriteRegister_ExclusiveAccess_On)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_ACK: ExclusiveAccess_On
     */

    if (
        GigEVision_ReceiveAcknowledge_WriteRegister (
              Context,
              GIGEVISION_COMMAND_EXCLUSIVE_ACCESS_ON_TAG,
            &(Context->Counter_Responses_WriteRegister_ExclusiveAccess_On)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_CMD: HeartbeatDisable_On
     */

    if (
        GigEVision_SendCommand_WriteRegister (
              Context,
              GIGEVISION_COMMAND_HEARTBEAT_DISABLE_ON_ADDRESS,
              GIGEVISION_COMMAND_HEARTBEAT_DISABLE_ON_VALUE,
              GIGEVISION_COMMAND_HEARTBEAT_DISABLE_ON_TAG,
            &(Context->Counter_Requests_WriteRegister_HeartbeatDisable_On)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_ACK: HeartbeatDisable_On
     */

    if (
        GigEVision_ReceiveAcknowledge_WriteRegister (
              Context,
              GIGEVISION_COMMAND_HEARTBEAT_DISABLE_ON_TAG,
            &(Context->Counter_Responses_WriteRegister_HeartbeatDisable_On)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_CMD: BalanceWhiteAuto_Continuous
     */

    if (
        GigEVision_SendCommand_WriteRegister (
              Context,
              GIGEVISION_COMMAND_BALANCE_WHITE_AUTO_CONTINUOUS_ADDRESS,
              GIGEVISION_COMMAND_BALANCE_WHITE_AUTO_CONTINUOUS_VALUE,
              GIGEVISION_COMMAND_BALANCE_WHITE_AUTO_CONTINUOUS_TAG,
            &(Context->Counter_Requests_WriteRegister_BalanceWhiteAuto_Continuous)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_ACK: BalanceWhiteAuto_Continuous
     */

    if (
        GigEVision_ReceiveAcknowledge_WriteRegister (
              Context,/*Control*/
              GIGEVISION_COMMAND_BALANCE_WHITE_AUTO_CONTINUOUS_TAG,
            &(Context->Counter_Responses_WriteRegister_BalanceWhiteAuto_Continuous)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_CMD: StreamDestination_Address
     */

    if (
        GigEVision_SendCommand_WriteRegister (
              Context,
              GIGEVISION_COMMAND_STREAM_DESTINATION_ADDRESS_ADDRESS,
              ntohl (Context->Socket_Address_Control_Source_Data_Parameter.sin_addr.s_addr), /* Same as control address */
              GIGEVISION_COMMAND_STREAM_DESTINATION_ADDRESS_TAG,
            &(Context->Counter_Requests_WriteRegister_StreamDestination_Address)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_ACK: StreamDestination_Address
     */

    if (
        GigEVision_ReceiveAcknowledge_WriteRegister (
              Context,
              GIGEVISION_COMMAND_STREAM_DESTINATION_ADDRESS_TAG,
            &(Context->Counter_Responses_WriteRegister_StreamDestination_Address)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_CMD: StreamDestination_Port
     */

    if (
        GigEVision_SendCommand_WriteRegister (
              Context,
              GIGEVISION_COMMAND_STREAM_DESTINATION_PORT_ADDRESS,
              GIGEVISION_PORT_STREAM,
              GIGEVISION_COMMAND_STREAM_DESTINATION_PORT_TAG,
            &(Context->Counter_Requests_WriteRegister_StreamDestination_Port)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_ACK: StreamDestination_Port
     */

    if (
        GigEVision_ReceiveAcknowledge_WriteRegister (
              Context,
              GIGEVISION_COMMAND_STREAM_DESTINATION_PORT_TAG,
            &(Context->Counter_Responses_WriteRegister_StreamDestination_Port)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

static GigEVision_Status_t GigEVision_Open_Prima (           /* Открытая прима GigEVision */
    GigEVision_Context_t *Context /* IN/OUT */
) {

    /*
     * Nothing yet
     */

#if 1
    (void)Context;
#endif

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

const char_t *GigEVision_ToString_Device (           /* Устройство для передачи данных GigEVision toString */
    GigEVision_Device_t Device /* IN */
) {

    switch (Device) {
        case (GIGEVISION_DEVICE_UNKNOWN): {
            return (GIGEVISION_DEVICE_NAME_UNKNOWN);
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
        case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35): {
            return (GIGEVISION_DEVICE_NAME_CREVIS_MG_A320K_35);
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
        case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22): {
            return (GIGEVISION_DEVICE_NAME_CREVIS_MG_A500M_22);
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
        case (GIGEVISION_DEVICE_PRIMA_IVS_70): {
            return (GIGEVISION_DEVICE_NAME_PRIMA_IVS_70);
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
#ifndef GIGEVISION_SUPPRESS_WARNING_COVERED_SWITCH_DEFAULT
        default: {
            return ("UNKNOWN");
            break;
        }
#endif
    }

#ifdef GIGEVISION_SUPPRESS_WARNING_COVERED_SWITCH_DEFAULT
    return ("UNKNOWN");
#endif

}

/*
 *
 */

GigEVision_Status_t GigEVision_Open (           /* Гиговидение открыто */
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
) {

    int32_t Socket_BufferSize_Control_Initial_Value = 0;
    int32_t Socket_BufferSize_Control_Result_Value  = 0;

    int32_t Socket_BufferSize_Stream_Initial_Value = 0;
    int32_t Socket_BufferSize_Stream_Result_Value  = 0;

    socklen_t Socket_BufferSize_Control_Initial_Size = 0;
    socklen_t Socket_BufferSize_Control_Result_Size  = 0;

    socklen_t Socket_BufferSize_Stream_Initial_Size = 0;
    socklen_t Socket_BufferSize_Stream_Result_Size  = 0;

    /*
     *
     */

    Context->Device = Device;

    memcpy (&(Context->Address_Source      [0]), &(Address_Source      [0]), GIGEVISION_SIZE_ADDRESS);
    memcpy (&(Context->Address_Destination [0]), &(Address_Destination [0]), GIGEVISION_SIZE_ADDRESS);

    Context->Socket_Handler_Control = GIGEVISION_DEFAULT_SOCKET_HANDLER;

    Context->Socket_Address_Control_Source_Size_Parameter      = sizeof (Context->Socket_Address_Control_Source_Data_Parameter);
    Context->Socket_Address_Control_Source_Size_Result         = sizeof (Context->Socket_Address_Control_Source_Data_Result);
    Context->Socket_Address_Control_Destination_Size_Parameter = sizeof (Context->Socket_Address_Control_Destination_Data_Parameter);
    Context->Socket_Address_Control_Response_Size_Result       = sizeof (Context->Socket_Address_Control_Response_Data_Result);
    Context->Socket_Address_Stream_Destination_Size_Parameter  = sizeof (Context->Socket_Address_Stream_Destination_Data_Parameter);
    Context->Socket_Address_Stream_Destination_Size_Result     = sizeof (Context->Socket_Address_Stream_Destination_Data_Result);

    memset (&(Context->Socket_Address_Control_Source_Data_Parameter),      0, Context->Socket_Address_Control_Source_Size_Parameter);
    memset (&(Context->Socket_Address_Control_Source_Data_Result),         0, Context->Socket_Address_Control_Source_Size_Result);
    memset (&(Context->Socket_Address_Control_Destination_Data_Parameter), 0, Context->Socket_Address_Control_Destination_Size_Parameter);
    memset (&(Context->Socket_Address_Control_Response_Data_Result),       0, Context->Socket_Address_Control_Response_Size_Result);
    memset (&(Context->Socket_Address_Stream_Destination_Data_Parameter),  0, Context->Socket_Address_Stream_Destination_Size_Parameter);
    memset (&(Context->Socket_Address_Stream_Destination_Data_Result),     0, Context->Socket_Address_Stream_Destination_Size_Result);

    /*
     *
     */

    Context->Socket_Poll_EmptyCycles_Maximum = Socket_Poll_EmptyCycles_Maximum;

    /*
     *
     */

#ifdef GIGEVISION_DEBUG
#if 0
    fprintf (stdout, "[GigEVision_Open] Address_Source              : %s\n", Address_Source);
    fprintf (stdout, "[GigEVision_Open] Address_Destination         : %s\n", Address_Destination);
    fprintf (stdout, "[GigEVision_Open] *Context                    : %p\n", (void *) Context);
    fprintf (stdout, "[GigEVision_Open] sizeof (*Context)           : %lu\n", sizeof (*Context));
    fprintf (stdout, "[GigEVision_Open] GIGEVISION_SIZE_ADDRESS     : %u\n", GIGEVISION_SIZE_ADDRESS);
#endif
    fprintf (stdout, "[GigEVision_Open] Context->Device             : %s\n", GigEVision_ToString_Device (Context->Device));
    fprintf (stdout, "[GigEVision_Open] Context->Address_Source     : %s\n", Context->Address_Source);
    fprintf (stdout, "[GigEVision_Open] Context->Address_Destination: %s\n", Context->Address_Destination);
#endif

    /*
     * Socket: Control
     */

    Context->Socket_Handler_Control =
        socket (
            PF_INET,
            SOCK_DGRAM,
            GIGEVISION_DEFAULT_SOCKET_PROTOCOL
        );

    if (Context->Socket_Handler_Control < 0) {
        perror ("[GigEVision_Open] [Control] socket()");
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * Control: SO_RCVTIMEO
     */

    Context->Socket_ReadTimeout_Control.tv_sec  = Socket_ReadTimeout_Control_Seconds;
    Context->Socket_ReadTimeout_Control.tv_usec = Socket_ReadTimeout_Control_Microseconds;

    if (
        setsockopt (
             Context->Socket_Handler_Control,
             SOL_SOCKET,
             SO_RCVTIMEO,
            &(Context->Socket_ReadTimeout_Control),
             sizeof (Context->Socket_ReadTimeout_Control)
        ) < 0
    ) {
        perror ("[GigEVision_Open] [Control] setsockopt(SO_RCVTIMEO)");
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * Control: SO_RCVBUF
     */

    Socket_BufferSize_Control_Initial_Size = sizeof (Socket_BufferSize_Control_Initial_Value);

    if (
        getsockopt (
             Context->Socket_Handler_Control,
             SOL_SOCKET,
             SO_RCVBUF,
            &(Socket_BufferSize_Control_Initial_Value),
            &(Socket_BufferSize_Control_Initial_Size)
        ) < 0
    ) {
        perror ("[GigEVision_Open] [Control] getsockopt(SO_RCVBUF, Initial)");
        return (GIGEVISION_STATUS_FAILED);
    }

    Context->Socket_BufferSize_Control_Initial = Socket_BufferSize_Control_Initial_Value;

#ifdef GIGEVISION_DEBUG
    fprintf (stdout, "[GigEVision_Open] [Control] Socket_BufferSize_Control_Initial_Value: %d\n", Socket_BufferSize_Control_Initial_Value);
    fprintf (stdout, "[GigEVision_Open] [Control] Socket_BufferSize_Control_Initial_Size : %d\n", Socket_BufferSize_Control_Initial_Size);
#endif

    if (
        setsockopt (
             Context->Socket_Handler_Control,
             SOL_SOCKET,
             SO_RCVBUF,
            &(Socket_BufferSize_Control_Target),
             sizeof (Socket_BufferSize_Control_Target)
        ) < 0
    ) {
        perror ("[GigEVision_Open] [Control] setsockopt(SO_RCVBUF, Target)");
        return (GIGEVISION_STATUS_FAILED);
    }

    Context->Socket_BufferSize_Control_Target = Socket_BufferSize_Control_Target;

#ifdef GIGEVISION_DEBUG
    fprintf (stdout, "[GigEVision_Open] [Control] Socket_BufferSize_Control_Target       : %d\n", Socket_BufferSize_Control_Target);
#endif

    Socket_BufferSize_Control_Result_Size = sizeof (Socket_BufferSize_Control_Result_Value);

    if (
        getsockopt (
             Context->Socket_Handler_Control,
             SOL_SOCKET,
             SO_RCVBUF,
            &(Socket_BufferSize_Control_Result_Value),
            &(Socket_BufferSize_Control_Result_Size)
        ) < 0
    ) {
        perror ("[GigEVision_Open] [Control] getsockopt(SO_RCVBUF, Result)");
        return (GIGEVISION_STATUS_FAILED);
    }

    Context->Socket_BufferSize_Control_Result = Socket_BufferSize_Control_Result_Value;

#ifdef GIGEVISION_DEBUG
    fprintf (stdout, "[GigEVision_Open] [Control] Socket_BufferSize_Control_Result_Value : %d\n", Socket_BufferSize_Control_Result_Value);
    fprintf (stdout, "[GigEVision_Open] [Control] Socket_BufferSize_Control_Result_Size  : %d\n", Socket_BufferSize_Control_Result_Size);
#endif

    /*
     *
     */

    Context->Socket_Address_Control_Source_Data_Parameter.sin_family      = AF_INET;
    Context->Socket_Address_Control_Source_Data_Parameter.sin_port        = htons (GIGEVISION_PORT_CONTROL);
#ifdef GIGEVISION_USE_ANY_SOURCE_ADDRESS
    Context->Socket_Address_Control_Source_Data_Parameter.sin_addr.s_addr = htonl (INADDR_ANY);
#else
    Context->Socket_Address_Control_Source_Data_Parameter.sin_addr.s_addr = inet_addr (Context->Address_Source);
#endif

    if (
        bind (
             Context->Socket_Handler_Control,
            (sockaddr_t *) &(Context->Socket_Address_Control_Source_Data_Parameter),
             Context->Socket_Address_Control_Source_Size_Parameter
        ) < 0
    ) {
        perror ("[GigEVision_Open] [Control] bind()");
        return (GIGEVISION_STATUS_FAILED);
    }

#ifdef GIGEVISION_TRACE

    fprintf (stdout, "[GigEVision_Open] [Control] Context->Socket_Address_Control_Source_Size_Parameter: %u\n", Context->Socket_Address_Control_Source_Size_Parameter);
    fprintf (stdout, "[GigEVision_Open] [Control] Context->Socket_Address_Control_Source_Data_Parameter: ");

    Print_Bytes (
        (const uint8_t *) &(Context->Socket_Address_Control_Source_Data_Parameter),
         Context->Socket_Address_Control_Source_Size_Parameter,
         TRUE
    );

#endif

    if (
        getsockname (
             Context->Socket_Handler_Control,
            (sockaddr_t *) &(Context->Socket_Address_Control_Source_Data_Result),
            &(Context->Socket_Address_Control_Source_Size_Result)
        ) < 0
    ) {
        perror ("[GigEVision_Open] [Control] getsockname()");
        return (GIGEVISION_STATUS_FAILED);
    }

#ifdef GIGEVISION_TRACE

    fprintf (stdout, "[GigEVision_Open] [Control] Context->Socket_Address_Control_Source_Size_Result: %u\n", Context->Socket_Address_Control_Source_Size_Result);
    fprintf (stdout, "[GigEVision_Open] [Control] Context->Socket_Address_Control_Source_Data_Result: ");

    Print_Bytes (
        (const uint8_t *) &(Context->Socket_Address_Control_Source_Data_Result),
         Context->Socket_Address_Control_Source_Size_Result,
         TRUE
    );

#endif

#ifdef GIGEVISION_DEBUG
    fprintf (stdout, "[GigEVision_Open] [Control] Context->Socket_Address_Control_Source_Data_Result.sin_addr: %s\n", inet_ntoa (Context->Socket_Address_Control_Source_Data_Result.sin_addr));
    fprintf (stdout, "[GigEVision_Open] [Control] Context->Socket_Address_Control_Source_Data_Result.sin_port: %u\n", ntohs (Context->Socket_Address_Control_Source_Data_Result.sin_port));
#endif

    /*
     * Socket: Stream
     */

    Context->Socket_Handler_Stream =
        socket (
            PF_INET,
            SOCK_DGRAM,
            GIGEVISION_DEFAULT_SOCKET_PROTOCOL
        );

    if (Context->Socket_Handler_Stream < 0) {
        perror ("[GigEVision_Open] [Stream] socket()");
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * Stream: SO_RCVTIMEO
     */

    Context->Socket_ReadTimeout_Stream.tv_sec  = Socket_ReadTimeout_Stream_Seconds;
    Context->Socket_ReadTimeout_Stream.tv_usec = Socket_ReadTimeout_Stream_Microseconds;

    if (
        setsockopt (
             Context->Socket_Handler_Stream,
             SOL_SOCKET,
             SO_RCVTIMEO,
            &(Context->Socket_ReadTimeout_Stream),
             sizeof (Context->Socket_ReadTimeout_Stream)
        ) < 0
    ) {
        perror ("[GigEVision_Open] [Stream] setsockopt(SO_RCVTIMEO)");
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * Stream: SO_RCVBUF
     */

    Socket_BufferSize_Stream_Initial_Size = sizeof (Socket_BufferSize_Stream_Initial_Value);

    if (
        getsockopt (
             Context->Socket_Handler_Stream,
             SOL_SOCKET,
             SO_RCVBUF,
            &(Socket_BufferSize_Stream_Initial_Value),
            &(Socket_BufferSize_Stream_Initial_Size)
        ) < 0
    ) {
        perror ("[GigEVision_Open] [Stream] getsockopt(SO_RCVBUF, Initial)");
        return (GIGEVISION_STATUS_FAILED);
    }

    Context->Socket_BufferSize_Stream_Initial = Socket_BufferSize_Stream_Initial_Value;

#ifdef GIGEVISION_DEBUG
    fprintf (stdout, "[GigEVision_Open] [Stream] Socket_BufferSize_Stream_Initial_Value: %d\n", Socket_BufferSize_Stream_Initial_Value);
    fprintf (stdout, "[GigEVision_Open] [Stream] Socket_BufferSize_Stream_Initial_Size : %d\n", Socket_BufferSize_Stream_Initial_Size);
#endif

    if (
        setsockopt (
             Context->Socket_Handler_Stream,
             SOL_SOCKET,
             SO_RCVBUF,
            &(Socket_BufferSize_Stream_Target),
             sizeof (Socket_BufferSize_Stream_Target)
        ) < 0
    ) {
        perror ("[GigEVision_Open] [Stream] setsockopt(SO_RCVBUF, Target)");
        return (GIGEVISION_STATUS_FAILED);
    }

    Context->Socket_BufferSize_Stream_Target = Socket_BufferSize_Stream_Target;

#ifdef GIGEVISION_DEBUG
    fprintf (stdout, "[GigEVision_Open] [Stream] Socket_BufferSize_Stream_Target       : %d\n", Socket_BufferSize_Stream_Target);
#endif

    Socket_BufferSize_Stream_Result_Size = sizeof (Socket_BufferSize_Stream_Result_Value);

    if (
        getsockopt (
             Context->Socket_Handler_Stream,
             SOL_SOCKET,
             SO_RCVBUF,
            &(Socket_BufferSize_Stream_Result_Value),
            &(Socket_BufferSize_Stream_Result_Size)
        ) < 0
    ) {
        perror ("[GigEVision_Open] [Stream] getsockopt(SO_RCVBUF, Result)");
        return (GIGEVISION_STATUS_FAILED);
    }

    Context->Socket_BufferSize_Stream_Result = Socket_BufferSize_Stream_Result_Value;

#ifdef GIGEVISION_DEBUG
    fprintf (stdout, "[GigEVision_Open] [Stream] Socket_BufferSize_Stream_Result_Value : %d\n", Socket_BufferSize_Stream_Result_Value);
    fprintf (stdout, "[GigEVision_Open] [Stream] Socket_BufferSize_Stream_Result_Size  : %d\n", Socket_BufferSize_Stream_Result_Size);
#endif

    /*
     *
     */

    Context->Socket_Address_Stream_Destination_Data_Parameter.sin_family = AF_INET;

    switch (Context->Device) {
        case (GIGEVISION_DEVICE_PRIMA_IVS_70): {
            Context->Socket_Address_Stream_Destination_Data_Parameter.sin_port = htons (GIGEVISION_PORT_STREAM_PRIMA);
            break;
        }
        case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35):
        case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22):
        default: {
            Context->Socket_Address_Stream_Destination_Data_Parameter.sin_port = htons (GIGEVISION_PORT_STREAM);
            break;
        }
    }

#ifdef GIGEVISION_USE_ANY_SOURCE_ADDRESS
    Context->Socket_Address_Stream_Destination_Data_Parameter.sin_addr.s_addr = htonl (INADDR_ANY);
#else
    Context->Socket_Address_Stream_Destination_Data_Parameter.sin_addr.s_addr = inet_addr (Context->Address_Source);
#endif

    if (
        bind (
             Context->Socket_Handler_Stream,
            (sockaddr_t *) &(Context->Socket_Address_Stream_Destination_Data_Parameter),
             Context->Socket_Address_Stream_Destination_Size_Parameter
        ) < 0
    ) {
        perror ("[GigEVision_Open] [Stream] bind()");
        return (GIGEVISION_STATUS_FAILED);
    }

#ifdef GIGEVISION_TRACE

    fprintf (stdout, "[GigEVision_Open] [Stream] Context->Socket_Address_Stream_Destination_Size_Parameter: %u\n", Context->Socket_Address_Stream_Destination_Size_Parameter);
    fprintf (stdout, "[GigEVision_Open] [Stream] Context->Socket_Address_Stream_Destination_Data_Parameter: ");

    Print_Bytes (
        (const uint8_t *) &(Context->Socket_Address_Stream_Destination_Data_Parameter),
         Context->Socket_Address_Stream_Destination_Size_Parameter,
         TRUE
    );

#endif

    if (
        getsockname (
             Context->Socket_Handler_Stream,
            (sockaddr_t *) &(Context->Socket_Address_Stream_Destination_Data_Result),
            &(Context->Socket_Address_Stream_Destination_Size_Result)
        ) < 0
    ) {
        perror ("[GigEVision_Open] [Stream] getsockname()");
        return (GIGEVISION_STATUS_FAILED);
    }

#ifdef GIGEVISION_TRACE

    fprintf (stdout, "[GigEVision_Open] [Stream] Context->Socket_Address_Stream_Destination_Size_Result: %u\n", Context->Socket_Address_Stream_Destination_Size_Result);
    fprintf (stdout, "[GigEVision_Open] [Stream] Context->Socket_Address_Stream_Destination_Data_Result: ");

    Print_Bytes (
        (const uint8_t *) &(Context->Socket_Address_Stream_Destination_Data_Result),
         Context->Socket_Address_Stream_Destination_Size_Result,
         TRUE
    );

#endif

#ifdef GIGEVISION_DEBUG
    fprintf (stdout, "[GigEVision_Open] [Stream] Context->Socket_Address_Stream_Destination_Data_Result.sin_addr: %s\n", inet_ntoa (Context->Socket_Address_Stream_Destination_Data_Result.sin_addr));
    fprintf (stdout, "[GigEVision_Open] [Stream] Context->Socket_Address_Stream_Destination_Data_Result.sin_port: %u\n", ntohs (Context->Socket_Address_Stream_Destination_Data_Result.sin_port));
#endif

    /*
     * Socket: Destination address
     */

    Context->Socket_Address_Control_Destination_Data_Parameter.sin_family      = AF_INET;
    Context->Socket_Address_Control_Destination_Data_Parameter.sin_port        = htons     (GIGEVISION_PORT_CONTROL);
    Context->Socket_Address_Control_Destination_Data_Parameter.sin_addr.s_addr = inet_addr (Context->Address_Destination);

#ifdef GIGEVISION_TRACE

    fprintf (stdout, "[GigEVision_Open] [Control] Context->Socket_Address_Control_Destination_Size_Parameter: %u\n", Context->Socket_Address_Control_Destination_Size_Parameter);
    fprintf (stdout, "[GigEVision_Open] [Control] Context->Socket_Address_Control_Destination_Data_Parameter: ");

    Print_Bytes (
        (const uint8_t *) &(Context->Socket_Address_Control_Destination_Data_Parameter),
         Context->Socket_Address_Control_Destination_Size_Parameter,
         TRUE
    );

#endif

    /*
     *
     */

    switch (Context->Device) {
        case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35):
        case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22): {
            if (
                GigEVision_Open_Crevis (Context) != GIGEVISION_STATUS_OK
            ) {
                return (GIGEVISION_STATUS_FAILED);
            }
            break;
        }
        case (GIGEVISION_DEVICE_PRIMA_IVS_70): {
            if (
                GigEVision_Open_Prima (Context) != GIGEVISION_STATUS_OK
            ) {
                return (GIGEVISION_STATUS_FAILED);
            }
            break;
        }
        default: {
            fprintf (stdout, "[GigEVision_Open] [UNKNOWN] Context->Device: %d\n", Context->Device);
            return (GIGEVISION_STATUS_FAILED);
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
    }

    /*
     *
     */

    signal (SIGINT, SignalHandler);

    /*
     *
     */

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

const char_t *GigEVision_ToString_Status (           /* Преобразование гигабайта в строковый статус */
    GigEVision_Status_t Status /* IN */
) {

    switch (Status) {
        case (GIGEVISION_STATUS_UNKNOWN): {
            return ("GIGEVISION_STATUS_UNKNOWN");
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
        case (GIGEVISION_STATUS_FAILED): {
            return ("GIGEVISION_STATUS_FAILED");
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
        case (GIGEVISION_STATUS_OK): {
            return ("GIGEVISION_STATUS_OK");
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
#ifndef GIGEVISION_SUPPRESS_WARNING_COVERED_SWITCH_DEFAULT
        default: {
            return ("UNKNOWN");
            break;
        }
#endif
    }

#ifdef GIGEVISION_SUPPRESS_WARNING_COVERED_SWITCH_DEFAULT
    return ("UNKNOWN");
#endif

}

/*
 *
 */

const char_t *GigEVision_ToString_AcquisitionMode (           /* GigEVision В режиме получения строк */
    GigEVision_AcquisitionMode_t AcquisitionMode /* IN */
) {

    switch (AcquisitionMode) {
        case (GIGEVISION_ACQUISITION_MODE_UNKNOWN): {
            return ("GIGEVISION_ACQUISITION_MODE_UNKNOWN");
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
        case (GIGEVISION_ACQUISITION_MODE_SINGLE_FRAME): {
            return ("GIGEVISION_ACQUISITION_MODE_SINGLE_FRAME");
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
        case (GIGEVISION_ACQUISITION_MODE_CONTINUOUS): {
            return ("GIGEVISION_ACQUISITION_MODE_CONTINUOUS");
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
#ifndef GIGEVISION_SUPPRESS_WARNING_COVERED_SWITCH_DEFAULT
        default: {
            return ("UNKNOWN");
            break;
        }
#endif
    }

#ifdef GIGEVISION_SUPPRESS_WARNING_COVERED_SWITCH_DEFAULT
    return ("UNKNOWN");
#endif

}

/*
 *
 */

GigEVision_Status_t GigEVision_Set_AcquisitionMode (           /* GigEVision Установил режим сбора данных */
    GigEVision_Context_t         *Context,        /* IN/OUT */
    GigEVision_AcquisitionMode_t  AcquisitionMode /* IN     */
) {

    Context->AcquisitionMode = AcquisitionMode;

    fprintf (stdout, "[GigEVision_Set_AcquisitionMode] AcquisitionMode: %s\n", GigEVision_ToString_AcquisitionMode (AcquisitionMode));

    switch (Context->AcquisitionMode) {

        case (GIGEVISION_ACQUISITION_MODE_SINGLE_FRAME): {

            /*
             * WRITEREG_CMD: AcquisitionMode_SingleFrame
             */

            if (
                GigEVision_SendCommand_WriteRegister (
                      Context,
                      GIGEVISION_COMMAND_ACQUISITION_MODE_SINGLE_FRAME_ADDRESS,
                      GIGEVISION_COMMAND_ACQUISITION_MODE_SINGLE_FRAME_VALUE,
                      GIGEVISION_COMMAND_ACQUISITION_MODE_SINGLE_FRAME_TAG,
                    &(Context->Counter_Requests_WriteRegister_AcquisitionMode_SingleFrame)
                ) != GIGEVISION_STATUS_OK
            ) {
                return (GIGEVISION_STATUS_FAILED);
            }

            /*
             * WRITEREG_ACK: AcquisitionMode_SingleFrame
             */

            if (
                GigEVision_ReceiveAcknowledge_WriteRegister (
                      Context,
                      GIGEVISION_COMMAND_ACQUISITION_MODE_SINGLE_FRAME_TAG,
                    &(Context->Counter_Responses_WriteRegister_AcquisitionMode_SingleFrame)
                ) != GIGEVISION_STATUS_OK
            ) {
                return (GIGEVISION_STATUS_FAILED);
            }

            break;
        }

        case (GIGEVISION_ACQUISITION_MODE_CONTINUOUS): {

            /*
             * WRITEREG_CMD: AcquisitionMode_Continuous
             */

            if (
                GigEVision_SendCommand_WriteRegister (
                      Context,
                      GIGEVISION_COMMAND_ACQUISITION_MODE_CONTINUOUS_ADDRESS,
                      GIGEVISION_COMMAND_ACQUISITION_MODE_CONTINUOUS_VALUE,
                      GIGEVISION_COMMAND_ACQUISITION_MODE_CONTINUOUS_TAG,
                    &(Context->Counter_Requests_WriteRegister_AcquisitionMode_Continous)
                ) != GIGEVISION_STATUS_OK
            ) {
                return (GIGEVISION_STATUS_FAILED);
            }

            /*
             * WRITEREG_ACK: AcquisitionMode_Continuous
             */

            if (
                GigEVision_ReceiveAcknowledge_WriteRegister (
                      Context,
                      GIGEVISION_COMMAND_ACQUISITION_MODE_CONTINUOUS_TAG,
                    &(Context->Counter_Responses_WriteRegister_AcquisitionMode_Continous)
                ) != GIGEVISION_STATUS_OK
            ) {
                return (GIGEVISION_STATUS_FAILED);
            }

            break;
        }

        default: {
            fprintf (stdout, "[GigEVision_Set_AcquisitionMode] [UNKNOWN] AcquisitionMode: %u\n", AcquisitionMode);
            return (GIGEVISION_STATUS_FAILED);
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }

    }

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

static void ParsePacket_Crevis (           /* Разбор пакета Crevis */
    GigEVision_Packet_Stream_t  *Packet_Stream,
    GigEVision_Context_t        *Context,
    void                       (*Callback_Buffer_Lock)  (void *Argument),
    void                       (*Callback_Buffer_Ready) (void *Argument),
    boolean_t                   *TrailerReceived,
    size_t                      *Size_Packet_Stream_Data,
    ssize_t                     *Size_Packet_Received,
    size_t                      *Size_Packet_Stream_Header,
    uint8_t                     *Buffer_Data,
    size_t                      *Buffer_Offset,
    size_t                      *Payload_Size,
    size_t                       PacketsNumber_Payload
) {

    switch (
        Packet_Stream->Header.TypeAndPacketID.Type
    ) {
        case (GIGEVISION_PACKET_STREAM_TYPE_LEADER): {

            Context->Counter_Data_Image_Leader++;

#if 0
            fprintf (stdout, "[ParsePacket_Crevis] [%ld] [LEADER] *Payload_Size: %ld\n", Context->Counter_Data_Image_Total, *Payload_Size);
#endif

            if (Callback_Buffer_Lock != NULL) {
                Callback_Buffer_Lock (Context);
            }

#ifdef GIGEVISION_TRACE
            Print_Counters (
                 Context,
                "LEADER"
            );
#endif

            break;
        }
        case (GIGEVISION_PACKET_STREAM_TYPE_PAYLOAD): {

            Context->Counter_Data_Image_Payload_Total++;
            Context->Counter_Data_Image_Payload_Current++;

            if (
                Context->Counter_Data_Image_Payload_Current > PacketsNumber_Payload
            ) {
                fprintf (stdout, "[ParsePacket_Crevis] Payload overflow: %ld\n", Context->Counter_Data_Image_Payload_Current);
            }
            else {

                *Size_Packet_Stream_Data = ((size_t) *Size_Packet_Received) - (*Size_Packet_Stream_Header);

                memcpy (
                    &(Buffer_Data [*Buffer_Offset]),
                    &(Packet_Stream->Data [0]),
                     *Size_Packet_Stream_Data
                );

                *Buffer_Offset = *Buffer_Offset + *Size_Packet_Stream_Data;
                *Payload_Size  = *Payload_Size  + *Size_Packet_Stream_Data;

#if 0
#if 0
                fprintf (stdout, "[ParsePacket_Crevis] [%ld] [PAYLOAD] *Buffer_Offset: %ld\n", Context->Counter_Data_Image_Total, *Buffer_Offset);
#endif
                fprintf (stdout, "[ParsePacket_Crevis] [%ld] [PAYLOAD] *Payload_Size : %ld\n", Context->Counter_Data_Image_Total, *Payload_Size);
#endif

            }

            break;
        }
        case (GIGEVISION_PACKET_STREAM_TYPE_TRAILER): {

            Context->Counter_Data_Image_Trailer++;

            Context->Payload_Size = *Payload_Size;

#if 0
            fprintf (stdout, "[ParsePacket_Crevis] [%ld] [TRAILER] Context->Payload_Size: %ld\n", Context->Counter_Data_Image_Total, Context->Payload_Size);
#endif

            if (Callback_Buffer_Ready != NULL) {
                Callback_Buffer_Ready (Context);
            }

#ifdef GIGEVISION_TRACE
            Print_Counters (
                 Context,
                "TRAILER"
            );
#endif

            *TrailerReceived = TRUE;

            if (
                Context->AcquisitionMode == GIGEVISION_ACQUISITION_MODE_CONTINUOUS
            ) {
                Context->Counter_Data_Image_Payload_Current = 0;
            }

            break;
        }
        default: {

            Context->Counter_Data_Image_Unknown++;

#ifdef GIGEVISION_TRACE
            Print_Counters (
                 Context,
                "UNKNOWN"
            );
#endif

            break;
        }
    }

}

/*
 *
 */

static void ParsePacket_Prima (           /* Разобрать пакет Prima */
    GigEVision_Packet_Stream_Prima_t  *Packet_Stream,
    GigEVision_Context_t              *Context,
    void                             (*Callback_Buffer_Lock)  (void *Argument),
    void                             (*Callback_Buffer_Ready) (void *Argument),
    boolean_t                         *TrailerReceived,
    size_t                            *Size_Packet_Stream_Data,
    ssize_t                           *Size_Packet_Received,
    size_t                            *Size_Packet_Stream_Header,
    uint8_t                           *Buffer_Data,
    size_t                             Buffer_Size
) {

#ifdef GIGEVISION_TRACE

    fprintf (stdout, "[ParsePacket_Prima]  Size_Packet_Received     : %lu\n", *Size_Packet_Received);
    fprintf (stdout, "[ParsePacket_Prima]  Size_Packet_Stream_Header: %lu\n", *Size_Packet_Stream_Header);
    fprintf (stdout, "[ParsePacket_Prima]  Size_Packet_Stream_Data  : %lu\n", *Size_Packet_Stream_Data);
    fprintf (stdout, "[ParsePacket_Prima] *Packet_Stream_Prima      : %p\n", (void *) Packet_Stream);

    fprintf (stdout, "[ParsePacket_Prima] [%ld]: ", Context->Counter_Data_Image_Total);

    Print_Bytes (
        (const uint8_t *) Packet_Stream,
        (size_t) (*Size_Packet_Received),
         TRUE
    );

    fprintf (stdout, "[ParsePacket_Prima] Packet_Stream->Header.MessageType                  : %02X\n", Packet_Stream->Header.MessageType);
    fprintf (stdout, "[ParsePacket_Prima] sizeof (Prima_Packet_Stream_Header_t)              : %ld\n", sizeof (Prima_Packet_Stream_Header_t));
    fprintf (stdout, "[ParsePacket_Prima] sizeof (Prima_Packet_Stream_Header_ChannelNumber_t): %ld\n", sizeof (Prima_Packet_Stream_Header_ChannelNumber_t));
    fprintf (stdout, "[ParsePacket_Prima] sizeof (Prima_Packet_Stream_Header_MessageType_t)  : %ld\n", sizeof (Prima_Packet_Stream_Header_MessageType_t));

#endif

    switch (
        Packet_Stream->Header.MessageType
    ) {
        case (GIGEVISION_PACKET_STREAM_HEADER_PRIMA_MESSAGE_TYPE_LEADER): {

            Context->Counter_Data_Image_Leader++;

            Context->Buffer_Offset = 0;
            Context->Payload_Size  = 0;

#if 0
            fprintf (stdout, "[ParsePacket_Prima] [%ld] [LEADER] *Buffer_Offset         : %ld\n", Context->Counter_Data_Image_Total, *Buffer_Offset);
            fprintf (stdout, "[ParsePacket_Prima] [%ld] [LEADER] *Payload_Size          : %ld\n", Context->Counter_Data_Image_Total, *Payload_Size);
            fprintf (stdout, "[ParsePacket_Prima] [%ld] [LEADER]  Context->Buffer_Offset: %ld\n", Context->Counter_Data_Image_Total,  Context->Buffer_Offset);
            fprintf (stdout, "[ParsePacket_Prima] [%ld] [LEADER]  Context->Payload_Size : %ld\n", Context->Counter_Data_Image_Total,  Context->Payload_Size);
#endif

            if (Callback_Buffer_Lock != NULL) {
                Callback_Buffer_Lock (Context);
            }

#ifdef GIGEVISION_TRACE
            Print_Counters (
                 Context,
                "LEADER"
            );
#endif

            break;
        }
        case (GIGEVISION_PACKET_STREAM_HEADER_PRIMA_MESSAGE_TYPE_PAYLOAD): {

            Context->Counter_Data_Image_Payload_Total++;
            Context->Counter_Data_Image_Payload_Current++;

            *Size_Packet_Stream_Data = ((size_t) *Size_Packet_Received) - (*Size_Packet_Stream_Header);

#if 0
            fprintf (stdout, "[ParsePacket_Prima] [%ld] [PAYLOAD] *Size_Packet_Stream_Data                      : %ld\n", Context->Counter_Data_Image_Total, *Size_Packet_Stream_Data);
            fprintf (stdout, "[ParsePacket_Prima] [%ld] [PAYLOAD] ((*Payload_Size) + (*Size_Packet_Stream_Data)): %ld\n", Context->Counter_Data_Image_Total, ((*Payload_Size) + (*Size_Packet_Stream_Data)));
            fprintf (stdout, "[ParsePacket_Prima] [%ld] [PAYLOAD] Buffer_Size                                   : %ld\n", Context->Counter_Data_Image_Total, Buffer_Size);
#endif

            if (
                (Context->Payload_Size + (*Size_Packet_Stream_Data)) > Buffer_Size
            ) {
                fprintf (stdout, "[ParsePacket_Prima] Payload overflow: %ld\n", Context->Counter_Data_Image_Payload_Current);
            }
            else {

#if 0
                *Size_Packet_Stream_Data = ((size_t) *Size_Packet_Received) - (*Size_Packet_Stream_Header);
#endif

#if 0
                memcpy (
                    &(Buffer_Data [*Buffer_Offset]),
                    &(Packet_Stream->Data [0]),
                     *Size_Packet_Stream_Data
                );
#else

#if 0
                Print_Bytes (
                    &(Packet_Stream->Data [0]),
                     *Size_Packet_Stream_Data,
                    TRUE
                );
#endif

#if 0
                memcpy (
                    &(Buffer_Data [Context->Buffer_Offset]),
                    &(Packet_Stream->Data [0]),
                     *Size_Packet_Stream_Data
                );
#else
                memcpy (
                      Buffer_Data + Context->Buffer_Offset,
                    &(Packet_Stream->Data [0]),
                     *Size_Packet_Stream_Data
                );
#endif

#endif

#if 0
                *Buffer_Offset = (*Buffer_Offset) + (*Size_Packet_Stream_Data);
                *Payload_Size  = (*Payload_Size)  + (*Size_Packet_Stream_Data);
#else
                Context->Buffer_Offset = Context->Buffer_Offset + (*Size_Packet_Stream_Data);
                Context->Payload_Size  = Context->Payload_Size  + (*Size_Packet_Stream_Data);
#endif

#if 0
                fprintf (stdout, "[ParsePacket_Prima]: ");

                Print_Bytes (
                    &(Buffer_Data [0]),
                      Context->Payload_Size,
                    TRUE
                );
#endif

#if 0
#if 0
                fprintf (stdout, "[ParsePacket_Prima] [%ld] [PAYLOAD] *Buffer_Offset: %ld\n", Context->Counter_Data_Image_Total, *Buffer_Offset);
                fprintf (stdout, "[ParsePacket_Prima] [%ld] [PAYLOAD] *Payload_Size : %ld\n", Context->Counter_Data_Image_Total, *Payload_Size);
#endif
                fprintf (stdout, "[ParsePacket_Prima] [%ld] [PAYLOAD] Context->Buffer_Offset: %ld\n", Context->Counter_Data_Image_Total, Context->Buffer_Offset);
                fprintf (stdout, "[ParsePacket_Prima] [%ld] [PAYLOAD] Context->Payload_Size : %ld\n", Context->Counter_Data_Image_Total, Context->Payload_Size);
#endif

            }

            break;
        }
        case (GIGEVISION_PACKET_STREAM_HEADER_PRIMA_MESSAGE_TYPE_TRAILER): {

            Context->Counter_Data_Image_Trailer++;

#if 0
            Context->Payload_Size = *Payload_Size;
#endif

#if 0
            fprintf (stdout, "[ParsePacket_Prima] [%ld] [TRAILER] Context->Payload_Size: %ld\n", Context->Counter_Data_Image_Total, Context->Payload_Size);
#endif

            if (Callback_Buffer_Ready != NULL) {
                Callback_Buffer_Ready (Context);
            }

#ifdef GIGEVISION_TRACE
            Print_Counters (
                 Context,
                "TRAILER"
            );
#endif

            *TrailerReceived = TRUE;

#if 0
            if (
                Context->AcquisitionMode == GIGEVISION_ACQUISITION_MODE_CONTINUOUS
            ) {
                Context->Counter_Data_Image_Payload_Current = 0;
            }
#else
                Context->Counter_Data_Image_Payload_Current = 0;
#endif

            break;
        }
        default: {

            Context->Counter_Data_Image_Unknown++;

#ifdef GIGEVISION_TRACE
            Print_Counters (
                 Context,
                "UNKNOWN"
            );
#endif

            break;
        }
    }

}

/*
 *
 */

GigEVision_Status_t GigEVision_Get_SingleFrame (           /* GigEVision Получает один кадр */
    GigEVision_Context_t  *Context,                                 /* IN/OUT */
    size_t                 Buffer_Size,                             /* IN     */
    uint8_t               *Buffer_Data,                             /* OUT    */
    size_t                *Payload_Size,                            /* OUT    */
    void                 (*Callback_Buffer_Lock)  (void *Argument), /* IN     */
    void                 (*Callback_Buffer_Ready) (void *Argument)  /* IN     */
) {

    GigEVision_Packet_Stream_t       Packet_Stream_Crevis;
    GigEVision_Packet_Stream_Prima_t Packet_Stream_Prima;

    ssize_t Size_Packet_Received = 0;

    size_t  Size_Packet_Stream_Total  = 0;
    size_t  Size_Packet_Stream_Header = 0;
    size_t  Size_Packet_Stream_Data   = 0;

    size_t Buffer_Offset = 0;

    boolean_t TrailerReceived = FALSE;

    size_t PacketsNumber_Payload = 0;

    /*
     *
     */

    memset (&Packet_Stream_Crevis, 0, sizeof (Packet_Stream_Crevis));
    memset (&Packet_Stream_Prima,  0, sizeof (Packet_Stream_Prima));

    *Payload_Size = 0;

    /*
     *
     */

    switch (Context->Device) {
        case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35):
        case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22): {

            if (
                Context->AcquisitionMode == GIGEVISION_ACQUISITION_MODE_SINGLE_FRAME
            ) {

                if (
                    GigEVision_AcquisitionEnable_On (
                        Context
                    ) != GIGEVISION_STATUS_OK
                ) {
                    return (GIGEVISION_STATUS_FAILED);
                }

            }

            break;
        }
        case (GIGEVISION_DEVICE_PRIMA_IVS_70): {

            /*
             * Nothing yet
             */

            break;
        }
        default: {
            fprintf (stdout, "[GigEVision_Get_SingleFrame] [GigEVision_AcquisitionEnable_On] [UNKNOWN] Context->Device: %d\n", Context->Device);
            return (GIGEVISION_STATUS_FAILED);
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
    }

    /*
     * Image: LEADER
     * Image: PAYLOAD
     * Image: TRAILER
     */

#if 0
    memset (Buffer_Data, 0, Buffer_Size);
#endif

    switch (Context->Device) {
        case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35):
        case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22): {
            Size_Packet_Stream_Total  = sizeof (GigEVision_Packet_Stream_t);
            Size_Packet_Stream_Header = sizeof (GigEVision_Packet_Stream_Header_t);
            break;
        }
        case (GIGEVISION_DEVICE_PRIMA_IVS_70): {
            Size_Packet_Stream_Total  = sizeof (GigEVision_Packet_Stream_Prima_t);
            Size_Packet_Stream_Header = sizeof (GigEVision_Packet_Stream_Header_Prima_t);
            break;
        }
        default: {
            fprintf (stdout, "[GigEVision_Get_SingleFrame] [Size_Packet_Stream_*] [UNKNOWN] Context->Device: %d\n", Context->Device);
            return (GIGEVISION_STATUS_FAILED);
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
    }

#ifdef GIGEVISION_TRACE
    fprintf (stdout, "[GigEVision_Get_SingleFrame] Size_Packet_Stream_Total : %lu\n", Size_Packet_Stream_Total);
    fprintf (stdout, "[GigEVision_Get_SingleFrame] Size_Packet_Stream_Header: %ld\n", Size_Packet_Stream_Header);
#endif

    while (Receive_Image == 1) {

        switch (Context->Device) {
            case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35):
            case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22): {

                memset (&Packet_Stream_Crevis, 0, Size_Packet_Stream_Total);

                Size_Packet_Received =
                    recvfrom (
                          Context->Socket_Handler_Stream,
                         (void *) &Packet_Stream_Crevis,
                          Size_Packet_Stream_Total,
                          GIGEVISION_FLAGS_RECEIVE,
                         (sockaddr_t *) &(Context->Socket_Address_Stream_Destination_Data_Result),
                        &(Context->Socket_Address_Stream_Destination_Size_Result)
                    );

                break;
            }
            case (GIGEVISION_DEVICE_PRIMA_IVS_70): {

                memset (&Packet_Stream_Prima, 0, Size_Packet_Stream_Total);

                Size_Packet_Received =
                    recvfrom (
                          Context->Socket_Handler_Stream,
                         (void *) &Packet_Stream_Prima,
                          Size_Packet_Stream_Total,
                          GIGEVISION_FLAGS_RECEIVE,
                         (sockaddr_t *) &(Context->Socket_Address_Stream_Destination_Data_Result),
                        &(Context->Socket_Address_Stream_Destination_Size_Result)
                    );

                break;
            }
            default: {
                fprintf (stdout, "[GigEVision_Get_SingleFrame] [Receive_Image] [UNKNOWN] Context->Device: %d\n", Context->Device);
                return (GIGEVISION_STATUS_FAILED);
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
                break;
#endif
            }
        }

        if (
            Size_Packet_Received < 0
        ) {

            perror ("[GigEVision_Get_SingleFrame] recvfrom()");

            if (
                errno != EAGAIN &&
                errno != EINTR
            ) {
                return (GIGEVISION_STATUS_FAILED);
            }
            else {
                fprintf (stdout, "[GigEVision_Get_SingleFrame] Size_Packet_Received: %ld\n", Size_Packet_Received);
            }

        }

#ifdef GIGEVISION_TRACE
        fprintf (stdout, "[GigEVision_Get_SingleFrame] Size_Packet_Stream_Total: %lu\n", Size_Packet_Stream_Total);
        fprintf (stdout, "[GigEVision_Get_SingleFrame] Size_Packet_Received    : %ld\n", Size_Packet_Received);
#endif

#ifdef GIGEVISION_TRACE

        fprintf (stdout, "[GigEVision_Get_SingleFrame] Context->Socket_Address_Stream_Destination_Size_Result: %u\n", Context->Socket_Address_Stream_Destination_Size_Result);
        fprintf (stdout, "[GigEVision_Get_SingleFrame] Context->Socket_Address_Stream_Destination_Data_Result: ");

        Print_Bytes (
            (const uint8_t *) &(Context->Socket_Address_Stream_Destination_Data_Result),
             Context->Socket_Address_Stream_Destination_Size_Result,
             TRUE
        );

#endif

        if (
            Size_Packet_Received == 0
        ) {
            fprintf (stdout, "[GigEVision_Get_SingleFrame] Size_Packet_Received: 0\n");
        }

        Context->Counter_Cycles_Receive_Image_Total++;

        if (Size_Packet_Received <= 0) {
            Context->Counter_Cycles_Receive_Image_Empty++;
        }

        if (
            Size_Packet_Received > 0
        ) {

            Context->Counter_Data_Image_Total++;

#ifdef GIGEVISION_TRACE
#ifdef GIGEVISION_TRACE_PRINT_RAW_PACKET_BYTES

            fprintf (stdout, "[GigEVision_Get_SingleFrame] [%ld]: ", Context->Counter_Data_Image_Total);

            switch (Context->DeviceManufacturer) {
                case (GIGEVISION_DEVICE_MANUFACTURER_CREVIS): {
                    Print_Bytes (
                        (const uint8_t *) &(Packet_Stream_Crevis),
                        (size_t) Size_Packet_Received,
                         TRUE
                    );
                    break;
                }
                case (GIGEVISION_DEVICE_MANUFACTURER_PRIMA): {
                    Print_Bytes (
                        (const uint8_t *) &(Packet_Stream_Prima),
                        (size_t) Size_Packet_Received,
                         TRUE
                    );
                    break;
                }
                default: {
                    fprintf (stdout, "[GigEVision_Get_SingleFrame] [Print_Bytes] [UNKNOWN] Context->DeviceManufacturer: %d\n", Context->DeviceManufacturer);
                    return (GIGEVISION_STATUS_FAILED);
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
                    break;
#endif
                }
            }

#else
            Print_Counters (
                 Context,
                "IMAGE"
            );
#endif
#endif

            switch (Context->Device) {
                case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35): {
                    PacketsNumber_Payload = GIGEVISION_STREAM_PACKETS_NUMBER_BAYER_RG8_2064x1544_PAYLOAD;
                    break;
                }
                case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22): {
                    PacketsNumber_Payload = GIGEVISION_STREAM_PACKETS_NUMBER_MONO8_2464x2056_PAYLOAD;
                    break;
                }
                default: {
                    fprintf (stderr, "[GigEVision_Get_SingleFrame] PacketsNumber_Payload: %ld", PacketsNumber_Payload);
                    break;
                }
            }

            switch (Context->Device) {
                case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35):
                case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22): {

                    ParsePacket_Crevis (
                        &Packet_Stream_Crevis,
                         Context,
                         Callback_Buffer_Lock,
                         Callback_Buffer_Ready,
                        &TrailerReceived,
                        &Size_Packet_Stream_Data,
                        &Size_Packet_Received,
                        &Size_Packet_Stream_Header,
                         Buffer_Data,
                        &Buffer_Offset,
                         Payload_Size,
                         PacketsNumber_Payload
                    );

                    break;
                }
                case (GIGEVISION_DEVICE_PRIMA_IVS_70): {

#ifdef GIGEVISION_TRACE

                    fprintf (stdout, "[GigEVision_Get_SingleFrame] *Packet_Stream_Prima: %p\n", (void *) &(Packet_Stream_Prima));

                    fprintf (stdout, "[GigEVision_Get_SingleFrame] [%ld]: ", Context->Counter_Data_Image_Total);

                    Print_Bytes (
                        (const uint8_t *) &(Packet_Stream_Prima),
                        (size_t) (Size_Packet_Received),
                         TRUE
                    );

#endif

#if 0
                    Context->Buffer_Data = Buffer_Data;
#endif

                    ParsePacket_Prima (
                        &Packet_Stream_Prima,
                         Context,
                         Callback_Buffer_Lock,
                         Callback_Buffer_Ready,
                        &TrailerReceived,
                        &Size_Packet_Stream_Data,
                        &Size_Packet_Received,
                        &Size_Packet_Stream_Header,
                         Buffer_Data,
                         Buffer_Size
                    );

#if 0
                    fprintf (stdout, "[GigEVision_Get_SingleFrame]: ");

                    Print_Bytes (
                        Context->Buffer_Data,
                        Context->Payload_Size,
                        TRUE
                    );
#endif

                    break;
                }
                default: {
                    fprintf (stdout, "[GigEVision_Get_SingleFrame] [ParsePacket_*] [UNKNOWN] Context->Device: %d\n", Context->Device);
                    return (GIGEVISION_STATUS_FAILED);
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
                    break;
#endif
                }
            }

        }

        if (
            Context->Counter_Cycles_Receive_Image_Empty >= Context->Socket_Poll_EmptyCycles_Maximum ||
            TrailerReceived == TRUE
        ) {

            Receive_Image = 0;

#if 0
            fprintf (stdout, "[GigEVision_Get_SingleFrame] [%ld] Payload_Size: %ld\n", Context->Counter_Data_Image_Total, *Payload_Size);
#endif

        }

    }

#ifdef GIGEVISION_TRACE
    Print_Counters (
         Context,
        "GigEVision_Get_SingleFrame"
    );
#endif

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

GigEVision_Status_t GigEVision_Stream_Start (           /* Начало трансляции GigEVision */
    GigEVision_Context_t  *Context,                                 /* IN/OUT */
    size_t                 Buffer_Size,                             /* IN     */
    uint8_t               *Buffer_Data,                             /* OUT    */
    size_t                *Payload_Size,                            /* OUT    */
    void                 (*Callback_Buffer_Lock)  (void *Argument), /* IN     */
    void                 (*Callback_Buffer_Ready) (void *Argument)  /* IN     */
) {

    switch (Context->Device) {
        case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35):
        case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22): {

            if (
                GigEVision_AcquisitionEnable_On (
                    Context
                ) != GIGEVISION_STATUS_OK
            ) {
                return (GIGEVISION_STATUS_FAILED);
            }

            break;
        }
        case (GIGEVISION_DEVICE_PRIMA_IVS_70): {

            /*
             * Nothing yet
             */

            break;
        }
        default: {
            fprintf (stdout, "[GigEVision_Stream_Start] [UNKNOWN] Context->Device: %d\n", Context->Device);
            return (GIGEVISION_STATUS_FAILED);
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
    }

    /*
     *
     */

    Context->Buffer_Data = Buffer_Data;

    /*
     *
     */

    while (Receive_Stream == 1) {

        Receive_Image = 1;

        Context->Counter_Cycles_Receive_Stream_Total++;

        if (
            GigEVision_Get_SingleFrame (
                Context,
                Buffer_Size,
                Buffer_Data,
                Payload_Size,
                Callback_Buffer_Lock,
                Callback_Buffer_Ready
            ) != GIGEVISION_STATUS_OK
        ) {
            Context->Counter_Cycles_Receive_Stream_Empty++;
        }

    }

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

GigEVision_Status_t GigEVision_Stream_Stop (           /* Остановка трансляции GigEVision */
    GigEVision_Context_t *Context /* IN */
) {

    switch (Context->Device) {
        case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35):
        case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22): {

            if (
                GigEVision_AcquisitionEnable_Off (
                    Context
                ) != GIGEVISION_STATUS_OK
            ) {
                return (GIGEVISION_STATUS_FAILED);
            }

            break;
        }
        case (GIGEVISION_DEVICE_PRIMA_IVS_70): {

            /*
             * Nothing yet
             */

            break;
        }
        default: {
            fprintf (stdout, "[GigEVision_Stream_Stop] [UNKNOWN] Context->Device: %d\n", Context->Device);
            return (GIGEVISION_STATUS_FAILED);
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
    }

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

static GigEVision_Status_t GigEVision_Close_Crevis (           /* Гиговидение Закрывает Кревис */
    GigEVision_Context_t *Context /* IN/OUT */
) {

    /*
     * WRITEREG_CMD: HeartbeatDisable_Off
     */

    if (
        GigEVision_SendCommand_WriteRegister (
              Context,
              GIGEVISION_COMMAND_HEARTBEAT_DISABLE_OFF_ADDRESS,
              GIGEVISION_COMMAND_HEARTBEAT_DISABLE_OFF_VALUE,
              GIGEVISION_COMMAND_HEARTBEAT_DISABLE_OFF_TAG,
            &(Context->Counter_Requests_WriteRegister_HeartbeatDisable_Off)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_ACK: HeartbeatDisable_Off
     */

    if (
        GigEVision_ReceiveAcknowledge_WriteRegister (
              Context,
              GIGEVISION_COMMAND_HEARTBEAT_DISABLE_OFF_TAG,
            &(Context->Counter_Responses_WriteRegister_HeartbeatDisable_Off)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_CMD: ExclusiveAccess_Off
     */

    if (
        GigEVision_SendCommand_WriteRegister (
              Context,
              GIGEVISION_COMMAND_EXCLUSIVE_ACCESS_OFF_ADDRESS,
              GIGEVISION_COMMAND_EXCLUSIVE_ACCESS_OFF_VALUE,
              GIGEVISION_COMMAND_EXCLUSIVE_ACCESS_OFF_TAG,
            &(Context->Counter_Requests_WriteRegister_ExclusiveAccess_Off)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    /*
     * WRITEREG_ACK: ExclusiveAccess_Off
     */

    if (
        GigEVision_ReceiveAcknowledge_WriteRegister (
              Context,
              GIGEVISION_COMMAND_EXCLUSIVE_ACCESS_OFF_TAG,
            &(Context->Counter_Responses_WriteRegister_ExclusiveAccess_Off)
        ) != GIGEVISION_STATUS_OK
    ) {
        return (GIGEVISION_STATUS_FAILED);
    }

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

static GigEVision_Status_t GigEVision_Close_Prima (           /* Гиговидение Закрывает Prima */
    GigEVision_Context_t *Context /* IN/OUT */
) {

    /*
     * Nothing yet
     */

#if 1
    (void)Context;
#endif

    return (GIGEVISION_STATUS_OK);

}

/*
 *
 */

GigEVision_Status_t GigEVision_Close (           /* GigEVision Закрывать */
    GigEVision_Context_t *Context /* IN */
) {

    switch (Context->Device) {
        case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35):
        case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22): {
            if (
                GigEVision_Close_Crevis (Context) != GIGEVISION_STATUS_OK
            ) {
                return (GIGEVISION_STATUS_FAILED);
            }
            break;
        }
        case (GIGEVISION_DEVICE_PRIMA_IVS_70): {
            if (
                GigEVision_Close_Prima (Context) != GIGEVISION_STATUS_OK
            ) {
                return (GIGEVISION_STATUS_FAILED);
            }
            break;
        }
        default: {
            fprintf (stdout, "[GigEVision_Close] [UNKNOWN] Context->Device: %d\n", Context->Device);
            return (GIGEVISION_STATUS_FAILED);
#ifndef GIGEVISION_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }
    }

    /*
     *
     */

    Print_Counters (Context, "GigEVision_Close");

    /*
     *
     */

    if (
        close (Context->Socket_Handler_Control) != GIGEVISION_HANDLER_IS_CLOSED_SUCCESSFULLY ||
        close (Context->Socket_Handler_Stream)  != GIGEVISION_HANDLER_IS_CLOSED_SUCCESSFULLY
    ) {
        perror ("[GigEVision_Close] close()");
        return (GIGEVISION_STATUS_FAILED);
    }
    else {

        signal (SIGINT, SIG_DFL);

        return (GIGEVISION_STATUS_OK);

    }

}
