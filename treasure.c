#include "treasure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>


void ensure_hunt_directory(const char* hunt_id) {
    char dir_path[MAX_PATH];
    snprintf(dir_path, sizeof(dir_path), "./%s", hunt_id);
    mkdir(dir_path, 0755);

    //creeaza legatura simbolica pentru logged_hunt
    char symlink_path[MAX_PATH];
    snprintf(symlink_path, sizeof(symlink_path), "logged_hunt-%s", hunt_id);

    char log_path[MAX_PATH];
    snprintf(log_path, sizeof(log_path), "./%s/%s", hunt_id, LOG_FILE);

    //creeaza legatura simbolica doar daca nu exista deja
    symlink(log_path, symlink_path);
}

void log_action(const char* hunt_id, const char* action) {
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "./%s/%s", hunt_id, LOG_FILE);

    //verificarea existentei fisierului de log 
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) {
        printf("Log file does not exist, skipping log action\n");
        return;
    }

    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("log_action");
        return;
    }

    time_t now = time(NULL);
    char* time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = 0; // scoate newline

    char log_entry[256];
    snprintf(log_entry, sizeof(log_entry), "[%s] %s\n", time_str, action);

    write(fd, log_entry, strlen(log_entry));
    close(fd);
}

void add_treasure(const char* hunt_id) {
    ensure_hunt_directory(hunt_id);

    Treasure new_t;
    printf("Enter Treasure ID: ");
    scanf("%d", &new_t.id);

    //verificarea existentei ID ului
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "./%s/%s", hunt_id, TREASURE_FILE);

    int fd = open(path, O_RDONLY);
    if (fd >= 0) {
        Treasure temp;
        while (read(fd, &temp, sizeof(Treasure)) == sizeof(Treasure)) {
            if (temp.id == new_t.id) {
                printf("Error: Treasure with ID %d already exists!\n", new_t.id);
                close(fd);
                return;
            }
        }
        close(fd);
    }

    //restul datelor
    printf("Enter Username: ");
    scanf("%s", new_t.username);

    printf("Enter Latitude: ");
    scanf("%f", &new_t.latitude);

    printf("Enter Longitude: ");
    scanf("%f", &new_t.longitude);

    printf("Enter Clue: ");
    getchar(); // pt newline-ul ramas
    fgets(new_t.clue, CLUE_SIZE, stdin);
    new_t.clue[strcspn(new_t.clue, "\n")] = 0;

    printf("Enter Value: ");
    scanf("%d", &new_t.value);

    //se copiaza comoara in fisier
    fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("add_treasure open");
        return;
    }

    write(fd, &new_t, sizeof(Treasure));
    close(fd);

    log_action(hunt_id, "Added a treasure");
    printf("Treasure added successfully!\n");
}

void list_treasures(const char* hunt_id) {
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "./%s/%s", hunt_id, TREASURE_FILE);

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("list_treasures open");
        return;
    }

    Treasure temp;
    printf("Listing treasures in hunt: %s\n", hunt_id);

    //citim fiecare comoara si o afisam
    while (read(fd, &temp, sizeof(Treasure)) == sizeof(Treasure)) {
        printf("ID: %d\n", temp.id);
        printf("Username: %s\n", temp.username);
        printf("Latitude: %.2f\n", temp.latitude);
        printf("Longitude: %.2f\n", temp.longitude);
        printf("Clue: %s\n", temp.clue);
        printf("Value: %d\n", temp.value);
        printf("-----------------------\n");
    }

    close(fd);
}

void view_treasure(const char* hunt_id, int treasure_id) {
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "./%s/%s", hunt_id, TREASURE_FILE);

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open treasure file");
        return;
    }

    Treasure t;
    int found = 0;

    //citim comorile pe rand
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.id == treasure_id) {
            printf("Treasure Details:\n");
            printf("ID: %d\n", t.id);
            printf("Username: %s\n", t.username);
            printf("Latitude: %.6f\n", t.latitude);
            printf("Longitude: %.6f\n", t.longitude);
            printf("Clue: %s\n", t.clue);
            printf("Value: %d\n", t.value);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Treasure with ID %d not found.\n", treasure_id);
    }

    close(fd);
}


void remove_treasure(const char* hunt_id, int treasure_id) {
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "./%s/%s", hunt_id, TREASURE_FILE);

    int fd = open(path, O_RDWR);
    if (fd < 0) {
        perror("remove_treasure open");
        return;
    }

    int temp_fd = open("temp.dat", O_WRONLY | O_CREAT, 0644);
    if (temp_fd < 0) {
        perror("remove_treasure temp file");
        close(fd);
        return;
    }

    Treasure temp;
    int found = 0;

    while (read(fd, &temp, sizeof(Treasure)) == sizeof(Treasure)) {
        if (temp.id != treasure_id) {
            write(temp_fd, &temp, sizeof(Treasure));
        } else {
            found = 1;
        }
    }

    close(fd);
    close(temp_fd);

    if (found) {
        remove(path);
        rename("temp.dat", path);
        printf("Treasure with ID %d removed successfully!\n", treasure_id);
        log_action(hunt_id, "Removed a treasure");
    } else {
        printf("Error: Treasure with ID %d not found!\n", treasure_id);
    }
}

#define MAX_PATH 512

void remove_hunt(const char* hunt_id) {
    char dir_path[MAX_PATH];
    snprintf(dir_path, sizeof(dir_path), "./%s", hunt_id);

    char treasure_file[MAX_PATH];
    if ((size_t)snprintf(treasure_file, sizeof(treasure_file), "%s/%s", dir_path, TREASURE_FILE) >= sizeof(treasure_file)) {
        fprintf(stderr, "Path too long\n");
        return;
    }

    char log_file[MAX_PATH];
    if ((size_t)snprintf(log_file, sizeof(log_file), "%s/%s", dir_path, LOG_FILE) >= sizeof(log_file)) {
        fprintf(stderr, "Path too long\n");
        return;
    }

    //se sterge fisierul de comori
    if (unlink(treasure_file) == -1 && errno != ENOENT) {
        perror("Could not delete treasure file");
        return;
    }

    //se sterge fisierul de log
    if (unlink(log_file) == -1 && errno != ENOENT) {
        perror("Could not delete log file");
        return;
    }

    //se sterge legatura simbolica
    char symlink_path[MAX_PATH];
    snprintf(symlink_path, sizeof(symlink_path), "logged_hunt-%s", hunt_id);
    unlink(symlink_path);  

    //se sterge directorul
    if (rmdir(dir_path) == -1) {
        perror("Could not remove hunt directory");
        return;
    }

    printf("Hunt '%s' removed successfully!\n", hunt_id);
}
