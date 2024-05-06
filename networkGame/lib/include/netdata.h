#ifndef NETDATA_H
#define NETDATA_H


#define MAX_MONKEYS 2
#define MAX_BULLETS 100



enum clientCommand{READY, UP, DOWN, LEFT, RIGHT, FIRE, BLOCKED};
typedef enum clientCommand ClientCommand;

enum gameState {MENU, ONGOING };
typedef enum gameState GameState;

struct monkeyData{
   float x, y, sx, sy;
   int health;
   
};

//client data fails to track position of monkeys, not sending to server correctly
typedef struct monkeyData MonkeyData;
struct clientData{
   ClientCommand command[7];//enum READY sparas på plats 0, UP på plats 1 osv
   MonkeyData monkey;
   int playerNumber, slotsTaken[4];
   int numberOfBullets;
   int bulletStartX[MAX_MONKEYS], bulletStartY[MAX_MONKEYS], bulletDx[MAX_MONKEYS], bulletDy[MAX_MONKEYS];
};
typedef struct clientData ClientData;

struct bulletData{
   float x, y, dx, dy;
};
typedef struct bulletData BulletData;


struct serverData{
   MonkeyData monkeys[MAX_MONKEYS];
   int slotsTaken[4], numberOfBullets, numberOfPlayers;
   GameState gState;
   
};
typedef struct serverData ServerData;


#endif
