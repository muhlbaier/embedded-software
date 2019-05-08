#ifndef GAME_BUILDING_H_
#define GAME_BUILDING_H_
#include "game.h"

/**
 * @defgroup game_building Building Module
 * @InGroup game
 * @brief This module can be used in terminal-based games to add buildings and check collisions between them an any char_object_t
 *
 * How to Use: Create an object of type building_t in the location you want it to be printed.
 * Every time a char_object_t moves in a specific direction, call the respective Game_Collision
 * function on each building and the char_object_t.
 *
 *
 *
 * @author Alex Marino
 *
 *
 * @{
 */

/** @struct building_t
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

/** @brief Detects collision during downward movement
 *
 * @param building is a building_t
 *
 * @param player is any char_object_t whose position might conflict with a building
 */
enum check_collision Game_CollisionDown(building_t building, char_object_t player);

/** @brief Detects collision during upward movement
 *
 * @param building is a building_t
 *
 * @param player is any char_object_t whose position might conflict with a building
 */
enum check_collision Game_CollisionUp(building_t building, char_object_t player);

/** @brief Detects collision during leftward movement
 *
 * @param building is a building_t
 *
 * @param player is any char_object_t whose position might conflict with a building
 */
enum check_collision Game_CollisionLeft(building_t building, char_object_t player);

/** @brief Detects collision during rightward movement
 *
 * @param building is a building_t
 *
 * @param player is any char_object_t whose position might conflict with a building
 */
enum check_collision Game_CollisionRight(building_t building, char_object_t player);
/** @} */

#endif /* GAME_BUILDING_H_ */

