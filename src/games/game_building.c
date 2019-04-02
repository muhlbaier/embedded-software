#include "game_building.h"


void Game_DrawBuilding(building_t building)
{
    Game_DrawRect(building.x, building.y, building.max_x, building.max_y); //draws wall
    Game_CharXY(' ', building.dx, building.dy); //draws door
}

enum check_collision Game_CollisionDown(building_t building, player_t player)
{
    if (player.x == building.dx && (building.dy-1 <= player.y && building.dy+1 >= player.y)) //door
    {
        return DOOR;
    }
    else if (player.x >= building.x && player.x <= building.max_x && player.y == building.y-1) //exterior collision
    {
        return COLLISION;
    }
    else if (player.x >= building.x+1 && player.x <= building.max_x-1 && player.y == building.max_y-1) //interior collision
    {
        return COLLISION;
    }
    else //no collision
    {
        return NO_COLLISION;
    }
}


enum check_collision Game_CollisionUp(building_t building, player_t player)
{
    if (player.x == building.dx && (building.dy-1 <= player.y && building.dy+1 >= player.y)) //door
    {
       return DOOR;
    }
    else if (building.x <= player.x && building.max_x >= player.x && player.y == building.max_y+1) //exterior collision
    {
        return COLLISION;
    }
    else if (building.x+1 <= player.x && building.max_x-1 >= player.x && player.y == building.y+1) //interior collision
    {
        return COLLISION;
    }

    else
    {
        return NO_COLLISION;
    }
}


enum check_collision Game_CollisionLeft(building_t building, player_t player)
{
    if (player.y == building.dy && (building.dx-1 <= player.x && building.dx+1 >= player.x)) //door
    {
       return DOOR;
    }
    else if (building.y <= player.y && building.max_y >= player.y && player.x == building.max_x+1) //exterior collision
    {
        return COLLISION;
    }
    else if (player.x == building.x+1 && building.y <= player.y && player.y <= building.max_y) //interior collision
    {
        return COLLISION;
    }
    else //no collision
    {
        return NO_COLLISION;
    }
}


enum check_collision Game_CollisionRight(building_t building, player_t player)
{
    if (player.y == building.dy && (building.dx-1 <= player.x && building.dx+1 >= player.x)) //door
    {
       return DOOR;
    }
    else if (building.y <= player.y && building.max_y >= player.y && player.x == building.x-1) //exterior collision
    {
        return COLLISION;
    }
    else if (player.x == building.max_x-1 && building.y <= player.y && player.y <= building.max_y) //interior collision
    {
        return COLLISION;
    }
    else
    {
        return NO_COLLISION;
    }
}
