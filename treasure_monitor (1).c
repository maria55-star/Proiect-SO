#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include "treasure.h"

volatile sig_atomic_t ready = 0;

// functia care trateaza semnalele
void handle_signal(int sig) {
    if (sig == SIGUSR1)
        ready = 1;
}

void list_hunts() {
    DIR *dir = opendir(".");
    if (!dir) {
        printf("Can not open current directory\n");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char path[256];
            snprintf(path, sizeof(path), "%s/treasures.dat", entry->d_name);

            struct stat st;
            if (stat(path, &st) == 0) {
                int count = st.st_size / sizeof(Treasure);
                printf("Hunt: %s | Comori: %d\n", entry->d_name, count);
            }
        }
    }
    closedir(dir);
}

// functia principala care va rula monitorul
void process() {
    char buffer[256];

    int fd = open("command.txt", O_RDONLY);
    if (fd < 0) {
        printf("Can not open command file\n");
        return;
    }

    ssize_t rd = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);

    if (rd < 0) {
        printf("Error at read\n");
        return;
    }

    buffer[rd] = '\0';

    if (strcmp(buffer, "stop") == 0) {
        printf("Stop, exit in 2s...\n");
	usleep(2000000);
	exit(0);
    } else if (strcmp(buffer, "list_hunts") == 0) {
        list_hunts();
    } else if (strncmp(buffer, "list_treasures ", 15) == 0) {
        char hunt_id[32];
	sscanf(buffer, "list_treasures %s", hunt_id);

	pid_t pid;
	if ((pid = fork()) == 0) {
	    execl("./main", "./main", "--list", hunt_id, NULL);
	    printf("execl main a esuat\n");
	    exit(1);
	} else {
	    waitpid(pid, NULL, 0);
	}
    } else if (strncmp(buffer, "view_treasure ", 14) == 0) {
        char hunt_id[32];
        char treasure_id[32];
      
        if (sscanf(buffer + 14, "%s %s", hunt_id, treasure_id) == 2) {
	    pid_t pid;
	    if ((pid = fork()) == 0) {
	        execl("./main", "./main", "--view", hunt_id, treasure_id, NULL);
		printf("execl main a esuat\n");
		exit(1);
	    } else {
	        waitpid(pid, NULL, 0);
	    }
	} else {
	    printf("Invalid format. Use: view_treasure hunt_id treasure_id\n");
	}
    } else {
        printf("Unknown command: %s\n", buffer);
    }
}

int main() {
    struct sigaction sa;
    // configuram semnalul
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1, &sa, NULL) < 0) {
        printf("sigaction error\n");
        exit(1);
    }

    // asteapta semnale pentru a le procesa
    printf("Monitorul a fost lansat. Aștept semnale...\n");

    while (1) {
        pause(); // asteapta semnale

	if (ready) {
	    process();
	    ready = 0;
	}
    }

    return 0;
}
