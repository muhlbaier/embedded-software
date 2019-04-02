/*
 * game_building.h
 *
 *  Created on: Apr 2, 2019
 *      Author: Alex Marino
 */

#ifndef GAME_BUILDING_H_
#define GAME_BUILDING_H_


/**@struct building_t
 *
 * @brief Building info structure
 *
 * Holds building origin, dimensions, and door location
 *
 * @warning Y values start from the top. Make max_y the larger value but recall that it will be the "bottom"
 */
typedef struct building_t{
    char x; //distance from left of screen to close wall
    char y; //distance from top of screen to close wall
    char max_x; //distance from left of screen to far wall
    char max_y; //distance from top of screen to far wall
    char dx; //x coordinate of door
    char dy; //y coordinate of door
}building_t;


/** @brief Enum to store collision type for use with building_t
 *
 * Use these values to determine if a collision occurs or if the object is in a doorway
 * If DOOR is detected, user should produce 2 movements so that the object moves through the door,
 * as the object could move upwards or downwards through the door if it stays in the doorway
 */
typedef enum check_collision
{
    NO_COLLISION = 0,
    COLLISION = 1,
    DOOR = 2
}check_collision;

/** @brief Function to draw buildings with doors
 *
 * @param building is a building_t that stores building parameters
 */
void Game_DrawBuilding(building_t building);

/** @brief Dectects collision during downward movement
 *
 * @param building is a building_t
 *
 * @param player is any char_object_t whose position might conflict with a building
 */
enum check_collision Game_CollisionDown(building_t building, player_t player);

/** @brief Dectects collision during upward movement
 *
 * @param building is a building_t
 *
 * @param player is any player_t whose position might conflict with a building
 */
enum check_collision Game_CollisionUp(building_t building, player_t player);

/** @brief Dectects collision during leftward movement
 *
 * @param building is a building_t
 *
 * @param player is any player_t whose position might conflict with a building
 */
enum check_collision Game_CollisionLeft(building_t building, player_t player);

/** @brief Dectects collision during rightward movement
 *
 * @param building is a building_t
 *
 * @param player is any player_t whose position might conflict with a building
 */
enum check_collision Game_CollisionRight(building_t building, player_t player);



#endif /* GAME_BUILDING_H_ */
