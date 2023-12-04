/*
 * Оrganization: GosNIIAS
 * Year        : 2023
 * Author      : Vorobev Alexey Vitalievich
 * EMail       : vorobev_av@gosniias.ru
 */

#if 0
#define GIGEVISION_TEST_SUPPRESS_WARNING_RESERVED_ID_MACRO
#endif

#ifdef GIGEVISION_TEST_SUPPRESS_WARNING_RESERVED_ID_MACRO
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreserved-id-macro"
#endif

#if 1
#define _GNU_SOURCE
#endif

#ifdef GIGEVISION_TEST_SUPPRESS_WARNING_RESERVED_ID_MACRO
#pragma GCC diagnostic pop
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "qdbmp.h"
#include "GigEVision.h"

/*
 *
 */

#define GIGEVISION_TEST_VERSION_MAJOR "2"
#define GIGEVISION_TEST_VERSION_MINOR "0"
#define GIGEVISION_TEST_VERSION_PATCH "0-alpha-4"

/*
 *
 */

typedef struct option option_t;

/*
 *
 */

#define GIGEVISION_TEST_OPTIONS_NOT_A_FLAG      ( 0)
#define GIGEVISION_TEST_OPTIONS_NO_MORE_OPTIONS (-1)

#define GIGEVISION_SOCKET_READ_TIMEOUT_CONTROL_SECONDS      0
#define GIGEVISION_SOCKET_READ_TIMEOUT_CONTROL_MICROSECONDS 250000

#define GIGEVISION_SOCKET_READ_TIMEOUT_STREAM_SECONDS      0
#define GIGEVISION_SOCKET_READ_TIMEOUT_STREAM_MICROSECONDS 250000

#define GIGEVISION_SOCKET_BUFFER_SIZE_CONTROL 1048576
#define GIGEVISION_SOCKET_BUFFER_SIZE_STREAM  1048576

#define GIGEVISION_SOCKET_POLL_EMPTY_CYCLES_MAXIMUM 1

#define GIGEVISION_FILE_BAYER_RG8_NAME        "./Data/GigEVision_Test.rg8"
#define GIGEVISION_FILE_BMP_NAME_BAYER_RG8    "./Data/GigEVision_Test.rg8.bmp"
#define GIGEVISION_FILE_BMP_NAME_INTERPOLATED "./Data/GigEVision_Test.interpolated-c.bmp"

#define GIGEVISION_FILE_BAYER_RG8_WIDTH  2064
#define GIGEVISION_FILE_BAYER_RG8_HEIGHT 1544

#define GIGEVISION_FILE_BMP_BAYER_RG8_WIDTH  GIGEVISION_FILE_BAYER_RG8_WIDTH
#define GIGEVISION_FILE_BMP_BAYER_RG8_HEIGHT GIGEVISION_FILE_BAYER_RG8_HEIGHT

#define GIGEVISION_FILE_BMP_INTERPOLATED_WIDTH  (GIGEVISION_FILE_BAYER_RG8_WIDTH  / 2)
#define GIGEVISION_FILE_BMP_INTERPOLATED_HEIGHT (GIGEVISION_FILE_BAYER_RG8_HEIGHT / 2)

#define GIGEVISION_FILE_BMP_COLOR_DEPTH 24

#define GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_TOTAL      250
#define GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_PER_SECOND 25

#define GIGEVISION_TEST_CONTINUOUS_DEBUG_FRAME_COUNTER_CREVIS_MG_A320K_35 35
#define GIGEVISION_TEST_CONTINUOUS_DEBUG_FRAME_COUNTER_CREVIS_MG_A500M_22 22
#define GIGEVISION_TEST_CONTINUOUS_DEBUG_FRAME_COUNTER_PRIMA_IVS_70       25

#define GIGEVISION_TEST_SIZE_DEVICE 128

/*
 *
 */

#if 1
#define GIGEVISION_TEST_SINGLE_FRAME
#endif

#if 0
#define GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE
#endif

#if 1
#define GIGEVISION_TEST_CONTINUOUS
#endif

/*
 *
 */

#if 1
#define GIGEVISION_TEST_SUPPRESS_WARNING_COVERED_SWITCH_DEFAULT
#endif

#if 1
#define GIGEVISION_TEST_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
#endif

/*
 *
 */

static void Print_Usage (
    char_t **Arguments_Values
) {
    fprintf (
        stdout,
        "Usage: %s <options>\n\n"
        "Options:\n\n"
        "    -t, --device             GigEVision camera device (\""
            GIGEVISION_DEVICE_NAME_CREVIS_MG_A320K_35 "\" or \""
            GIGEVISION_DEVICE_NAME_CREVIS_MG_A500M_22 "\" or \""
            GIGEVISION_DEVICE_NAME_PRIMA_IVS_70 "\")\n"
        "    -s, --source             Local ethernet interface address\n"
        "    -d, --destination        GigEVision camera address\n",
        basename (Arguments_Values [0])
    );
}

/*
 *
 */

static boolean_t StringsAreEqual (
    const char_t *A,
    const char_t *B
) {

    int Result = 0;

    Result = strcmp (A, B);

    if (
        Result == 0
    ) {
        return (TRUE);
    }
    else {
        return (FALSE);
    }

}

/*
 *
 */

static uint8_t Image_BayerRG8_2064x1544 [GIGEVISION_SIZE_BUFFER_IMAGE_BAYER_RG8_2064x1544];
static uint8_t Image_Mono8_2464x2056    [GIGEVISION_SIZE_BUFFER_IMAGE_MONO8_2464x2056];
static uint8_t Image_H264               [GIGEVISION_SIZE_BUFFER_IMAGE_H264];

/*
 *
 */

#pragma pack (push, 1)

typedef enum TimeValueDifferenceSign_e {
    TIME_VALUE_DIFFERENCE_SIGN_POSITIVE = 0,
    TIME_VALUE_DIFFERENCE_SIGN_NEGATIVE = 1
} TimeValueDifferenceSign_t;

typedef struct FrameAcquisitionTime_s {
    timeval_t                 Time_Request;
    timeval_t                 Time_Response;
    timeval_t                 Difference_Value;
    TimeValueDifferenceSign_t Difference_Sign;
} FrameAcquisitionTime_t;

#pragma pack (pop)

/*
 *
 */

#if 1

static void Calculate_TimeValueDifference (
    timeval_t                 *Begin,        /* IN  */
    timeval_t                 *End,          /* IN  */
    timeval_t                 *Result_Value, /* OUT */
    TimeValueDifferenceSign_t *Result_Sign   /* OUT */
) {

    timeval_t X;
    timeval_t Y;

    long nsec = 0;

    X = *Begin;
    Y = *End;

    if (X.tv_usec < Y.tv_usec) {
        nsec = (Y.tv_usec - X.tv_usec) / 1000000 + 1;
        Y.tv_usec -= 1000000 * nsec;
        Y.tv_sec  += nsec;
    }

    if ((X.tv_usec - Y.tv_usec) > 1000000) {
        nsec = (X.tv_usec - Y.tv_usec) / 1000000;
        Y.tv_usec += 1000000 * nsec;
        Y.tv_sec  -= nsec;
    }

    Result_Value->tv_sec  = X.tv_sec  - Y.tv_sec;
    Result_Value->tv_usec = X.tv_usec - Y.tv_usec;

    if (X.tv_sec < Y.tv_sec) {
        *Result_Sign = TIME_VALUE_DIFFERENCE_SIGN_NEGATIVE;
    }
    else {
        *Result_Sign = TIME_VALUE_DIFFERENCE_SIGN_POSITIVE;
    }

}

#endif

/*
 *
 */

#ifdef GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE

static char Get_TimeValueDifferenceSign (
    TimeValueDifferenceSign_t TimeValueDifferenceSign /* IN */
) {

    switch (TimeValueDifferenceSign) {
        case (TIME_VALUE_DIFFERENCE_SIGN_POSITIVE): {
            return ('+');
        }
        case (TIME_VALUE_DIFFERENCE_SIGN_NEGATIVE): {
            return ('-');
        }
#ifndef GIGEVISION_TEST_SUPPRESS_WARNING_COVERED_SWITCH_DEFAULT
        default: {
            return ('u');
        }
#endif
    }

#ifdef GIGEVISION_TEST_SUPPRESS_WARNING_COVERED_SWITCH_DEFAULT
    return ('u');
#endif

}

#endif

/*
 *
 */

static void Print_Line (void) {
    fprintf (stdout, "--------------------------------------------------------------------------------\n");
}

/*
 *
 */

#if 0

static void Print_Bytes (
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
    }

    if (PrintLineEnd == TRUE) {
        fprintf (stdout, "\n");
    }

}

/*
 *
 */

static void Print_Characters (
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

static void Callback_Buffer_Lock (
    GigEVision_Context_t *Context
) {

    gettimeofday (
        &(Context->Time_Leader),
        NULL
    );

}

/*
 *
 */

static void Callback_Buffer_Ready (
    GigEVision_Context_t *Context
) {

    TimeValueDifferenceSign_t Result_Sign_Image;
    TimeValueDifferenceSign_t Result_Sign_Gap;

    timeval_t Time_Trailer_Previous;

    size_t DebugFrameCounter = 0;

    /*
     *
     */

    memset (&Time_Trailer_Previous, 0 , sizeof (Time_Trailer_Previous));

    /*
     *
     */

    Time_Trailer_Previous = Context->Time_Trailer;

    gettimeofday (
        &(Context->Time_Trailer),
        NULL
    );

    Calculate_TimeValueDifference (
        &(Context->Time_Trailer),
        &(Context->Time_Leader),
        &(Context->Time_Image),
        &(Result_Sign_Image)
    );

    if (
        Context->Counter_Data_Image_Leader == 1
    ) {
        memset (&(Context->Time_Gap), 0 , sizeof (Context->Time_Gap));
    }
    else {
        Calculate_TimeValueDifference (
            &(Context->Time_Leader),
            &(Time_Trailer_Previous),
            &(Context->Time_Gap),
            &(Result_Sign_Gap)
        );
    }

    switch (Context->Device) {
        case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35): {
            DebugFrameCounter = GIGEVISION_TEST_CONTINUOUS_DEBUG_FRAME_COUNTER_CREVIS_MG_A320K_35;
            break;
        }
        case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22): {
            DebugFrameCounter = GIGEVISION_TEST_CONTINUOUS_DEBUG_FRAME_COUNTER_CREVIS_MG_A500M_22;
            break;
        }
        case (GIGEVISION_DEVICE_PRIMA_IVS_70): {
            DebugFrameCounter = GIGEVISION_TEST_CONTINUOUS_DEBUG_FRAME_COUNTER_PRIMA_IVS_70;
            break;
        }
        default: {
            fprintf (stderr, "[Callback_Buffer_Ready] DebugFrameCounter: %ld", DebugFrameCounter);
            break;
        }
    }

    if (
         (Context->Counter_Data_Image_Leader == 1) ||
        ((Context->Counter_Data_Image_Leader % DebugFrameCounter) == 0)
    ) {

        fprintf (
            stdout,
            "Frame: %09lu Leader: %ld.%06ld Trailer: %ld.%06ld Image: %ld.%06ld Gap: %ld.%06ld Size: %ld\n",
            Context->Counter_Data_Image_Leader,
            Context->Time_Leader.tv_sec,
            Context->Time_Leader.tv_usec,
            Context->Time_Trailer.tv_sec,
            Context->Time_Trailer.tv_usec,
            Context->Time_Image.tv_sec,
            Context->Time_Image.tv_usec,
            Context->Time_Gap.tv_sec,
            Context->Time_Gap.tv_usec,
            Context->Payload_Size
        );

#if 0
        if (
            Context->DeviceManufacturer == GIGEVISION_DEVICE_MANUFACTURER_PRIMA
        ) {
            Print_BytesAndCharacters (
                &(Image_H264 [0]),
                  Context->Payload_Size,
                  TRUE
            );
        }
#endif

    }

}

/*
 *
 */

int32_t main (int32_t Arguments_Counter, char_t **Arguments_Values) {

    GigEVision_Device_t Device = GIGEVISION_DEVICE_UNKNOWN;

    char_t Option_Value_Device              [GIGEVISION_TEST_SIZE_DEVICE];
    char_t Option_Value_Address_Source      [GIGEVISION_SIZE_ADDRESS];
    char_t Option_Value_Address_Destination [GIGEVISION_SIZE_ADDRESS];

    option_t Options_List [] = {
        { "device",      required_argument, GIGEVISION_TEST_OPTIONS_NOT_A_FLAG, 't' },
        { "source",      required_argument, GIGEVISION_TEST_OPTIONS_NOT_A_FLAG, 's' },
        { "destination", required_argument, GIGEVISION_TEST_OPTIONS_NOT_A_FLAG, 'd' },
        { 0, 0, 0, 0 }
    };

    int32_t Options_Result = 0;
    int32_t Options_Index  = 0;

    boolean_t Option_IsSet_Device              = FALSE;
    boolean_t Option_IsSet_Address_Source      = FALSE;
    boolean_t Option_IsSet_Address_Destination = FALSE;

    GigEVision_Status_t  Status = GIGEVISION_STATUS_UNKNOWN;
    GigEVision_Context_t Context;

#if 0
    uint8_t Image_BayerRG8_2064x1544 [GIGEVISION_SIZE_BUFFER_IMAGE_BAYER_RG8_2064x1544];
    uint8_t Image_Mono8_2064x1544    [GIGEVISION_SIZE_BUFFER_IMAGE_MONO8_2464x2056];
    uint8_t Image_H264               [GIGEVISION_SIZE_BUFFER_IMAGE_H264];
#endif

    size_t Payload_Size = 0;

#ifdef GIGEVISION_TEST_SINGLE_FRAME

    FILE *File_BayerRG8         = NULL;
    BMP  *File_BMP_BayerRG8     = NULL;
    BMP  *File_BMP_Interpolated = NULL;

    size_t BlocksWritten          = 0;
    size_t BlocksNumber           = 1;
    int    FileClosedSuccessfully = 0;

    size_t Index_Rows    = 0;
    size_t Index_Columns = 0;

    uint8_t R  = 0x00;
    uint8_t G0 = 0x00;
    uint8_t G1 = 0x00;
    uint8_t B  = 0x00;
    uint8_t G  = 0x00;

#endif

#ifdef GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE

    FrameAcquisitionTime_t FrameAcquisitionTimes [GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_TOTAL];

    size_t Index_Frames = 0;

    timespec_t FramesDelay_Target;
    timespec_t FramesDelay_Remaining;

#endif

    /*
     *
     */

    memset (&(Option_Value_Device              [0]), 0, GIGEVISION_TEST_SIZE_DEVICE);
    memset (&(Option_Value_Address_Source      [0]), 0, GIGEVISION_SIZE_ADDRESS);
    memset (&(Option_Value_Address_Destination [0]), 0, GIGEVISION_SIZE_ADDRESS);

    memset (&Context, 0, sizeof (Context));

    memset (&(Image_BayerRG8_2064x1544 [0]), 0, GIGEVISION_SIZE_BUFFER_IMAGE_BAYER_RG8_2064x1544);
    memset (&(Image_Mono8_2464x2056    [0]), 0, GIGEVISION_SIZE_BUFFER_IMAGE_MONO8_2464x2056);
    memset (&(Image_H264               [0]), 0, GIGEVISION_SIZE_BUFFER_IMAGE_H264);

#ifdef GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE
    memset (&(FrameAcquisitionTimes [0]), 0, GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_TOTAL * sizeof (FrameAcquisitionTime_t));
#endif

    /*
     *
     */

    Print_Line ();

    fprintf (
         stdout,
        "[Version] GigEVision_Test: %s.%s.%s\n",
         GIGEVISION_TEST_VERSION_MAJOR,
         GIGEVISION_TEST_VERSION_MINOR,
         GIGEVISION_TEST_VERSION_PATCH
    );

    fprintf (
         stdout,
        "[Version] libGigEVision  : %s.%s.%s\n",
         GIGEVISION_VERSION_MAJOR,
         GIGEVISION_VERSION_MINOR,
         GIGEVISION_VERSION_PATCH
    );

    /*
     *
     */

    while (TRUE) {

        Options_Result =
            getopt_long (
                 Arguments_Counter,
                 Arguments_Values,
                "t:s:d:",
                 Options_List,
                &Options_Index
            );

        if (
            Options_Result == GIGEVISION_TEST_OPTIONS_NO_MORE_OPTIONS
        ) {
            break;
        }

        switch (Options_Result) {
            case ('t'): {
                Option_IsSet_Device = TRUE;
                memcpy (&(Option_Value_Device [0]), optarg, strlen (optarg));
                break;
            }
            case ('s'): {
                Option_IsSet_Address_Source = TRUE;
                memcpy (&(Option_Value_Address_Source [0]), optarg, strlen (optarg));
                break;
            }
            case ('d'): {
                Option_IsSet_Address_Destination = TRUE;
                memcpy (&(Option_Value_Address_Destination [0]), optarg, strlen (optarg));
                break;
            }
            default: {
                break;
            }
        }

    }

    /*
     *
     */

    Print_Line ();

    if (
        Option_IsSet_Device              == FALSE ||
        Option_IsSet_Address_Source      == FALSE ||
        Option_IsSet_Address_Destination == FALSE
    ) {
        Print_Usage (Arguments_Values);
        Print_Line ();
        return (EXIT_FAILURE);
    }

    if (
        StringsAreEqual (Option_Value_Device, GIGEVISION_DEVICE_NAME_CREVIS_MG_A320K_35) == FALSE &&
        StringsAreEqual (Option_Value_Device, GIGEVISION_DEVICE_NAME_CREVIS_MG_A500M_22) == FALSE &&
        StringsAreEqual (Option_Value_Device, GIGEVISION_DEVICE_NAME_PRIMA_IVS_70)       == FALSE
    ) {
        Print_Usage (Arguments_Values);
        Print_Line ();
        return (EXIT_FAILURE);
    }

    if (
        StringsAreEqual (Option_Value_Device, GIGEVISION_DEVICE_NAME_CREVIS_MG_A320K_35) == TRUE
    ) {
        Device = GIGEVISION_DEVICE_CREVIS_MG_A320K_35;
    }

    if (
        StringsAreEqual (Option_Value_Device, GIGEVISION_DEVICE_NAME_CREVIS_MG_A500M_22) == TRUE
    ) {
        Device = GIGEVISION_DEVICE_CREVIS_MG_A500M_22;
    }

    if (
        StringsAreEqual (Option_Value_Device, GIGEVISION_DEVICE_NAME_PRIMA_IVS_70) == TRUE
    ) {
        Device = GIGEVISION_DEVICE_PRIMA_IVS_70;
    }

    fprintf (stdout, "[Option] Device     : %s\n", Option_Value_Device);
    fprintf (stdout, "[Option] Source     : %s\n", Option_Value_Address_Source);
    fprintf (stdout, "[Option] Destination: %s\n", Option_Value_Address_Destination);

    /***************************************************************************/
    /* SingleFrame                                                             */
    /***************************************************************************/

#ifdef GIGEVISION_TEST_SINGLE_FRAME

    Print_Line ();

    switch (Device) {

        case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35): {

            Status = GigEVision_Open (
                 Device,
                       &(*Option_Value_Address_Source),
                //&(Option_Value_Address_Source      [0]),
                &(Option_Value_Address_Destination [0]),
                 GIGEVISION_SOCKET_READ_TIMEOUT_CONTROL_SECONDS,
                 GIGEVISION_SOCKET_READ_TIMEOUT_CONTROL_MICROSECONDS,
                 GIGEVISION_SOCKET_READ_TIMEOUT_STREAM_SECONDS,
                 GIGEVISION_SOCKET_READ_TIMEOUT_STREAM_MICROSECONDS,
                 GIGEVISION_SOCKET_BUFFER_SIZE_CONTROL,
                 GIGEVISION_SOCKET_BUFFER_SIZE_STREAM,
                 GIGEVISION_SOCKET_POLL_EMPTY_CYCLES_MAXIMUM,
                &Context
            );
            size_t GIGEVISION_STATUS_UNKNOWNSize = sizeof(GIGEVISION_STATUS_UNKNOWN);
            size_t DeviceSize = sizeof(Device);
            size_t StructSize = sizeof(Context) - sizeof(Context.Device) - sizeof(Context.Address_Source) - sizeof(Context.Address_Destination);

            Print_Line ();

            fprintf (stdout, "[GigEVision_Open] [SingleFrame] Status: %s\n", GigEVision_ToString_Status (Status));

            Print_Line ();

            Status = GigEVision_Set_AcquisitionMode (
                &Context,
                 GIGEVISION_ACQUISITION_MODE_SINGLE_FRAME
            );

            Status = GigEVision_Get_SingleFrame (
                &Context,
                 GIGEVISION_SIZE_BUFFER_IMAGE_BAYER_RG8_2064x1544,
                &(Image_BayerRG8_2064x1544 [0]),
                &Payload_Size,
                 NULL,
                 NULL
            );

            Print_Line ();

            fprintf (stdout, "[GigEVision_Get_SingleFrame] [SingleFrame] Status      : %s\n", GigEVision_ToString_Status (Status));
            fprintf (stdout, "[GigEVision_Get_SingleFrame] [SingleFrame] Payload_Size: %ld\n", Payload_Size);

            Print_Line ();

            Status = GigEVision_Close (
                &Context
            );

            Print_Line ();

            fprintf (stdout, "[GigEVision_Close] [SingleFrame] Status: %s\n", GigEVision_ToString_Status (Status));

            /*
             *
             */

            Print_Line ();

            /*
             * BayerRG8
             */

            File_BayerRG8 = fopen (GIGEVISION_FILE_BAYER_RG8_NAME, "wb");

            if (File_BayerRG8 == NULL) {
                perror ("[BayerRG8] fopen()");
                return (EXIT_FAILURE);
            }

            BlocksWritten =
                fwrite (
                    &(Image_BayerRG8_2064x1544 [0]),
                    GIGEVISION_SIZE_BUFFER_IMAGE_BAYER_RG8_2064x1544,
                    BlocksNumber,
                    File_BayerRG8
                );

            if (BlocksWritten != BlocksNumber) {
                perror ("[BayerRG8] fwrite()");
                return (EXIT_FAILURE);
            }

            if (fclose (File_BayerRG8) != FileClosedSuccessfully) {
                perror ("[BayerRG8] fclose()");
                return (EXIT_FAILURE);
            }

            /*
             * BMP BayerRG8
             */

            File_BMP_BayerRG8 = BMP_Create (
                GIGEVISION_FILE_BMP_BAYER_RG8_WIDTH,
                GIGEVISION_FILE_BMP_BAYER_RG8_HEIGHT,
                GIGEVISION_FILE_BMP_COLOR_DEPTH
            );

            for (
                Index_Rows = 0;
                Index_Rows < GIGEVISION_FILE_BMP_BAYER_RG8_HEIGHT;
                Index_Rows++
            ) {

                for (
                    Index_Columns = 0;
                    Index_Columns < GIGEVISION_FILE_BMP_BAYER_RG8_WIDTH;
                    Index_Columns++
                ) {

                    /*
                     * RG
                     * GB
                     */

                    if ((Index_Rows % 2) == 0) { /* Rows start from 0 -> Odd row */
                        if ((Index_Columns % 2) == 0) { /* Columns start from 0 -> Odd column */
                            R = Image_BayerRG8_2064x1544 [Index_Rows * GIGEVISION_FILE_BAYER_RG8_WIDTH + Index_Columns];
                            G = 0x00;
                            B = 0x00;
                        }
                        else {                          /* Columns start from 0 -> Even column */
                            R = 0x00;
                            G = Image_BayerRG8_2064x1544 [Index_Rows * GIGEVISION_FILE_BAYER_RG8_WIDTH + Index_Columns]; /* G0 */
                            B = 0x00;
                        }
                    }
                    else {                       /* Rows start from 0 -> Even row */
                        if ((Index_Columns % 2) == 0) { /* Columns start from 0 -> Odd column */
                            R = 0x00;
                            G = Image_BayerRG8_2064x1544 [Index_Rows * GIGEVISION_FILE_BAYER_RG8_WIDTH + Index_Columns]; /* G1 */
                            B = 0x00;
                        }
                        else {                          /* Columns start from 0 -> Even column */
                            R = 0x00;
                            G = 0x00;
                            B = Image_BayerRG8_2064x1544 [Index_Rows * GIGEVISION_FILE_BAYER_RG8_WIDTH + Index_Columns];
                        }
                    }

                    BMP_SetPixelRGB (
                        File_BMP_BayerRG8,
                        Index_Columns,
                        Index_Rows,
                        R,
                        G,
                        B
                    );

                }

            }

            BMP_WriteFile (
                File_BMP_BayerRG8,
                GIGEVISION_FILE_BMP_NAME_BAYER_RG8
            );

            BMP_Free (File_BMP_BayerRG8);

            /*
             * BMP Interpolated
             */

            File_BMP_Interpolated = BMP_Create (
                GIGEVISION_FILE_BMP_INTERPOLATED_WIDTH,
                GIGEVISION_FILE_BMP_INTERPOLATED_HEIGHT,
                GIGEVISION_FILE_BMP_COLOR_DEPTH
            );

            for (
                Index_Rows = 0;
                Index_Rows < GIGEVISION_FILE_BMP_INTERPOLATED_HEIGHT;
                Index_Rows++
            ) {

                for (
                    Index_Columns = 0;
                    Index_Columns < GIGEVISION_FILE_BMP_INTERPOLATED_WIDTH;
                    Index_Columns++
                ) {

                    /*
                     * RG
                     * GB
                     */

                    R  = Image_BayerRG8_2064x1544 [(Index_Rows * 2 + 0) * GIGEVISION_FILE_BAYER_RG8_WIDTH + Index_Columns * 2 + 0];
                    G0 = Image_BayerRG8_2064x1544 [(Index_Rows * 2 + 0) * GIGEVISION_FILE_BAYER_RG8_WIDTH + Index_Columns * 2 + 1];
                    G1 = Image_BayerRG8_2064x1544 [(Index_Rows * 2 + 1) * GIGEVISION_FILE_BAYER_RG8_WIDTH + Index_Columns * 2 + 0];
                    B  = Image_BayerRG8_2064x1544 [(Index_Rows * 2 + 1) * GIGEVISION_FILE_BAYER_RG8_WIDTH + Index_Columns * 2 + 1];

                   /*
                    * Average green
                    */

                    G = (G0 + G1) / 2;

                    BMP_SetPixelRGB (
                        File_BMP_Interpolated,
                        Index_Columns,
                        Index_Rows,
                        R,
                        G,
                        B
                    );

                }

            }

            BMP_WriteFile (
                File_BMP_Interpolated,
                GIGEVISION_FILE_BMP_NAME_INTERPOLATED
            );

            BMP_Free (File_BMP_Interpolated);

            break;
        }

        case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22): {

            fprintf (stdout, "[SingleFrame] Test not implemented yet for device: %s\n", GIGEVISION_DEVICE_NAME_CREVIS_MG_A500M_22);

            break;

        }

        case (GIGEVISION_DEVICE_PRIMA_IVS_70): {

            fprintf (stdout, "[SingleFrame] Test not applicable for device: %s\n", GIGEVISION_DEVICE_NAME_PRIMA_IVS_70);

            break;
        }

        default: {
            fprintf (stdout, "[SingleFrame] [UNKNOWN] Device: %d\n", Device);
            return (GIGEVISION_STATUS_FAILED);
#ifndef GIGEVISION_TEST_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }

    }

#endif

    /***************************************************************************/
    /* SingleFrame Sequence                                                    */
    /***************************************************************************/

#ifdef GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE

    switch (DeviceManufacturer) {

        case (GIGEVISION_DEVICE_MANUFACTURER_CREVIS): {

            memset (&Context, 0, sizeof (Context));

            Status = GigEVision_Open (
                 DeviceManufacturer,
                &(Option_Value_Address_Source      [0]),
                &(Option_Value_Address_Destination [0]),
                 GIGEVISION_SOCKET_READ_TIMEOUT_CONTROL_SECONDS,
                 GIGEVISION_SOCKET_READ_TIMEOUT_CONTROL_MICROSECONDS,
                 GIGEVISION_SOCKET_READ_TIMEOUT_STREAM_SECONDS,
                 GIGEVISION_SOCKET_READ_TIMEOUT_STREAM_MICROSECONDS,
                 GIGEVISION_SOCKET_BUFFER_SIZE_CONTROL,
                 GIGEVISION_SOCKET_BUFFER_SIZE_STREAM,
                 GIGEVISION_SOCKET_POLL_EMPTY_CYCLES_MAXIMUM,
                &Context
            );

            Print_Line ();

            fprintf (stdout, "[GigEVision_Open] [%d SingleFrames at %d FPS] Status: %s\n", GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_TOTAL, GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_PER_SECOND, GigEVision_ToString_Status (Status));

            memset (&FramesDelay_Target, 0, sizeof (FramesDelay_Target));
            memset (&FramesDelay_Remaining, 0, sizeof (FramesDelay_Remaining));

            FramesDelay_Target.tv_sec  = 0;
            FramesDelay_Target.tv_nsec = (long) ((1.0 / GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_PER_SECOND) / 1.0e-9); /* 1.0e-9 -> nanosecond */

#if 0
            FramesDelay_Target.tv_nsec = FramesDelay_Target.tv_nsec / 2;
#endif

            fprintf (stdout, "[GigEVision_Open] [%d SingleFrames at %d FPS] FramesDelay_Target.tv_sec : %lu\n", GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_TOTAL, GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_PER_SECOND, FramesDelay_Target.tv_sec);
            fprintf (stdout, "[GigEVision_Open] [%d SingleFrames at %d FPS] FramesDelay_Target.tv_nsec: %lu\n", GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_TOTAL, GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_PER_SECOND, FramesDelay_Target.tv_nsec);

            Status = GigEVision_Set_AcquisitionMode (
                &Context,
                 GIGEVISION_ACQUISITION_MODE_SINGLE_FRAME
            );

            for (
                Index_Frames = 0;
                Index_Frames < GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_TOTAL;
                Index_Frames++
            ) {

                gettimeofday (
                    &(FrameAcquisitionTimes [Index_Frames].Time_Request),
                    NULL
                );

                Status = GigEVision_Get_SingleFrame (
                    &Context,
                     GIGEVISION_SIZE_BUFFER_IMAGE_BAYER_RG8_2064x1544,
                    &(Image_BayerRG8_2064x1544 [0]),
                    &Payload_Size,
                     NULL,
                     NULL
                );

                gettimeofday (
                    &(FrameAcquisitionTimes [Index_Frames].Time_Response),
                    NULL
                );

#if 1
                fprintf (stdout, "[GigEVision_Get_SingleFrame] [SingleFrame %03lu of %d at %d FPS] Status      : %s\n", Index_Frames + 1, GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_TOTAL, GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_PER_SECOND, GigEVision_ToString_Status (Status));
                fprintf (stdout, "[GigEVision_Get_SingleFrame] [SingleFrame %03lu of %d at %d FPS] Payload_Size: %ld\n", Index_Frames + 1, GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_TOTAL, GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_PER_SECOND, Payload_Size);
#endif

#if 0
                sleep (1);
#else
                nanosleep (&FramesDelay_Target, &FramesDelay_Remaining);
#endif

            }

#if 1
            Print_Line ();
#endif

            Status = GigEVision_Close (
                &Context
            );

            Print_Line ();

            fprintf (stdout, "[GigEVision_Close] [%d SingleFrames at %d FPS] Status: %s\n", GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_TOTAL, GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_PER_SECOND, GigEVision_ToString_Status (Status));

            Print_Line ();

            for (
                Index_Frames = 0;
                Index_Frames < GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_TOTAL;
                Index_Frames++
            ) {

                Calculate_TimeValueDifference (
                    &(FrameAcquisitionTimes [Index_Frames].Time_Response),
                    &(FrameAcquisitionTimes [Index_Frames].Time_Request),
                    &(FrameAcquisitionTimes [Index_Frames].Difference_Value),
                    &(FrameAcquisitionTimes [Index_Frames].Difference_Sign)
                );

                fprintf (
                    stdout,
                    "[GigEVision_Get_SingleFrame] [SingleFrame %03lu of %d at %d FPS] [FrameAcquisitionTime] %c %ld.%06ld\n",
                    Index_Frames + 1,
                    GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_TOTAL,
                    GIGEVISION_TEST_SINGLE_FRAME_SEQUENCE_FRAMES_PER_SECOND,
                    Get_TimeValueDifferenceSign (FrameAcquisitionTimes [Index_Frames].Difference_Sign),
                    FrameAcquisitionTimes [Index_Frames].Difference_Value.tv_sec,
                    FrameAcquisitionTimes [Index_Frames].Difference_Value.tv_usec
                );

            }

            Print_Line ();

            break;
        }

        case (GIGEVISION_DEVICE_MANUFACTURER_PRIMA): {

            fprintf (stdout, "[SingleFrame Sequence] Test not applicable for device manufacturer: %s\n", GIGEVISION_DEVICE_MANUFACTURER_NAME_PRIMA);

            break;
        }

        default: {
            fprintf (stdout, "[SingleFrame Sequence] [UNKNOWN] DeviceManufacturer: %d\n", DeviceManufacturer);
            return (GIGEVISION_STATUS_FAILED);
#ifndef GIGEVISION_TEST_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }

    }

#endif

    /***************************************************************************/
    /* Continuous                                                              */
    /***************************************************************************/

#ifdef GIGEVISION_TEST_CONTINUOUS

    memset (&Context, 0, sizeof (Context));

    Status = GigEVision_Open (
         Device,
        &(Option_Value_Address_Source      [0]),
        &(Option_Value_Address_Destination [0]),
         GIGEVISION_SOCKET_READ_TIMEOUT_CONTROL_SECONDS,
         GIGEVISION_SOCKET_READ_TIMEOUT_CONTROL_MICROSECONDS,
         GIGEVISION_SOCKET_READ_TIMEOUT_STREAM_SECONDS,
         GIGEVISION_SOCKET_READ_TIMEOUT_STREAM_MICROSECONDS,
         GIGEVISION_SOCKET_BUFFER_SIZE_CONTROL,
         GIGEVISION_SOCKET_BUFFER_SIZE_STREAM,
         GIGEVISION_SOCKET_POLL_EMPTY_CYCLES_MAXIMUM,
        &Context
    );

    Print_Line ();

    fprintf (stdout, "[GigEVision_Open] [Continuous] Status: %s\n", GigEVision_ToString_Status (Status));

    switch (Context.Device) {

        case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35):
        case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22): {

            Status = GigEVision_Set_AcquisitionMode (
                &Context,
                 GIGEVISION_ACQUISITION_MODE_CONTINUOUS
            );

            fprintf (stdout, "[GigEVision_Set_AcquisitionMode] [Continuous] Status: %s\n", GigEVision_ToString_Status (Status));

            break;
        }

        case (GIGEVISION_DEVICE_PRIMA_IVS_70): {

            /*
             * Nothing yet
             */

            break;
        }

        default: {
            fprintf (stdout, "[Continuous -> GigEVision_Set_AcquisitionMode] [UNKNOWN] Context.Device: %d\n", Context.Device);
            return (GIGEVISION_STATUS_FAILED);
#ifndef GIGEVISION_TEST_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }

    }

    switch (Context.Device) {

        case (GIGEVISION_DEVICE_CREVIS_MG_A320K_35): {

            Status = GigEVision_Stream_Start (
                &Context,
                 GIGEVISION_SIZE_BUFFER_IMAGE_BAYER_RG8_2064x1544,
                &(Image_BayerRG8_2064x1544 [0]),
                &Payload_Size,
                 (void (*) (void *)) Callback_Buffer_Lock,
                 (void (*) (void *)) Callback_Buffer_Ready
            );

            break;
        }

        case (GIGEVISION_DEVICE_CREVIS_MG_A500M_22): {

            Status = GigEVision_Stream_Start (
                &Context,
                 GIGEVISION_SIZE_BUFFER_IMAGE_MONO8_2464x2056,
                &(Image_Mono8_2464x2056 [0]),
                &Payload_Size,
                 (void (*) (void *)) Callback_Buffer_Lock,
                 (void (*) (void *)) Callback_Buffer_Ready
            );

            break;
        }

        case (GIGEVISION_DEVICE_PRIMA_IVS_70): {

            Status = GigEVision_Stream_Start (
                &Context,
                 GIGEVISION_SIZE_BUFFER_IMAGE_H264,
                &(Image_H264 [0]),
                &Payload_Size,
                 (void (*) (void *)) Callback_Buffer_Lock,
                 (void (*) (void *)) Callback_Buffer_Ready
            );

            break;
        }

        default: {
            fprintf (stdout, "[Continuous -> GigEVision_Stream_Start] [UNKNOWN] Context.Device: %d\n", Context.Device);
            return (GIGEVISION_STATUS_FAILED);
#ifndef GIGEVISION_TEST_SUPPRESS_WARNING_UNREACHABLE_CODE_BREAK
            break;
#endif
        }

    }

    fprintf (stdout, "[GigEVision_Stream_Start] [Continuous] Status: %s\n", GigEVision_ToString_Status (Status));

    Status = GigEVision_Stream_Stop (
        &Context
    );

    fprintf (stdout, "[GigEVision_Stream_Stop] [Continuous] Status: %s\n", GigEVision_ToString_Status (Status));

    Print_Line ();

    Status = GigEVision_Close (
        &Context
    );

    Print_Line ();

    fprintf (stdout, "[GigEVision_Close] [Continuous] Status: %s\n", GigEVision_ToString_Status (Status));

#endif

    /*
     *
     */

    if (Status == GIGEVISION_STATUS_OK) {
        return (EXIT_SUCCESS);
    }
    else {
        return (EXIT_FAILURE);
    }

}
