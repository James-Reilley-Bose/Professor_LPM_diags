/****************************************************************************
 * Copyright:  (C) 2017 Bose Corporation, Framingham, MA                    *
 ****************************************************************************/

/**
 * @file Sii9437Driver.h
 * @author lb1032816
 * @date 5/26/2017
 * @brief Wrapper for the driver provided by Lattice of the Sii9437Task.
 */

#ifndef Sii9437DriverDRIVER_H
#define Sii9437DriverDRIVER_H

#include "Sii9437Task.h"

typedef void (*restart_callback)(void);

enum sii_arc_mode;
typedef enum sii_arc_mode Sii9437_ArcMode_t;

enum sii9437_pref_extraction_mode;
typedef enum sii9437_pref_extraction_mode Sii9437_PreferedExtractMode_t;

enum sii9437_extraction_mode;
typedef enum sii9437_extraction_mode Sii9437_ExtractMode_t;

struct sii_erx_latency_req;
typedef struct sii_erx_latency_req Sii9437_ErxLatency_t;

enum sii9437_mclk_mode;
typedef enum sii9437_mclk_mode Sii9437_MclkMode_t;

/**
 * @brief Instantiates an instance of the Sii9437Driver and assigns it to Sii9437Instance.
 *
 * In order to use the sii9437 driver the driver must be first initialized. Any attempt
 * to call other functions prior to the insantiation of the driver will result in an error.
 * A reset callback function must be provided upon construction in order to handle the case
 * where a call is made on the sii9437 driver that causes an assertion. When an assertion occurs,
 * the behavior of the Sii9437 is unknown until the chip is restarted.
 *
 * @param restart_callback: Function pointer of the type void(func)(void) to be invoked upon an assertion in the chip
*/
void Sii9437Driver_CreateInstance(restart_callback);

/**
 * @brief Sets the power state to either standby or full power
 *
 * The default power state of the chip upon instantiation of the driver is stanby. No ARC or eARC
 * communication can take place until the power state is changed to full power.
 *
 * @param power_state(BOOL): 0 = Full Power 1 = Standby
 * @return FALSE(0) if successfull TRUE(1) otherwise
*/
BOOL Sii9437Driver_SetPowerState(BOOL);

/**
 * @brief Gets the power state of the chip
 *
 * @param power_state(BOOL*): Pointer used to return power state. 0 = Full Power 1 = Standby
 * @return FALSE(0) if successfull TRUE(1) otherwise
*/
BOOL Sii9437Driver_GetPowerState(BOOL*);

/**
 * @brief Gets the value of the HPD line
 *
 * @param hpd_state(BOOL*): Pointer used for return of HPD state. 0 = Low 1 = High
 * @return FALSE(0) if successfull TRUE(1) otherwise
*/
BOOL Sii9437Driver_GetHotPlugDetected(BOOL*);

/**
 * @brief Sets the value of the HPD line
 *
 * @param hpd_state(BOOL*): 0 = Low 1 = High
 * @return FALSE(0) if successfull TRUE(1) otherwise
*/
BOOL Sii9437Driver_SetHotPlugDetected(BOOL);

/**
 * @brief Sets arc state of the chip
 *
 * The arc mode cannot be set until the chip is transitioned to full power.
 *
 * @param arc_mode(sii_arc_mode): arc mode (none, arc, eARC)
 * @return FALSE(0) if successfull TRUE(1) otherwise
*/
BOOL Sii9437Driver_SetArcMode(Sii9437_ArcMode_t);

/**
 * @brief Get arc state of the chip
 *
 * @param arc_mode(sii_arc_mode*): Pointer used for return value of ArcMode. 0 = Low 1 = High
 * @return FALSE(0) if successfull TRUE(1) otherwise
*/
BOOL Sii9437Driver_GetArcMode(Sii9437_ArcMode_t*);

/**
 * @brief Sets the prefered audio output of the chip (spdif or i2s)
 *
 * @param extraction_mode(sii9437_pref_extraction_mode): prefered audio output (spdif, i2s)
 * @return FALSE(0) if successfull TRUE(1) otherwise
*/
BOOL Sii9437Driver_SetPreferedExtractionMode(Sii9437_PreferedExtractMode_t extration_mode);

/**
 * @brief Set the audio edid of the chip
 *
 * Setting the edid of the chip must be done before eARC communication can occur since the
 * handshake inbetween the TX (TV) and the RX (Audio Equipment) requires a negatioation of support
 * audio formats.
 *
 * @param audio_edid(uint8_t[]): binary representation of edid, up to 128 bytes
 * @param length(uint16_t): length of array up to 128 bytes
 * @return FALSE(0) if successfull TRUE(1) otherwise
*/
BOOL Sii9437Driver_SetEDID(uint8_t audio_edid[], uint16_t length);

/**
 * @brief Update the chip based on the current channel status
 *
 * If the channel status changes, the MCLK may also need to be update. This should be
 * invoked when ever a channel status change occurs.
 *
 * @return FALSE(0) if successfull TRUE(1) otherwise
*/
BOOL Sii9437Driver_UpdateAudioChannel(void);

/**
 * @brief Get the extraction mode based on the current state of the earc connection
 *
 * Retrieves the current extraction mode of the eARC chip.
 * Possible extraction modes are SPDIF2, SPDIF8, I2S2, I2S8
 *
 * @param extraction_mode(Sii9437_ExtractMode_t): Return value for the current extraction mode
 * @return FALSE(0) if successfull TRUE(1) otherwise
*/
BOOL Sii9437Driver_GetExtractionMode(Sii9437_ExtractMode_t *extration_mode);

/**
 * @brief Queries the eARC chip for the cause of assertion and logs it.
 *
 * Upon any call to the Sii9437 driver provided by Lattice, you must check the return
 * value to see if any errors have occured. One error that *may* occur is an assertion
 * on the chips firmware. If this occurs, then the chip must be restarted.
 *
 * This function will query the Sii9437 for the cause of the assertion and then invoke
 * the restart callback function provided when instatiating an the Sii9437Driver (Sii9437Driver_CreateInstance).
*/
void Sii9437Driver_AssertionQuery(void);

/**
 * @brief Prints earc link status to log
 *
*/
void Sii9437Driver_EarcLinkStatus(void);

#endif