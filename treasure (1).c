#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include "treasure.h"

// functie pentru adaugarea unei comori
void add_treasure(const char *hunt_id, Treasure *treasure) {
    mkdir(hunt_id, 0755);
  
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/treasures.dat", hunt_id);

    // deschiderea fisierului pentru a adaquga la el
    int fd = open(filename, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Eroare deschidere fisier");
        return;
    }

    // scrierea comorii in fisier
    ssize_t bytes_written = write(fd, treasure, sizeof(Treasure));
    if (bytes_written == -1) {
        perror("Eroare scriere fisier");
    }

    // incihderea fisierului
    close(fd);
}

// functie pentru listarea comorilor dintr-un "hunt_id"
void list_treasures(const char *hunt_id) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/treasures.dat", hunt_id);

    // desc fisierul pentru citire
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare deschidere fisier");
        return;
    }

    Treasure treasure;
    // cit fiec comoara si afiseaza info
    while (read(fd, &treasure, sizeof(Treasure)) > 0) {
        printf("ID: %d, Nume: %s, Lat: %.2f, Long: %.2f, Clue: %s, Valoare: %d\n",
            treasure.id, treasure.user_name, treasure.latitude, treasure.longitude,
            treasure.clue, treasure.value);
    }

    // inchidere fisier
    close(fd);
}

// functie pentru vizualizarea unei comori pe baza ID-ului
void view_treasure(const char *hunt_id, int treasure_id) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/treasures.dat", hunt_id);

    // desc fisier pentru citire
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare deschidere fisier");
        return;
    }

    Treasure treasure;
    // citirea fiecarei comori si verificarea ID-ului
    while (read(fd, &treasure, sizeof(Treasure)) > 0) {
        if (treasure.id == treasure_id) {
            // daca gasim comoara cu ID-ul respectiv, o afisam
            printf("ID: %d, Nume: %s, Lat: %.2f, Long: %.2f, Clue: %s, Valoare: %d\n",
                treasure.id, treasure.user_name, treasure.latitude, treasure.longitude,
                treasure.clue, treasure.value);
            break;
        }
    }

    // inchiderea fisierului
    close(fd);
}

// functie pentru stergerea unei comori pe baza ID-ului
void remove_treasure(const char *hunt_id, int treasure_id) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/treasures.dat", hunt_id);

    char tmp[] = "./tmp_treasuresXXXXXX";//XXXXXX sunt placeholdere pentru a asigura dimensiunea
    int tmp_fd = mkstemp(tmp);

    // deschiderea fisierului pentru citire
    int fd = open(filename, O_RDWR);
    if (fd == -1 || tmp_fd == -1) {
        perror("Eroare deschidere fisier");
        return;
    }

    // citire si stergere a comorii
    Treasure treasure;
    int found = 0;
    while (read(fd, &treasure, sizeof(Treasure)) == sizeof(Treasure)) {
        if (treasure.id != treasure_id) {
            write(tmp_fd, &treasure, sizeof(Treasure));
        } else {
            found = 1;
        }
    }

    // inchiderea fisierului
    close(fd);
    close(tmp_fd);

    if (found) {
        rename(tmp, filename);
        printf("Treasure removed.\n");
    } else {
        unlink(tmp);
        printf("Treasure not found.\n");
    }
}

void remove_hunt(const char *hunt_id) {
    char path[256];
    snprintf(path, sizeof(path), "./%s", hunt_id);
    
    char data_path[256];
    snprintf(data_path, sizeof(data_path), "%s/treasures.dat", hunt_id);

    unlink(data_path);
    rmdir(path);

    printf("Hunt '%s' removed.\n", hunt_id);
}
