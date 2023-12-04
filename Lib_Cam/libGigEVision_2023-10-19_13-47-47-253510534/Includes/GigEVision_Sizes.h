/*
 * Оrganization: GosNIIAS
 * Year        : 2023
 * Author      : Vorobev Alexey Vitalievich
 * EMail       : vorobev_av@gosniias.ru
 */

#ifndef GIGEVISION_SIZES_H
#define GIGEVISION_SIZES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "GigEVision_Utilities.h"

/*
 *
 */

#define GIGEVISION_SIZE_ADDRESS GIGEVISION_SIZE_TO_C_STRING(15)

#define GIGEVISION_SIZE_COUNTER_REQUESTS_TOTAL_LOW_MAXIMUM 65535

#define GIGEVISION_SIZE_BUFFER_IMAGE_BAYER_RG8_2064x1544 3186816
#define GIGEVISION_SIZE_BUFFER_IMAGE_MONO8_2464x2056     5065984
#define GIGEVISION_SIZE_BUFFER_IMAGE_H264                1048576

#ifdef __cplusplus
}
#endif

#endif
