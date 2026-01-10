#ifndef GAME_H
#define GAME_H

#include "bst.h"

typedef enum { ARMOR, SWORD } ItemType;
typedef enum { PHANTOM, SPIDER, DEMON, GOLEM, COBRA } MonsterType;
typedef void (*GameFunc)(GameState*);
typedef enum { KEEP_RUNNING, EXIT_GAME } ProgramStatus;

typedef struct Item {
    char* name;
    ItemType type;
    int value;
} Item;

typedef struct Monster {
    char* name;
    MonsterType type;
    int hp;
    int maxHp;
    int attack;
} Monster;

typedef struct Room {
    int id;
    int x, y;
    int visited;
    Monster* monster;
    Item* item;
    struct Room* next;
} Room;

typedef struct Player {
    int hp;
    int maxHp;
    int baseAttack;
    BST* bag;
    BST* defeatedMonsters;
    Room* currentRoom;
} Player;

typedef struct {
    Room* rooms;
    Player* player;
    int roomCount;
    int configMaxHp;
    int configBaseAttack;
} GameState;

// Monster functions
void freeMonster(void* data);
int compareMonsters(void* a, void* b);
void printMonster(void* data);
void addMonster(Room* room); 

// Item functions
void freeItem(void* data);
int compareItems(void* a, void* b);
void printItem(void* data);
void addItem(Room* room); 

Room* findRoomByCoordinates(GameState* g, int x, int y);
Room* findRoomById(GameState* g, int id);
void addRoom(GameState* g);
void printRoom(Room* room, Player* player);

void initPlayer(GameState* g);
void playGame(GameState* g);
void freeGame(GameState* g);
int isPlayerVictory(GameState* gameState);

void move(GameState* gameState);
void pickup(GameState* gameState);
void bag(GameState* gameState);
void fight(GameState* gameState);
void defeated(GameState* gameState);

#endif
