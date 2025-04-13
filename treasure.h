#ifndef TREASURE_H
#define TREASURE_H

#define USERNAME_SIZE 32
#define CLUE_SIZE 128
#define MAX_PATH 512
#define LOG_FILE "logged_hunt.txt"    //fisier de log
#define TREASURE_FILE "treasures.dat" //fisier de comori

typedef struct {
    int id;
    char username[USERNAME_SIZE];
    float latitude;
    float longitude;
    char clue[CLUE_SIZE];
    int value;
} Treasure;

void add_treasure(const char* hunt_id);
void list_treasures(const char* hunt_id);
void view_treasure(const char* hunt_id, int treasure_id);
void remove_treasure(const char* hunt_id, int treasure_id);
void remove_hunt(const char* hunt_id);
void log_action(const char* hunt_id, const char* action);
void ensure_hunt_directory(const char* hunt_id);

#endif
