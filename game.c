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
    printf("===================\n");

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
        int id = getInt("Attach to room ID: ");

        Room *roomToAttachTo = findRoomById(gameState, id);

        direction = getInt("Direction (0=Up,1=Down,2=Left,3=Right): ");

        int newRoomX = roomToAttachTo->x;
        int newRoomY = roomToAttachTo->y;

        if (direction == 0) {
            newRoomY--;
        } else if (direction == 1) {
            newRoomY++;
        } else if (direction == 2) {
            newRoomX--;
        } else if (direction == 3) {
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

    int shouldAddMonster = getInt("Add monster? (1=Yes, 0=No): ");

    if (shouldAddMonster == 1) {
        addMonster(newRoom);
    }

    int shouldAddItem = getInt("Add item? (1=Yes, 0=No): ");

    if (shouldAddItem == 1) {
        addItem(newRoom);
    }

    newRoom->next = gameState->rooms;
    gameState->rooms = newRoom;

    printf("Created room %d at (%d,%d)\n", newRoom->id, newRoom->x, newRoom->y);
}

void addMonster(Room* room) {
    Monster *newMonster = malloc(sizeof(Monster));

    newMonster->name = getString("Monster name: ");
    newMonster->type = getInt("Type (0-4): ");
    newMonster->hp = getInt("HP: ");
    newMonster->attack = getInt("Attack: ");
    newMonster->maxHp = newMonster->hp;
    room->monster = newMonster;
}

void addItem(Room* room) {
    Item *newItem = malloc(sizeof(Item));

    newItem->name = getString("Item name: ");
    newItem->type = getInt("Type (0=Armor, 1=Sword): ");
    newItem->value = getInt("Value: ");
    room->item = newItem;
}

// Return 1 if left is bigger, -1 if right is bigger, 0 if identical
int compareItems(void* a, void* b) {
    Item* item1 = (Item*)a;
    Item* item2 = (Item*)b;

    int compareItemNames = strcmp(item1->name, item2->name);

    if (compareItemNames > 0) {
        return 1;
    } else if (compareItemNames < 0) {
        return -1;
    }

    if (item1->value > item2->value) {
        return 1;
    } else if (item1->value < item2->value) {
        return -1;
    }

    if (item1->type > item2->type) {
        return 1;
    } else if (item1->type < item2->type) {
        return -1;
    }

    // Items are identical
    return 0;
}

// Return 1 if left is bigger, -1 if right is bigger, 0 if identical
int compareMonsters(void* a, void* b) {
    Monster* monster1 = (Monster*)a;
    Monster* monster2 = (Monster*)b;

    int compareItemNames = strcmp(monster1->name, monster2->name);

    if (compareItemNames > 0) {
        return 1;
    } else if (compareItemNames < 0) {
        return -1;
    }

    if (monster1->attack > monster2->attack) {
        return 1;
    } else if (monster1->attack < monster2->attack) {
        return -1;
    }

    if (monster1->maxHp > monster2->maxHp) {
        return 1;
    } else if (monster1->maxHp < monster2->maxHp) {
        return -1;
    }

    if (monster1->type > monster2->type) {
        return 1;
    } else if (monster1->type < monster2->type) {
        return -1;
    }

    // Items are identical
    return 0;
}

void printItem(void* data) {
    Item* item = (Item*)data;
    
    char* typeNames[] = {"ARMOR", "SWORD"};

    printf("[%s] %s - Value: %d\n", typeNames[item->type], item->name, item->value);
}

void printMonster(void* data) {
    Monster* monster = (Monster*)data;
    
    char* typeNames[] = {"Phantom", "Spider", "Demon", "Golem", "Cobra"};

    printf("[%s] Type: %s, Attack: %d, HP: %d\n", monster->name, typeNames[monster->type], monster->attack, monster->maxHp);
}

void freeItem(void* data) {
    if (data == NULL) 
        return;

    Item* item = (Item*)data;

    free(item->name);
    free(item);
}

void freeMonster(void* data) {
    if (data == NULL) 
        return;

    Monster* monster = (Monster*)data;

    free(monster->name);
    free(monster);
}

void initPlayer(GameState* gameState) {
    if (gameState->rooms == NULL) {
        printf("Create rooms first\n");

        return;
    }

    Player *player = malloc(sizeof(Player));

    if (player == NULL) 
        return;

    player->maxHp = gameState->configMaxHp;
    player->hp = gameState->configMaxHp;
    player->baseAttack = gameState->configBaseAttack;
    player->bag = createBST(compareItems, printItem, freeItem);
    player->defeatedMonsters = createBST(compareMonsters, printMonster, freeMonster);
    player->currentRoom=NULL;
    gameState->player = player;
}

void printRoom(Room* room, Player* player) {
    printf("--- Room %d ---\n", room->id);

    if (room->monster != NULL)
        printf("Monster: %s (HP:%d)\n", room->monster->name, room->monster->hp);

    if (room->item != NULL)
        printf("Item: %s\n", room->item->name);

    printf("HP: %d/%d\n", player->hp, player->maxHp);
}

void move(GameState* gameState) {
    if (gameState->player->currentRoom->monster != NULL) {
        printf("Kill monster first\n");

        return;
    }

    int direction = getInt("Direction (0=Up,1=Down,2=Left,3=Right): ");
    Room *room;

    if (direction == 0) {
        room = findRoomByCoordinates(gameState, gameState->player->currentRoom->x,
             gameState->player->currentRoom->y - 1);
    } else if (direction == 1) {
        room = findRoomByCoordinates(gameState, gameState->player->currentRoom->x,
             gameState->player->currentRoom->y + 1);
    } else if (direction == 2) {
        room = findRoomByCoordinates(gameState, gameState->player->currentRoom->x - 1,
             gameState->player->currentRoom->y);
    } else if (direction == 3){
        room = findRoomByCoordinates(gameState, gameState->player->currentRoom->x + 1,
             gameState->player->currentRoom->y);
    }

    if (room == NULL) {
        printf("No room there\n");
    } else {
        gameState->player->currentRoom = room;
        room->visited = 1;

        if (isPlayerVictory(gameState) == 1) {
            printOnVictory();

            freeGame(gameState);
            exit(0);
        }
    }
}

void printOnVictory() {
    printf("***************************************\n");
    printf("VICTORY!\n");
    printf("All rooms explored. All monsters defeated.\n");
    printf("***************************************\n");
}

int isPlayerVictory(GameState* gameState) {
    for (Room* room = gameState->rooms; room; room = room->next) {
        if (room->monster != NULL || room->visited == 0) {
            return 0;
        }
    }

    return 1;
}

void fight(GameState* gameState) {
    Monster *monster = gameState->player->currentRoom->monster;
    Player *player = gameState->player;

    if (monster == NULL) {
        printf("No monster\n");

        return;
    }

    while (monster->hp >= 0 && player->hp >=0) {
        monster->hp = monster->hp - player->baseAttack;
        printf("You deal %d damage. Monster HP: %d\n", player->baseAttack, monster->hp < 0 ? 0 : monster->hp);

        if (monster->hp <= 0) break;

        player->hp = player->hp - monster->attack;
        printf("Monster deals %d damage. Your HP: %d\n", monster->attack, player->hp);
    }

    if (player->hp <= 0) {
        printf("--- YOU DIED ---\n");
        freeGame(gameState);
        exit(0);
    } else {
        printf("Monster defeated!\n");

        player->defeatedMonsters->root = bstInsert(player->defeatedMonsters->root, monster,
             player->defeatedMonsters->compare);
        gameState->player->currentRoom->monster = NULL;

        if (isPlayerVictory(gameState) == 1) {
            printOnVictory();

            freeGame(gameState);
            exit(0);
        }
    }
}

void pickup(GameState* gameState) {
    if (gameState->player->currentRoom->monster != NULL) {
        printf("Kill monster first\n");

        return;
    }

    Item *item = gameState->player->currentRoom->item;

    if (item == NULL) {
        printf("No item here\n");

        return;
    }

    if (bstFind(gameState->player->bag->root, item, gameState->player->bag->compare) != NULL) {
        printf("Duplicate item.\n");

        return;
    }

    gameState->player->bag->root = bstInsert(gameState->player->bag->root, item, gameState->player->bag->compare);
    gameState->player->currentRoom->item = NULL;
    printf("Picked up %s\n", item->name);
}

void bag(GameState* gameState) {
    printf("=== INVENTORY ===\n");
    int printByOrder = getInt("1.Preorder 2.Inorder 3.Postorder\n");

    if (printByOrder == 1) {
        bstPreorder(gameState->player->bag->root, printItem);
    } else if (printByOrder == 2) {
        bstInorder(gameState->player->bag->root, printItem);
    } else if (printByOrder == 3) {
        bstPostorder(gameState->player->bag->root, printItem);
    }
}

void defeated(GameState* gameState) {
    printf("=== DEFEATED MONSTERS ===\n");
    int printByOrder = getInt("1.Preorder 2.Inorder 3.Postorder\n");
    
    if (printByOrder == 1) {
        bstPreorder(gameState->player->defeatedMonsters->root, printMonster);
    } else if (printByOrder == 2) {
        bstInorder(gameState->player->defeatedMonsters->root, printMonster);
    } else if (printByOrder == 3) {
        bstPostorder(gameState->player->defeatedMonsters->root, printMonster);
    }
}

void freeGame(GameState* gameState) {
    Player *player = gameState->player;
    Room* room = gameState->rooms;

    if (player != NULL) {
        if (player->bag != NULL) {
            bstFree(player->bag->root, player->bag->freeData);
            free(player->bag);
        }

        if (player->defeatedMonsters != NULL) {
            bstFree(player->defeatedMonsters->root, player->defeatedMonsters->freeData);
            free(player->defeatedMonsters); 
        }

        free(player);
    }

    while (room != NULL) {
        Room* nextRoom = room->next;
        freeItem(room->item);
        freeMonster(room->monster);
        free(room);
        room = nextRoom;
    }

    gameState->rooms = NULL;
    gameState->player = NULL;
}

void playGame(GameState* gameState) {
    if (gameState->player == NULL || gameState->rooms == NULL) {
        printf("Init player first\n");

        return;
    }

    if (gameState->player->currentRoom == NULL) {
        // ensure initialization of room in start of game
        gameState->player->currentRoom = findRoomByCoordinates(gameState, 0, 0);
        gameState->player->currentRoom->visited = 1;
    }

    int notDefeated = 1;

    GameFunc actions[] = {move, fight, pickup, bag, defeated};

    while (notDefeated) {
        displayMap(gameState);
        printRoom(gameState->player->currentRoom, gameState->player);

        int choice = getInt("1.Move 2.Fight 3.Pickup 4.Bag 5.Defeated 6.Quit\n");

        if (choice >= 1 && choice <= 5) {
            actions[choice - 1](gameState);
        }

        if (choice == 6 || choice == INVALID_INDEX) {
            return;
        }
    }

    freeGame(gameState);
}