#ifndef TREASURE_H
#define TREASURE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    char user_name[50];
    double latitude;
    double longitude;
    char clue[100];
    int value;
} Treasure;

void add_treasure(const char *hunt_id, Treasure *treasure);
void list_treasures(const char *hunt_id);
void view_treasure(const char *hunt_id, int id);
void remove_treasure(const char *hunt_id, int id);
void list_hunts(); // Adăugat prototipul pentru list_hunts

#endif
