#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "treasure.h"

// Funcție pentru adăugarea unei comori
void add_treasure(const char *hunt_id, Treasure *treasure) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s_treasures.dat", hunt_id);

    // Deschiderea fișierului pentru a adăuga la el
    int fd = open(filename, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Eroare deschidere fișier");
        return;
    }

    // Scrierea comorii în fișier
    ssize_t bytes_written = write(fd, treasure, sizeof(Treasure));
    if (bytes_written == -1) {
        perror("Eroare scriere fișier");
    }

    // Închiderea fișierului
    close(fd);
}

// Funcție pentru listarea comorilor dintr-un "hunt_id"
void list_treasures(const char *hunt_id) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s_treasures.dat", hunt_id);

    // Deschiderea fișierului pentru citire
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare deschidere fișier");
        return;
    }

    Treasure treasure;
    // Citirea fiecărei comori și afișarea informațiilor
    while (read(fd, &treasure, sizeof(Treasure)) > 0) {
        printf("ID: %d, Nume: %s, Lat: %.2f, Long: %.2f, Clue: %s, Valoare: %d\n",
            treasure.id, treasure.user_name, treasure.latitude, treasure.longitude,
            treasure.clue, treasure.value);
    }

    // Închiderea fișierului
    close(fd);
}

// Funcție pentru vizualizarea unei comori pe baza ID-ului
void view_treasure(const char *hunt_id, int treasure_id) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s_treasures.dat", hunt_id);

    // Deschiderea fișierului pentru citire
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare deschidere fișier");
        return;
    }

    Treasure treasure;
    // Citirea fiecărei comori și verificarea ID-ului
    while (read(fd, &treasure, sizeof(Treasure)) > 0) {
        if (treasure.id == treasure_id) {
            // Dacă găsim comoara cu ID-ul respectiv, o afișăm
            printf("ID: %d, Nume: %s, Lat: %.2f, Long: %.2f, Clue: %s, Valoare: %d\n",
                treasure.id, treasure.user_name, treasure.latitude, treasure.longitude,
                treasure.clue, treasure.value);
            break;
        }
    }

    // Închiderea fișierului
    close(fd);
}

// Funcție pentru ștergerea unei comori pe baza ID-ului
void remove_treasure(const char *hunt_id, int treasure_id) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s_treasures.dat", hunt_id);

    // Deschiderea fișierului pentru citire
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("Eroare deschidere fișier");
        return;
    }

    // Citirea și ștergerea comorii
    Treasure treasure;
    off_t offset;
    while ((offset = lseek(fd, 0, SEEK_CUR)) >= 0 && read(fd, &treasure, sizeof(Treasure)) > 0) {
        if (treasure.id == treasure_id) {
            // Setăm pointerul la începutul poziției pentru ștergere
            lseek(fd, offset, SEEK_SET);
            // Umplem cu zero pentru a șterge efectiv din fișier
            write(fd, &(Treasure){0}, sizeof(Treasure));
            break;
        }
    }

    // Închiderea fișierului
    close(fd);
}
