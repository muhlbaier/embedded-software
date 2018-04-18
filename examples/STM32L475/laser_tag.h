/**
 * laser_tag.h
 *
 *  Created on: Apr 18, 2018
 *      Author: Michael
 */

#ifndef _LASER_TAG_H_
#define _LASER_TAG_H_

#include "stdint.h"

enum laser_tag_status {GAME_NOT_STARTED, GAME_ON_ALIVE, GAME_ON_RESPAWN, GAME_ON_DEAD, GAME_OVER};

typedef struct {
  enum laser_tag_status status;
  uint32_t my_score;
  uint32_t their_score;
  uint8_t health;
  uint8_t lives;
  uint8_t player_id;
  uint8_t team_id; ///< 0 is team1, 1 is team2
} laser_tag_status_t;

void LaserTag_Init(void);

void LaserTag_Fire(void);

void LaserTag_RegisterStatusCallback(void(*callback)(const laser_tag_status_t *));

#endif /* _LASER_TAG_H_ */
