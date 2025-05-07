#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "treasure.h"

// functie pentru adaugarea unei comori
void add_treasure(const char *hunt_id, Treasure *treasure) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s_treasures.dat", hunt_id);

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
    snprintf(filename, sizeof(filename), "%s_treasures.dat", hunt_id);

    // desc fisierul pentru citire
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare deschidere fișier");
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
    snprintf(filename, sizeof(filename), "%s_treasures.dat", hunt_id);

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

    // inchiderea fișierului
    close(fd);
}

// functie pentru ștergerea unei comori pe baza ID-ului
void remove_treasure(const char *hunt_id, int treasure_id) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s_treasures.dat", hunt_id);

    // deschiderea fișierului pentru citire
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("Eroare deschidere fisier");
        return;
    }

    // citire si stergere a comorii
    Treasure treasure;
    off_t offset;
    while ((offset = lseek(fd, 0, SEEK_CUR)) >= 0 && read(fd, &treasure, sizeof(Treasure)) > 0) {
        if (treasure.id == treasure_id) {
            // setam pointerul la inceputul poz pentru stergere
            lseek(fd, offset, SEEK_SET);
            // umplem cu zero pentru a sterge efectiv din fisier
            write(fd, &(Treasure){0}, sizeof(Treasure));
            break;
        }
    }

    // inchiderea fisierului
    close(fd);
}
