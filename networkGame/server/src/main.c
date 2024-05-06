#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include "../../lib/include/netdata.h"
#include "../../lib/include/character.h"
#include "../../lib/include/bullet.h"
#include "../../lib/include/text.h"
#include "../../lib/include/game.h"

#define NR_OF_MENUTEXTURES 2


enum menuState{MAIN, SETTINGS, CONFIGURE, INGAME};
typedef enum menuState MenuState;
struct menuTextures{
    SDL_Texture *SDLmTex[NR_OF_MENUTEXTURES];
    char MenuTextureFiles[NR_OF_MENUTEXTURES][60];
}; typedef struct menuTextures MenuTextures;

struct game{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    Character *pPlayers[MAX_PLAYERS];  
    SDL_Texture *background;
    MenuTextures *menuTextures;
    SDL_Rect background_rect;
    SDL_Rect menu_rect;
    GameState state;
    MenuState menuState;
    Bullet *bullets[1000];
    int num_bullets, num_players, slotsTaken[4]; // track the number of players in the game
    TTF_Font *font;
    Text *pWaitingText, *pJoinedText;
    ServerData sData;
    
    UDPsocket pSocket;
    IPaddress serverAddress[MAX_PLAYERS];
    UDPpacket *pPacket;
    
}; typedef struct game Game;

int initiate(Game *pGame);
void run(Game *pGame);
void close(Game *pGame);
void add(IPaddress address, IPaddress clients[],int *pNrOfClients);
void sendGameData(Game *pGame);
void executeCommand(Game *pGame,ClientData cData);
void acceptClients(Game *pGame);

 

int main(int argv, char** args){
    Game g={0};
    if(!initiate(&g)) return 1;
    run(&g);
    close(&g);

    return 0;
}

int initiate(Game *pGame){
    srand(time(NULL));
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)!=0){
        printf("Error: %s\n",SDL_GetError());
        return 0;
    }
    if (SDLNet_Init())
    {
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        SDL_Quit();
        return 0;
    }

    if (TTF_Init() == -1) {
    printf("TTF_Init Error: %s\n", TTF_GetError());
    return 1;
    }

    pGame->font = TTF_OpenFont("../lib/resources/arial.ttf", 60);
    if (!pGame->font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        return 0;
    }

    pGame->pWindow = SDL_CreateWindow("Monkey Server",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,0);
    if(!pGame->pWindow){
        printf("Error: %s\n",SDL_GetError());
        close(pGame);
        return 0;
    }

    pGame->pRenderer = SDL_CreateRenderer(pGame->pWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!pGame->pRenderer){
        printf("Error: %s\n",SDL_GetError());
        close(pGame);
        return 0;    
    }
    pGame->background = IMG_LoadTexture(pGame->pRenderer, "../lib/resources/monkeyMap.png");
    if (!pGame->background) {
        printf("Error loading background image: %s\n", IMG_GetError());
        return FALSE;
    }
    


    if (!(pGame->pSocket = SDLNet_UDP_Open(2000)))
	{
		printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		close(pGame);
        return 0;
	}
	if (!(pGame->pPacket = SDLNet_AllocPacket(10000)))
	{
		printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		close(pGame);
        return 0;
	}

    for(int i=0;i<MAX_PLAYERS;i++) pGame->pPlayers[i] = createCharacter(pGame->pRenderer);
    
    pGame->num_players = MAX_MONKEYS;
    
    for(int i=0;i<MAX_MONKEYS;i++){
        if(!pGame->pPlayers[i]){
            printf("Error: %s\n",SDL_GetError());
            close(pGame);
            return 0;
        }
    }
    pGame->pWaitingText = createText(pGame->pRenderer, 255, 255, 255, pGame->font, "Waiting for players", 400, 400);
    pGame->pJoinedText = createText(pGame->pRenderer, 255, 255, 255, pGame->font, "All players joined", 400, 400);
   
    pGame->state = MENU;
    pGame->num_players = 0;
    return 1;
}

void acceptClients(Game *pGame){
    while(pGame->num_players < MAX_PLAYERS) {
        // Receive UDP packets
        if(SDLNet_UDP_Recv(pGame->pSocket,pGame->pPacket)==1) {
            // Add the player
            add(pGame->pPacket->address, pGame->serverAddress, &(pGame->num_players));
            sendGameData(pGame);
        } else {
            // No incoming packets, break out of the loop
            break;
        }
    }
}
void renderCharacters(Game *pGame){
    for(int i = 0; i < pGame->num_players; i++){
        Character *character = pGame->pPlayers[i];
        SDL_Rect characterDest = {
            pGame->pPlayers[i]->dest.x ,//- pGame->viewport.x,
            pGame->pPlayers[i]->dest.y ,//- pGame->viewport.y,
            pGame->pPlayers[i]->dest.w,
            pGame->pPlayers[i]->dest.h
        };
        SDL_RenderCopyEx(pGame->pRenderer, character->tex, &character->source, &characterDest, 0, NULL, SDL_FLIP_NONE);
    }
}


void run(Game *pGame){
    int close_requested = 0;
    pGame->num_bullets = 0; 
    SDL_Event event;
    ClientData cData = {0};
    while(!close_requested){
       

        switch (pGame->state)
        {
            case ONGOING:
                //printf("Game is ongoing\n");
                sendGameData(pGame);
                SDL_RenderCopy(pGame->pRenderer, pGame->background, NULL, NULL);
                renderCharacters(pGame);
                if(SDLNet_UDP_Recv(pGame->pSocket,pGame->pPacket)==1){
                    
                    if(cData.command[5]==FIRE) printf("fire=on");
                    memcpy(&cData, pGame->pPacket->data, sizeof(ClientData));
                    printf("in ongoing:bulletStartX: %d, bulletStartY: %d, bulletDx: %d, bulletDy: %d\n", cData.bulletStartX[cData.playerNumber], cData.bulletStartY[cData.playerNumber], cData.bulletDx[cData.playerNumber], cData.bulletDy[cData.playerNumber]);
                    executeCommand(pGame,cData);
                    sendGameData(pGame);
                    memset(&cData, 0, sizeof(cData));
                    
                    
                }
                for (int i = 0; i < pGame->num_bullets; i++){
                    if(pGame->bullets[i] == NULL) continue;
                    drawBullet(pGame->bullets[i], pGame->pRenderer);
                    moveBullet(pGame->bullets[i]);
                        
                }
                
                if(SDL_PollEvent(&event)) {
                    if(event.type==SDL_QUIT) {
                        close_requested = 1;
                        break; // Break out of the while loop when the window is closed
                    }
                }
            /*
                for(int i=0;i<MAX_PLAYERS;i++)
                    pGame->pPlayers[cData.playerNumber]->health = cData.monkey.health;
                    pGame->pPlayers[cData.playerNumber]->dest.x = cData.monkey.sx;
                    pGame->pPlayers[cData.playerNumber]->dest.y = cData.monkey.sy;
                
                for(int i=0;i<MAX_PLAYERS;i++)
                    renderCharacter(pGame->pPlayers[i], pGame->pRenderer);

                //go through and check if all players are dead
                int allPlayersDead = 1;
                for(int i = 0; i < MAX_PLAYERS; i++) {
                    if(pGame->pPlayers[i]->health > 0) {
                        allPlayersDead = 0;
                        break;
                    }
                }

                //if all players dead, got to menu and reset health
                if(allPlayersDead) {
                    pGame->state = MENU;
                    pGame->num_players = 0;
                    for(int i = 0; i < MAX_PLAYERS; i++) {
                        pGame->pPlayers[i]->health = MAX_HEALTH; // Reset player health
                    }
                }
                */
                SDL_RenderPresent(pGame->pRenderer);
                
                break;
            /*case :
                
                sendGameData(pGame);
                if(pGame->num_players==MAX_PLAYERS) pGame->num_players = 0;*/
            case MENU:
                acceptClients(pGame);
                drawText(pGame->pWaitingText);
                SDL_RenderPresent(pGame->pRenderer);
                sendGameData(pGame);
                
                //printf("Waiting for players\n");
                if(SDL_PollEvent(&event) && event.type==SDL_QUIT) {
                    close_requested = 1;
                    break; // Break out of the while loop when the window is closed
                }
                if(SDLNet_UDP_Recv(pGame->pSocket,pGame->pPacket)==1){
                    add(pGame->pPacket->address,pGame->serverAddress,&(pGame->num_players));
                    if(pGame->num_players==MAX_MONKEYS){
                        pGame->state = ONGOING;
                        destroyText(pGame->pWaitingText);
                    }
                     
                }
                break;
        }
        //SDL_Delay(1000/60-15);//might work when you run on different processors
    }
}



void add(IPaddress address, IPaddress clients[],int *pNrOfClients){
	for(int i=0;i<*pNrOfClients;i++) if(address.host==clients[i].host &&address.port==clients[i].port) return;
	clients[*pNrOfClients] = address;
	(*pNrOfClients)++;
}

void executeCommand(Game *pGame,ClientData cData){
    

    if(cData.command[1]==UP&& cData.command[6]!=BLOCKED) turnUp(pGame->pPlayers[cData.playerNumber]);
    if(cData.command[2]==DOWN&& cData.command[6]!=BLOCKED) turnDown(pGame->pPlayers[cData.playerNumber]);
    if(cData.command[3]==LEFT&& cData.command[6]!=BLOCKED) turnLeft(pGame->pPlayers[cData.playerNumber]);
    if(cData.command[4]==RIGHT&& cData.command[6]!=BLOCKED) turnRight(pGame->pPlayers[cData.playerNumber]);
    if(cData.command[5]==FIRE){
        printf("%d,cData.playerNumber in fire func\n", cData.playerNumber);
        pGame->bullets[pGame->num_bullets] = createBullet(pGame->pRenderer, cData.bulletStartX[cData.playerNumber], cData.bulletStartY[cData.playerNumber]);
        if (pGame->bullets[pGame->num_bullets]) {
            pGame->bullets[pGame->num_bullets]->dx = ((float)cData.bulletDx[cData.playerNumber])/100;
            pGame->bullets[pGame->num_bullets]->dy = ((float)cData.bulletDy[cData.playerNumber])/100;
            pGame->num_bullets++;
            printf("bulletStartX: %d, bulletStartY: %d, bulletDx: %d, bulletDy: %d\n", cData.bulletStartX[cData.playerNumber], cData.bulletStartY[cData.playerNumber], cData.bulletDx[cData.playerNumber], cData.bulletDy[cData.playerNumber]);
            return;
        }
    }
        


     if (cData.playerNumber < 0 || cData.playerNumber >= MAX_PLAYERS) {
        printf("Error: Invalid player number %d\n", cData.playerNumber);
        return;
    }

    // Check if coordinates are not negative
    if (cData.monkey.x < 0 || cData.monkey.y < 0) {
        printf("Error: Invalid coordinates for player %d: x=%d, y=%d\n", cData.playerNumber, cData.monkey.x, cData.monkey.y);
        return;
    }
    //track player's position (im guessing cData is not tracking the player's position, so we need to update it here)
    //printf("Player %d position: x=%f, y=%f\n", cData.playerNumber, cData.monkey.x, cData.monkey.y);

    //Update player data
    //pGame->pPlayers[cData.playerNumber]->health = cData.monkey.health;
    
    
    
}

void sendGameData(Game *pGame){
    ServerData sData;
    sData.gState = pGame->state;
    for(int i=0;i<MAX_MONKEYS;i++){
        sData.slotsTaken[i] = pGame->slotsTaken[i];
        sData.monkeys[i].x = pGame->pPlayers[i]->dest.x;
        sData.monkeys[i].y = pGame->pPlayers[i]->dest.y;
        sData.monkeys[i].sx = pGame->pPlayers[i]->source.x;
        sData.monkeys[i].sy = pGame->pPlayers[i]->source.y;
    }
    sData.numberOfPlayers = pGame->num_players;
    memcpy(pGame->pPacket->data, &(sData), sizeof(ServerData));
    pGame->pPacket->len = sizeof(ServerData);
    for(int i=0;i<pGame->num_players;i++){
        pGame->pPacket->address = pGame->serverAddress[i];
        if(0==SDLNet_UDP_Send(pGame->pSocket,-1,pGame->pPacket)) printf("SDLNet_UDP_Send: %s\n", SDLNet_GetError());
    }
}

void close(Game *pGame){
    for(int i=0;i<MAX_PLAYERS;i++) if(pGame->pPlayers[i]) destroyCharacter(pGame->pPlayers[i]);
    if(pGame->pRenderer) SDL_DestroyRenderer(pGame->pRenderer);
    if(pGame->pWindow) SDL_DestroyWindow(pGame->pWindow);
    
    for (int i = 0; i < pGame->num_bullets; i++) {
        if (pGame->bullets[i]) {
            destroyBullet(pGame->bullets[i]);
            pGame->bullets[i] = NULL;
        }
    }

    if(pGame->pPacket) SDLNet_FreePacket(pGame->pPacket);
	if(pGame->pSocket) SDLNet_UDP_Close(pGame->pSocket);

    destroyText(pGame->pWaitingText);

    TTF_CloseFont(pGame->font);
    TTF_Quit();
    SDLNet_Quit();    
    SDL_Quit();
}

