#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "treasure.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage:\n");
        printf("  %s add <hunt_id>\n", argv[0]);
        printf("  %s list <hunt_id>\n", argv[0]);
        printf("  %s view <hunt_id> <treasure_id>\n", argv[0]);
        printf("  %s remove_treasure <hunt_id> <treasure_id>\n", argv[0]);
        printf("  %s remove_hunt <hunt_id>\n", argv[0]);
        return 1;
    }

    const char* command = argv[1];
    const char* hunt_id = argv[2];

    if (strcmp(command, "add") == 0 && argc == 3) {
        add_treasure(hunt_id);
    } else if (strcmp(command, "list") == 0 && argc == 3) {
        list_treasures(hunt_id);
    } else if (strcmp(command, "view") == 0 && argc == 4) {
        int treasure_id = atoi(argv[3]);
        view_treasure(hunt_id, treasure_id);
    } else if (strcmp(command, "remove_treasure") == 0 && argc == 4) {
        int treasure_id = atoi(argv[3]);
        remove_treasure(hunt_id, treasure_id);
    } else if (strcmp(command, "remove_hunt") == 0 && argc == 3) {
        remove_hunt(hunt_id);
    } else {
        printf("Invalid command or arguments.\n");
        return 1;
    }

    return 0;
}
