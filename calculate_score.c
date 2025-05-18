#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#define MAX_USERS 100
#define MAX_USERNAME_LEN 32
#define MAX_LINE_LEN 256

typedef struct {
    char username[MAX_USERNAME_LEN];
    int total_value;
} UserScore;

int find_user(UserScore *scores, int count, const char *username) {
    for (int i = 0; i < count; i++) {
        if (strcmp(scores[i].username, username) == 0)
            return i;
    }
    return -1;
}

// Functie simplificata pentru citirea liniei din fisier folosind read
// Citeste pana la '\n' sau EOF in buffer
ssize_t read_line(int fd, char *buffer, size_t max_len) {
    ssize_t total_read = 0;
    while (total_read < (ssize_t)(max_len - 1)) {
        char c;
        ssize_t r = read(fd, &c, 1);
        if (r <= 0) break;
        if (c == '\n') break;
        buffer[total_read++] = c;
    }
    buffer[total_read] = '\0';
    return total_read;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        const char *msg = "Usage: calculate_score <hunt_id>\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return 1;
    }

    const char *hunt_id = argv[1];
    char hunt_path[256];
    snprintf(hunt_path, sizeof(hunt_path), "treasure hunts/hunt:%s", hunt_id);

    DIR *dir = opendir(hunt_path);
    if (!dir) {
        char errbuf[256];
        snprintf(errbuf, sizeof(errbuf), "Error: cannot open directory %s\n", hunt_path);
        write(STDERR_FILENO, errbuf, strlen(errbuf));
        return 1;
    }

    UserScore scores[MAX_USERS];
    int user_count = 0;
    for (int i = 0; i < MAX_USERS; i++) {
        scores[i].username[0] = '\0';
        scores[i].total_value = 0;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            if (strncmp(entry->d_name, "treasure", 8) == 0) {
                char filepath[300];
                snprintf(filepath, sizeof(filepath), "%s/%s", hunt_path, entry->d_name);

                int fd = open(filepath, O_RDONLY);
                if (fd == -1) {
                    char warn[256];
                    snprintf(warn, sizeof(warn), "Warning: cannot open file %s\n", filepath);
                    write(STDERR_FILENO, warn, strlen(warn));
                    continue;
                }

                char line[MAX_LINE_LEN];
                ssize_t len = read_line(fd, line, sizeof(line));
                close(fd);

                if (len <= 0) continue;

                // Parsam linia:
                // format: id username latitude longitude clue value
                int id, value;
                char username[MAX_USERNAME_LEN];
                double lat, lon;
                char clue[64];

                // Note: clue fara spatii in acest simplu parsing
                int n = sscanf(line, "%d %31s %lf %lf %63s %d", &id, username, &lat, &lon, clue, &value);
                if (n == 6) {
                    int idx = find_user(scores, user_count, username);
                    if (idx == -1) {
                        if (user_count < MAX_USERS) {
                            strcpy(scores[user_count].username, username);
                            scores[user_count].total_value = value;
                            user_count++;
                        }
                    } else {
                        scores[idx].total_value += value;
                    }
                }
            }
        }
    }

    closedir(dir);

    char outbuf[512];
    int outlen = snprintf(outbuf, sizeof(outbuf), "Scores for hunt %s:\n", hunt_id);
    write(STDOUT_FILENO, outbuf, outlen);
    for (int i = 0; i < user_count; i++) {
        outlen = snprintf(outbuf, sizeof(outbuf), "%s: %d\n", scores[i].username, scores[i].total_value);
        write(STDOUT_FILENO, outbuf, outlen);
    }

    return 0;
}
