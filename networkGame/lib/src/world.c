#include "../include/game.h"
#include "../include/character.h"
#include "../include/world.h"

//define the walls in map
Wall walls[23] = {
    {405, 635, 267, 745},  {388, 404, 320, 740},   {324, 387, 360, 699}, {280, 323, 427, 630},  //left side circle wall 
    {635, 663, 319, 693}, {664, 700, 355, 677}, {701, 719, 425, 655}, {720, 750, 456, 545},    //circle wall

    {205, 275, 137, 220}, {220, 310, 55, 135}, {188, 259, 503, 572}, {188, 244, 621, 679}, {59, 131, 413, 475},
    {258, 332, 832, 911}, {555, 591, 903, 945}, {733, 790, 766, 828}, {280, 323, 427, 630}, {776, 841, 860, 928},
    {719, 803, 914, 1004}, {734, 800, 309, 376}, {602, 674, 191, 261}, {332, 462, 23, 103}, {566, 647, 23, 103},        //trees
     
};

//check collision with boundaries and walls
bool checkCollision(Character *character, Wall *walls, int num_walls) {
    //check that its away from the borders
    if (character->dest.x < PLAYABLE_AREA_X_MIN || character->dest.x + character->dest.w > PLAYABLE_AREA_X_MAX ||
        character->dest.y < PLAYABLE_AREA_Y_MIN || character->dest.y + character->dest.h / 2 > PLAYABLE_AREA_Y_MAX) {
        return TRUE;   // true if collision with boundary is detected
    }
    
    //check if it's away from walls
    /*
    for (int i = 0; i < num_walls; ++i) {
        if (character->dest.x + character->dest.w > walls[i].x_min && character->dest.x < walls[i].x_max &&
            character->dest.y + character->dest.h > walls[i].y_min && character->dest.y + character->dest.h / 2 < walls[i].y_max) {
            return TRUE;    // true if collision with wall...
        }
    }
    */
    
    //no collision
    return FALSE;
}
