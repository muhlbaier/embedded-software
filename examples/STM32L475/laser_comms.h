/*
 * LaserComms.h
 *
 *  Created on: Apr 13, 2018
 *      Author: Russell
 */

#ifndef LASERCOMMS_H_
#define LASERCOMMS_H_

#include "timing.h"
#include "uart.h"
#include "project_settings.h"
#include "stdint.h"

#define FIRE_BAUD_RATE 9600 //can be changed
#define SHOTS_PER_BURST 3
#define GUN_UART 1
#define MIN_LIMIT 100



void LaserComms_Init(uint8_t playerid, void (* hit)(uint8_t), uint32_t limit);

void LaserComms_Fire(void); //fire SHOTS_PER_BURST times

void LaserComms_HitFilter(uint8_t enemyPlayer); //get hit, but only if not just hit

#endif /* LASERCOMMS_H_ */
