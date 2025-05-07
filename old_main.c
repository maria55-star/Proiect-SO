#include "treasure.h"

// adaugam o comoara intr un fisier
int add_treasure(const char *hunt_id, Treasure *treasure) {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s/treasures.dat", hunt_id);

    int fd = open(filename, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Eroare deschidere fisier");
        return -1;
    }

    ssize_t bytes_written = write(fd, treasure, sizeof(Treasure));
    if (bytes_written == -1) {
        perror("Eroare scriere in fisier");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

// listare toate comorile dintr-un anumit hunt
int list_treasures(const char *hunt_id) {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s/treasures.dat", hunt_id);

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare deschidere fisier");
        return -1;
    }

    Treasure treasure;
    while (read(fd, &treasure, sizeof(Treasure)) > 0) {
        printf("ID: %d, User: %s, Locatie: (%lf, %lf), Indiciu: %s, Valoare: %d\n", 
            treasure.id, treasure.username, treasure.latitude, treasure.longitude, treasure.clue, treasure.value);
    }

    close(fd);
    return 0;
}

// Vizualizeaza o comoara specifica
int view_treasure(const char *hunt_id, int treasure_id) {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s/treasures.dat", hunt_id);

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare deschidere fisier");
        return -1;
    }

    Treasure treasure;
    while (read(fd, &treasure, sizeof(Treasure)) > 0) {
        if (treasure.id == treasure_id) {
            printf("ID: %d, User: %s, Locatie: (%lf, %lf), Indiciu: %s, Valoare: %d\n", 
                treasure.id, treasure.username, treasure.latitude, treasure.longitude, treasure.clue, treasure.value);
            close(fd);
            return 0;
        }
    }

    printf("Comoara cu ID-ul %d nu a fost gasita.\n", treasure_id);
    close(fd);
    return -1;
}

// stegrge o comoara dintr-un hunt
int remove_treasure(const char *hunt_id, int treasure_id) {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s/treasures.dat", hunt_id);

    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("Eroare deschidere fisier");
        return -1;
    }

    Treasure treasure;
    int offset = 0;
    int found = 0;
    while (read(fd, &treasure, sizeof(Treasure)) > 0) {
        offset++;
        if (treasure.id == treasure_id) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Comoara cu ID-ul %d nu a fost gasita.\n", treasure_id);
        close(fd);
        return -1;
    }

    
    lseek(fd, offset * sizeof(Treasure), SEEK_SET);
    Treasure emptyTreasure = {0};
    write(fd, &emptyTreasure, sizeof(Treasure));
    close(fd);
    return 0;
}
