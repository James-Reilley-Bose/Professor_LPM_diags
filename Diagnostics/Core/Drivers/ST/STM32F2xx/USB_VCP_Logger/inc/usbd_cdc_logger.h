/**
	File	:	usbd_cdc_logger.h
	Title	:	
	Author	:	dx1007255
	Created	:	12/29/2015
	Language:	C
	Copyright:	(C) 2015 Bose Corporation, Framingham, MA

	Description:   Interface header for USB CDC logger
===============================================================================
*/

#ifndef __USBD_CDC_LOGGER_H
#define __USBD_CDC_LOGGER_H

void USB_LoggerTaskInit(void);
void USB_LoggerEnterStandby(BOOL enterStby);


void USB_LoggerPrintString(const char *Buf);

#endif /* __USBD_CDC_LOGGER_H */
