#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>//header-ul necesar pentru semnale
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

pid_t monitor_pid = -1;
int monitor_running = 0;

void sigchld_handler(int signo) {
    int status;
    pid_t pid;
    if ((pid = wait(&status)) == monitor_pid) {
        printf("Monitor exit with status %d\n", WEXITSTATUS(status));
	monitor_running = 0;
    }
}

void send_command(const char* cmd) {
    int fd = open("command.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, cmd, strlen(cmd));
        close(fd);
    }
    kill(monitor_pid, SIGUSR1);
}

void start_monitor() {
    if (monitor_running) {
        printf("Monitor running\n");
	return;
    }

    if ((monitor_pid = fork()) == 0) {
        // procesul copil va lansa monitorul
        execl("./treasure_monitor", "treasure_monitor", NULL);
        perror("Eroare la lansarea monitorului");
        exit(1);
    }
    else if (monitor_pid < 0) {
        perror("Eroare la crearea procesului monitor");
    } else {
        monitor_running = 1;
        printf("Monitorul a fost lansat cu PID %d.\n", monitor_pid);
    }
}

void stop_monitor() {
    if (!monitor_running) {
        printf("Monitorul nu este in executie\n");
	return;
    }

    send_command("stop");
    printf("Monitorul va fi oprit.\n");
}

void list_hunts() {
    if (!monitor_running) {
        printf("Monitorul nu este in executie\n");
	return;
    }

    send_command("list_hunts");
    printf("Se listeaza hunt-urile...\n");
}

void list_treasures(char command[]) {
    if (!monitor_running) {
        printf("Monitorul nu este in executie\n");
	return;
    }

    char *hunt_id = command + 15; // tot ce urmeaza dupa "list_treasures "
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "list_treasures %s", hunt_id);
    send_command(cmd);
}

void view_treasure(char command[]) {
    if (!monitor_running) {
      printf("Monitorul nu este in executie\n");
      return;
    }

    char cmd[256];
    snprintf(cmd, sizeof(cmd), "%s", command);
    send_command(cmd);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) < 0)
      return 1;
  
    char command[100];

    while (1) {
        printf("Comenzi disponibile:\n");
        printf("  start_monitor - incepe monitorul\n");
        printf("  stop_monitor - opreste monitorul\n");
        printf("  list_hunts    - listeaza hunt-urile\n");
        printf("  list_treasures- listeaza comorile\n");
        printf("  view_treasure  - vizualizeaza o comoara\n");
        printf("  exit          - paraseste programul\n");
        printf("\nhub: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;  // elimina newline-ul

        if (strcmp(command, "start_monitor") == 0) {
            start_monitor();
        } else if (strcmp(command, "stop_monitor") == 0) {
            stop_monitor();
        } else if (strcmp(command, "list_hunts") == 0) {
            list_hunts();
        } else if (strncmp(command, "list_treasures ", 15) == 0) {
            list_treasures(command);
        } else if (strncmp(command, "view_treasure ", 14) == 0) {
	    view_treasure(command);
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
