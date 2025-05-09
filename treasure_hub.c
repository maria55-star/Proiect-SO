#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

pid_t monitor_pid = -1;

void start_monitor() {
    monitor_pid = fork();

    if (monitor_pid == 0) {
        // procesul copil va lansa monitorul
        execl("./treasure_monitor", "treasure_monitor", NULL);
        perror("Eroare la lansarea monitorului");
        exit(1);
    }
    else if (monitor_pid < 0) {
        perror("Eroare la crearea procesului monitor");
    } else {
        printf("Monitorul a fost lansat cu PID %d.\n", monitor_pid);
    }
}

void stop_monitor() {
    if (monitor_pid != -1) {
        kill(monitor_pid, SIGTERM);  // Trimitem semnalul SIGTERM pentru a opri monitorul
        printf("Monitorul va fi oprit.\n");
    } else {
        printf("Monitorul nu este în execuție.\n");
    }
}

void list_hunts() {
    if (monitor_pid != -1) {
        kill(monitor_pid, SIGUSR1);  // trimitem semnalul SIGUSR1 pentru a lista hunt-urile
        printf("Se listă hunt-urile...\n");
    } else {
        printf("Monitorul nu este în executie.\n");
    }
}

void list_treasures() {
    if (monitor_pid != -1) {
        kill(monitor_pid, SIGUSR2);  // trimitem semnalul SIGUSR2 pentru a lista comorile
        printf("Se listeaza comorile...\n");
    } else {
        printf("Monitorul nu este în executie.\n");
    }
}

int main() {
    char command[100];

    while (1) {
        printf("Comenzi disponibile:\n");
        printf("  start_monitor - incepe monitorul\n");
        printf("  stop_monitor - opreste monitorul\n");
        printf("  list_hunts    - listeaza hunt-urile\n");
        printf("  list_treasures- listeaza comorile\n");
        printf("  exit          - paraseste programul\n");
        printf("Introduceți comanda: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;  // elimina newline-ul

        if (strcmp(command, "start_monitor") == 0) {
            start_monitor();
        } else if (strcmp(command, "stop_monitor") == 0) {
            stop_monitor();
        } else if (strcmp(command, "list_hunts") == 0) {
            list_hunts();
        } else if (strcmp(command, "list_treasures") == 0) {
            list_treasures();
        } else if (strcmp(command, "exit") == 0) {
            if (monitor_pid != -1) {
                stop_monitor();
            }
            break;
        } else {
            printf("comanda invalida!\n");
        }
    }

    return 0;
}
