#include "treasure.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h> // Adăugat header-ul necesar pentru semnale

void start_monitor() {
    pid_t pid = fork();
    if (pid == 0) {
        // Codul procesului monitor
        execl("./treasure_monitor", "treasure_monitor", NULL);
    }
    else if (pid < 0) {
        perror("Eroare la crearea procesului monitor");
    }
}

void stop_monitor(pid_t monitor_pid) {
    kill(monitor_pid, SIGINT);  // SIGINT este acum definit corect
}

int main() {
    char command[256];
    pid_t monitor_pid = -1;

    while (1) {
        printf("Introduceți o comandă (start_monitor, list_hunts, exit): ");
        fgets(command, sizeof(command), stdin);

        if (strncmp(command, "start_monitor", 13) == 0) {
            start_monitor();
        } 
        else if (strncmp(command, "stop_monitor", 12) == 0) {
            if (monitor_pid != -1) {
                stop_monitor(monitor_pid);
            } else {
                printf("Monitorul nu este în execuție.\n");
            }
        } 
        else if (strncmp(command, "exit", 4) == 0) {
            break;
        } 
        else {
            printf("Comandă necunoscută.\n");
        }
    }

    return 0;
}
