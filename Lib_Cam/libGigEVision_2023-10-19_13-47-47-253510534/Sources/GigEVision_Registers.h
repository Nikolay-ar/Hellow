/*
 * Оrganization: GosNIIAS
 * Year        : 2023
 * Author      : Vorobev Alexey Vitalievich
 * EMail       : vorobev_av@gosniias.ru
 */

#ifndef GIGEVISION_REGISTERS_H
#define GIGEVISION_REGISTERS_H

#ifdef __cplusplus
extern "C" {
#endif

#define GIGEVISION_REGISTER_EXCLUSIVE_ACCESS           0x00000A00
#define GIGEVISION_REGISTER_HEARTBEAT_DISABLE          0x00010000
#define GIGEVISION_REGISTER_BALANCE_WHITE_AUTO         0x00011000
#define GIGEVISION_REGISTER_ACQUISITION_MODE           0x0000A028
#define GIGEVISION_REGISTER_ACQUISITION_ENABLE         0x0000A000
#define GIGEVISION_REGISTER_STREAM_DESTINATION_ADDRESS 0x00000D18
#define GIGEVISION_REGISTER_STREAM_DESTINATION_PORT    0x00000D00

#ifdef __cplusplus
}
#endif

#endif
