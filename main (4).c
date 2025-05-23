#include "treasure.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("eroare argumente\nUse: %s --<command> <hunt_id> [<treasure_id>]\n", argv[0]);
        return 1;
    }

    const char *command = argv[1];
    const char *hunt_id = argv[2];

    int treasure_id;
    if (argc == 4)
	treasure_id = atoi(argv[3]);

    if (strcmp(command, "--add") == 0) {
        Treasure t;
	printf("Treasure ID: ");
	scanf("%d", &t.id);
	printf("Username: ");
	scanf("%49s", t.user_name);
	printf("Latitude: ");
	scanf("%lf", &t.latitude);
	printf("Longitude: ");
	scanf("%lf", &t.longitude);
	printf("Clue: ");
	getchar();
	fgets(t.clue, 100, stdin);
	t.clue[strcspn(t.clue, "\n")] = '\0';
	printf("Value: ");
	scanf("%d", &t.value);
      
	add_treasure(hunt_id, &t);
    } else if (strcmp(command, "--list") == 0)
        list_treasures(hunt_id);
    else if (strcmp(command, "--view") == 0 && argc == 4)
        view_treasure(hunt_id, treasure_id);
    else if (strcmp(command, "--remove_treasure") == 0 && argc == 4)
        remove_treasure(hunt_id, treasure_id);
    else if (strcmp(command, "--remove_hunt") == 0) {
        remove_hunt(hunt_id);
    } else {
        printf("operatie invalida\n");
	printf("./main --add HuntXXX\n");
	printf("./main --list HuntXXX\n");
	printf("./main --view HuntXXX treasure_id\n");
	printf("./main --remove_treasure HuntXXX treasure_id\n");
	printf("./main --remove_hunt HuntXXX\n\n");
        return 2;
    }

    return 0;
}
