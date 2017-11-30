/*
 * psocbootloader.h
 *
 *  Created on: May 31, 2017
 *      Author: mike
 */

#ifndef PSOCBOOTLOADER_H_
#define PSOCBOOTLOADER_H_

#include "project.h"

int PSoCBootload(const char **imageBuffer, uint8_t numLines);

#endif /* PSOCBOOTLOADER_H_ */
