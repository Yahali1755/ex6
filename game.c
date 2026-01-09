#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "utils.h"

#define EXISTS_CHAR 'V'
#define MISSING_CHAR 'X'

// Map display functions
static void displayMap(GameState* g) {
    if (!g->rooms) return;
    
    // Find bounds
    int minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (Room* r = g->rooms; r; r = r->next) {
        if (r->x < minX) minX = r->x;
        if (r->x > maxX) maxX = r->x;
        if (r->y < minY) minY = r->y;
        if (r->y > maxY) maxY = r->y;
    }
    
    int width = maxX - minX + 1;
    int height = maxY - minY + 1;
    
    // Create grid
    int** grid = malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        grid[i] = malloc(width * sizeof(int));
        for (int j = 0; j < width; j++) grid[i][j] = -1;
    }
    
    for (Room* r = g->rooms; r; r = r->next)
        grid[r->y - minY][r->x - minX] = r->id;
    
    printf("=== SPATIAL MAP ===\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (grid[i][j] != -1) printf("[%2d]", grid[i][j]);
            else printf("    ");
        }
        printf("\n");
    }

    printf("=== ROOM LEGEND ===\n");
    for (Room* r = g->rooms; r; r = r->next) {
        char hasItem = r->item == NULL ? MISSING_CHAR : EXISTS_CHAR;
        char hasMonster = r->monster == NULL ? MISSING_CHAR : EXISTS_CHAR;

        printf("ID %d: [M:%c] [I:%c]\n", r->id, hasMonster, hasItem );
    }
    printf("==================\n");

    for (int i = 0; i < height; i++) free(grid[i]);
    free(grid);
}

Room* findRoomByCoordinates(GameState* g, int x, int y) {
    Room* currentRoom = g->rooms;

    while (currentRoom != NULL) {
        if (currentRoom->x == x && currentRoom->y == y) {
            return currentRoom;
        }

        currentRoom = currentRoom->next;
    }

    return NULL;
}

Room* findRoomById(GameState* g, int id) {
    Room* currentRoom = g->rooms;

    while (currentRoom != NULL) {
        if (currentRoom->id == id) {
            return currentRoom;
        }

        currentRoom = currentRoom->next;
    }

    return NULL;
}

void addRoom(GameState* gameState) {
    Room *lastestRoom = gameState->rooms;

    if (lastestRoom != NULL) {
        displayMap(gameState);
    }

    Room *newRoom = malloc(sizeof(Room));

    if (newRoom == NULL)
        return;

    newRoom->monster = NULL;
    newRoom->item = NULL;
    newRoom->visited = 0;

    if (lastestRoom == NULL) {
        newRoom->id = 0;
        newRoom->x = 0;
        newRoom->y = 0;
        gameState->roomCount++;
    } else {
        int direction; 
        int id = getInt("Attach to room ID:\n");

        Room *roomToAttachTo = findRoomById(gameState, id);

        direction = getInt("Direction (0=Up,1=Down,2=Left,3=Right):\n");

        int newRoomX = roomToAttachTo->x;
        int newRoomY = roomToAttachTo->y;

        if (direction == 0) {
            newRoomY--;
        } else if (direction == 1) {
            newRoomY++;
        } else if (direction == 2) {
            newRoomX--;
        } else {
            newRoomX++;
        }

        if (findRoomByCoordinates(gameState, newRoomX, newRoomY) != NULL) {
            printf("Room exists there\n");
            free(newRoom);

            return;
        }

        newRoom->id = gameState->roomCount++;
        newRoom->x = newRoomX;
        newRoom->y = newRoomY;
    }

    int shouldAddMonster = getInt("Add monster? (1=Yes, 0=No)\n");

    if (shouldAddMonster == 1) {
        addMonster(newRoom);
    }

    int shouldAddItem = getInt("Add item? (1=Yes, 0=No):\n");

    if (shouldAddItem == 1) {
        addItem(newRoom);
    }

    newRoom->next = gameState->rooms;
    gameState->rooms = newRoom;

    printf("Created room %d at (%d,%d)", newRoom->id, newRoom->x, newRoom->y);
}

void addMonster(Room* room) {
    Monster *newMonster = malloc(sizeof(Monster));

    newMonster->name = getString("Monster name:\n");
    newMonster->type = getInt("Type (0-4):\n");
    newMonster->hp = getInt("HP:\n");
    newMonster->attack = getInt("Attack:\n");
    newMonster->maxHp = newMonster->hp;
    room->monster = newMonster;
}

void addItem(Room* room) {
    Item *newItem = malloc(sizeof(Item));

    newItem->name = getString("Item name:\n");
    newItem->type = getInt("Type (0=Armor, 1=Sword):\n");
    newItem->value = getInt("Value:\n");
    room->item = newItem;
}

// Return 1 if first is bigger, or 0 otherwise, -1 if identical
int compareItems(void* a, void* b) {
    Item* item1 = (Item*)a;
    Item* item2 = (Item*)b;

    int compareItemNames = strcmp(item1->name, item2->name);

    if (compareItemNames > 0) {
        return 1;
    } else if (compareItemNames < 0) {
        return 0;
    }

    if (item1->value > item2->value) {
        return 1;
    } else if (item1->value < item2->value) {
        return 0;
    }

    if (item1->type > item2->type) {
        return 1;
    } else if (item1->type < item2->type) {
        return 0;
    }

    // Items are identical
    return INVALID_INDEX;
}

// Return 1 if first is bigger, or 0 otherwise, -1 if identical
int compareMonsters(void* a, void* b) {
    Monster* monster1 = (Monster*)a;
    Monster* monster2 = (Monster*)b;

    int compareItemNames = strcmp(monster1->name, monster2->name);

    if (compareItemNames > 0) {
        return 1;
    } else if (compareItemNames < 0) {
        return 0;
    }

    if (monster1->attack > monster2->attack) {
        return 1;
    } else if (monster1->attack < monster2->attack) {
        return 0;
    }

    if (monster1->hp > monster2->hp) {
        return 1;
    } else if (monster1->hp < monster2->hp) {
        return 0;
    }

    if (monster1->type > monster2->type) {
        return 1;
    } else if (monster1->type < monster2->type) {
        return 0;
    }

    // Items are identical
    return INVALID_INDEX;
}

void printItem(void* data) {
    Item* item = (Item*)data;
    
    char* typeNames[] = {"ARMOR", "SWORD"};

    printf("[%s] %s - Value: %d\n", typeNames[item->type], item->name, item->value);
}

void printMonster(void* data) {
    Monster* monster = (Monster*)data;
    
    char* typeNames[] = {"Phantom", "Spider", "Demon", "Golem", "Cobra"};

    printf("[%s] Type: %s, Attack: %d, HP: %d\n", monster->name, typeNames[monster->type], monster->attack, monster->hp);
}

void freeItem(void* data) {
    Item* item = (Item*)data;
    
    char* typeNames[] = {"ARMOR", "SWORD"};

    printf("[%s] %s - Value: %d\n", typeNames[item->type], item->name, item->value);
}

void initPlayer(GameState* gameState) {
    Player *player = malloc(sizeof(Player));

    player->maxHp = gameState->configMaxHp;
    player->hp = gameState->configMaxHp;
    player->baseAttack = gameState->configBaseAttack;
    player->bag = createBST(compareItems, printItem, printItem);
    player->defeatedMonsters = createBST(compareMonsters, printMonster, printItem);
    player->currentRoom=NULL;
}