/**
 * @file etapLBCS.h
 *
 * @author Shelby Apps Team
 *
 * @attention
 *     BOSE CORPORATION.
 *     COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
 *     This program may not be reproduced, in whole or in part in any
 *     form or any means whatsoever without the written permission of:
 *         BOSE CORPORATION
 *         The Mountain,
 *         Framingham, MA 01701-9168
 */

#pragma once

#include "etap.h"

/**
 * @DOCETAP
 * COMMAND:   lbcs
 *
 * ALIAS:     lb
 *
 * DESCRIP:   controls the lightbar / capsense component
 *
 * PARAM:     sub-command, params
 *            
 *
 * REPLY:     depends on sub-command
 *
 * EXAMPLE:   lb l1,1,f000 -- set led 1 intentisy to 0x00f0
 *            lb lc -- clear all leds
 *
 * @DOCETAPEND
 */

#define LBCS_HELP_TEXT "Control the lightbar / capsense component\n\r\t \
Usage: lb cmd,params \n\r"

TAPCommand(TAP_LBCS);
