#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "treasure.h" 


extern void list_treasures(const char *hunt_id);
extern void view_treasure(const char *hunt_id, int treasure_id);
extern void remove_treasure(const char *hunt_id, int treasure_id);

// functia care trateaza semnalele
void handle_signal(int sig) {
    switch (sig) {
        case SIGINT:
            printf("\nMonitorul a primit semnalul SIGINT. Opriți monitorul.\n");
        
            break;
        case SIGUSR1:
            printf("\nMonitorul a primit semnalul SIGUSR1. Listarea comorilor.\n");
            list_treasures("example_hunt"); // Exemplu: listarea comorilor pentru un anumit hunt_id
            break;
        case SIGUSR2:
            printf("\nMonitorul a primit semnalul SIGUSR2. Vizualizarea unei comori.\n");
            view_treasure("example_hunt", 1); // Exemplu: vizualizarea unei comori cu ID-ul 1
            break;
        default:
            printf("\nSemnal necunoscut primit.\n");
            break;
    }
}

// functie pentru a inregistra semnalele
void setup_signal_handlers() {
    if (signal(SIGINT, handle_signal) == SIG_ERR) {
        perror("Eroare la inregistrarea semnalului SIGINT");
        exit(1);
    }
    if (signal(SIGUSR1, handle_signal) == SIG_ERR) {
        perror("Eroare la inregistrarea semnalului SIGUSR1");
        exit(1);
    }
    if (signal(SIGUSR2, handle_signal) == SIG_ERR) {
        perror("Eroare la inregistrarea semnalului SIGUSR2");
        exit(1);
    }
}

// functia principala care va rula monitorul
int main() {
    // configuram semnalele
    setup_signal_handlers();

    // asteapta semnale pentru a le procesa
    printf("Monitorul a fost lansat. Aștept semnale...\n");

    while (1) {
        pause(); // asteapta semnale
    }

    return 0;
}
