/*
  File   :  nv.h
  Title  :
  Author :  Dylan Reid
  Created   :  6/15/6
  Language: C
  Copyright:   (C) 2006, 2009, 2010 Bose Corporation, Framingham, MA

  Description:   manage data flash

  ===============================================================================
*/
#ifndef NV_H
#define NV_H

#include "project.h"
#include "boselinkmessages.h"

#define NV_VALID_CTRL_WORD 0xB05E

typedef enum
{
    LANGUAGE_ENGLISH,
    LANUGAGE_SPANISH,
    LANGUAGE_FRENCH,
    LANGUAGE_DANISH,
    LANGUAGE_GERMAN,
    LANGUAGE_ITALIAN,
    LANGUAGE_SWEDISH,
    LANGUAGE_POLISH,
    LANGUAGE_HUNGARIAN,
    LANGUAGE_FINNISH,
    LANGUAGE_CZECH,
    LANGUAGE_GREEK,
    LANGUAGE_NORWEGIAN,
    LANGUAGE_PORTUGUESE,
    LANGUAGE_ROMANIAN,
    LANGUAGE_RUSSIAN,
    LANGUAGE_SLOVENIAN,
    LANGUAGE_TURKISH,
    LANGUAGE_SIMPLE_MANDARIN,
    LANGUAGE_THAI,
    LANGUAGE_KOREAN,
    LANGUAGE_TRADITIONAL_MANDARIN,
    LANGUAGE_HEBREW,
    LANGUAGE_ARABIC,
    LANGUAGE_JAPANESE,
    LANGUAGE_DUTCH,
    LANGUAGE_TRADITIONAL_CANTONESE,
    LANGUAGE_NUM,
    LANGUAGE_UNDEFINED = 0xff,
} LANGUAGE_SELECTION;



#define IS_LANGUAGE_SUPPORTED(support,lang) ((support >> lang) & 0x01)
#define SUPPORT_LANGUAGE(lang) (1 << lang)

#define AIM_LANGUAGE_SUPPORT   (SUPPORT_LANGUAGE(LANGUAGE_ENGLISH) | \
                                SUPPORT_LANGUAGE(LANUGAGE_SPANISH) | \
                                SUPPORT_LANGUAGE(LANGUAGE_FRENCH))

#define EURO_LANGUAGE_SUPPORT  (SUPPORT_LANGUAGE(LANGUAGE_ENGLISH)      | \
                                SUPPORT_LANGUAGE(LANUGAGE_SPANISH)      | \
                                SUPPORT_LANGUAGE(LANGUAGE_FRENCH)       | \
                                SUPPORT_LANGUAGE(LANGUAGE_DANISH)       | \
                                SUPPORT_LANGUAGE(LANGUAGE_GERMAN)       | \
                                SUPPORT_LANGUAGE(LANGUAGE_ITALIAN)      | \
                                SUPPORT_LANGUAGE(LANGUAGE_DUTCH)        | \
                                SUPPORT_LANGUAGE(LANGUAGE_SWEDISH)      | \
                                SUPPORT_LANGUAGE(LANGUAGE_POLISH)       | \
                                SUPPORT_LANGUAGE(LANGUAGE_HUNGARIAN)    | \
                                SUPPORT_LANGUAGE(LANGUAGE_FINNISH)      | \
                                SUPPORT_LANGUAGE(LANGUAGE_GREEK)        | \
                                SUPPORT_LANGUAGE(LANGUAGE_NORWEGIAN)    | \
                                SUPPORT_LANGUAGE(LANGUAGE_PORTUGUESE)   | \
                                SUPPORT_LANGUAGE(LANGUAGE_ROMANIAN)     | \
                                SUPPORT_LANGUAGE(LANGUAGE_RUSSIAN)      | \
                                SUPPORT_LANGUAGE(LANGUAGE_SLOVENIAN)    | \
                                SUPPORT_LANGUAGE(LANGUAGE_TURKISH)      | \
                                SUPPORT_LANGUAGE(LANGUAGE_CZECH))

#define AP_LANGUAGE_SUPPORT    (SUPPORT_LANGUAGE(LANGUAGE_ENGLISH) | \
                                SUPPORT_LANGUAGE(LANGUAGE_SIMPLE_MANDARIN) | \
                                SUPPORT_LANGUAGE(LANGUAGE_THAI)       | \
                                SUPPORT_LANGUAGE(LANGUAGE_KOREAN)     | \
                                SUPPORT_LANGUAGE(LANGUAGE_TRADITIONAL_MANDARIN) | \
                                SUPPORT_LANGUAGE(LANGUAGE_TRADITIONAL_CANTONESE) | \
                                SUPPORT_LANGUAGE(LANGUAGE_ARABIC)       | \
                                SUPPORT_LANGUAGE(LANGUAGE_HEBREW))

#define JAPAN_LANGUAGE_SUPPORT (SUPPORT_LANGUAGE(LANGUAGE_ENGLISH) | \
                                SUPPORT_LANGUAGE(LANGUAGE_JAPANESE))



#endif // NV_H
