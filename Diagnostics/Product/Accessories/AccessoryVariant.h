/*
  File  : AccessortVariant.h
  Title :
  Author  : D. W. Richardson
  Created : 9/19/2017
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:   Product implementation for handling wireless / wired accessorts

===============================================================================
*/

#define HAS_SKIPPER         (IPC_SPEAKER_PACKAGE_GINGERSKIPPER - IPC_SPEAKER_PACKAGE_GINGER)
#if HAS_SKIPPER != (IPC_SPEAKER_PACKAGE_PROFESSORSKIPPER - IPC_SPEAKER_PACKAGE_PROFESSOR)
#error "IPC_SPEAKER_PACKAGE_GINGER and IPC_SPEAKER_PACKAGE_PROFESSOR must be alligned"
#endif

#define HAS_MAXWELL         (IPC_SPEAKER_PACKAGE_GINGERMAXWELL - IPC_SPEAKER_PACKAGE_GINGER)
#if HAS_MAXWELL != (IPC_SPEAKER_PACKAGE_PROFESSORMAXWELL - IPC_SPEAKER_PACKAGE_PROFESSOR)
#error "IPC_SPEAKER_PACKAGE_GINGER and IPC_SPEAKER_PACKAGE_PROFESSOR must be alligned"
#endif

#define HAS_LOVEY           (IPC_SPEAKER_PACKAGE_GINGERLOVEY - IPC_SPEAKER_PACKAGE_GINGER)
#if HAS_LOVEY != (IPC_SPEAKER_PACKAGE_PROFESSORLOVEY - IPC_SPEAKER_PACKAGE_PROFESSOR)
#error "IPC_SPEAKER_PACKAGE_GINGER and IPC_SPEAKER_PACKAGE_PROFESSOR must be alligned"
#endif

#define HAS_SKIPPER_MAXWELL (IPC_SPEAKER_PACKAGE_GINGERSKIPPERMAXWELL - IPC_SPEAKER_PACKAGE_GINGER)
#if HAS_SKIPPER_MAXWELL != (IPC_SPEAKER_PACKAGE_PROFESSORSKIPPERMAXWELL - IPC_SPEAKER_PACKAGE_PROFESSOR)
#error "IPC_SPEAKER_PACKAGE_GINGER and IPC_SPEAKER_PACKAGE_PROFESSOR must be alligned"
#endif

#define HAS_LOVEY_MAXWELL   (IPC_SPEAKER_PACKAGE_GINGERLOVEYMAXWELL - IPC_SPEAKER_PACKAGE_GINGER)
#if HAS_LOVEY_MAXWELL != (IPC_SPEAKER_PACKAGE_PROFESSORLOVEYMAXWELL - IPC_SPEAKER_PACKAGE_PROFESSOR)
#error "IPC_SPEAKER_PACKAGE_GINGER and IPC_SPEAKER_PACKAGE_PROFESSOR must be alligned"
#endif


#define SPEAKER_PACKAGE_HAS_SKIPPER(x)        (x + HAS_SKIPPER)
#define SPEAKER_PACKAGE_HAS_MAXWELL(x)        (x + HAS_MAXWELL)
#define SPEAKER_PACKAGE_HAS_LOVEY(x)          (x + HAS_LOVEY)
#define SPEAKER_PACKAGE_HAS_SKIPPERMAXWELL(x) (x + HAS_SKIPPER_MAXWELL)
#define SPEAKER_PACKAGE_HAS_LOVEYMAXWELL(x)   (x + HAS_LOVEY_MAXWELL)



BOOL AccessoryVariant_SaveSpeakerPackageOnShutdown(void);
AccessoryConnectionStatus_t AccessoryVariant_GetSpeakerNeverConfig(void);
AccessoryConnectionStatus_t AccessoryVariant_GetSpeakerDefault(AccessoryType_t type);
BOOL AccessoryVariant_OverrideWiredAccessory(void);
BOOL AccessoryVariant_SupportedAccessory(uint8_t channel);

void AccessoryVariant_HandleLlwPairingClosed(void);

/* These two are kept just for the log essentially */
IpcSpeakerPackage_t AccessoryVariant_GetSpeakerVariant(void);
IpcSpeakerPackage_t AccessoryVariant_CheckSpeakerPackageActive(IpcSpeakerPackage_t speakerPackage);
SpeakerPackageVariant_t const* AccessoryVariant_GetSpeakerPackageVaraint(void);
AccessoryDescriptionShort_t AccessoryVariant_GetDescShort(AccessoryDescription_t* acc, BOOL* valid);
