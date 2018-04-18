#ifndef LASERCOMMS_H_
#define LASERCOMMS_H_

#include "timing.h"
#include "uart.h"
#include "project_settings.h"
#include "stdint.h"

#define FIRE_BAUD_RATE 9600 //can be changed
#define SHOTS_PER_BURST 3

void LaserComms_Init(uint8_t playerid, void (* hit)(uint8_t));

void LaserComms_Fire(void); //fire SHOTS_PER_BURST times

void LaserComms_HitFilter(uint32_t limit); //get hit, but only if not just hit

#endif /* LASERCOMMS_H_ */
