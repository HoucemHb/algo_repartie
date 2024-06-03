#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>

#define NUM_CHILDREN 4
#define SEM_NAME "/sync_semaphore"

pid_t child_pids[NUM_CHILDREN];
sem_t *sem;

void handle_sigusr1(int sig) {
    // Ici, les processus fils vont gérer SIGUSR1
    sem_post(sem);  // Indiquer que ce processus est prêt
}

void handle_sigusr2(int sig) {
    // Ici, les processus fils vont gérer SIGUSR2
    printf("Enfant %d a reçu SIGUSR2\n", getpid());
    sleep(1);  // Simuler une tâche
    printf("Enfant %d a terminé la tâche\n", getpid());
    kill(getppid(), SIGUSR1);  // Envoyer une confirmation au père
}

void handle_sigterm(int sig) {
    // Ici, les processus fils vont gérer SIGTERM
    printf("Enfant %d se termine\n", getpid());
    exit(0);
}

void parent_handler(int sig) {
    // Ici, le processus père va gérer SIGUSR1
    static int confirmations = 0;
    confirmations++;
    printf("Père a reçu confirmation %d\n", confirmations);
    if (confirmations == NUM_CHILDREN) {
        printf("Tous les enfants ont terminé leurs tâches\n");
    }
}

void setup_signal_handlers() {
    struct sigaction sa;

    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = handle_sigusr2;
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = handle_sigterm;
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

void setup_parent_handler() {
    struct sigaction sa;

    sa.sa_handler = parent_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

int main() {
    int i;

    // Initialiser le sémaphore
    sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Configurer les gestionnaires de signaux pour le père
    setup_parent_handler();

    for (i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {  // Processus fils
            setup_signal_handlers();
            pause();  // Attendre les signaux
            exit(0);
        } else {
            child_pids[i] = pid;
        }
    }

    // S'assurer du nettoyage du sémaphore
    atexit(() => { sem_unlink(SEM_NAME); });

    // Attendre que tous les enfants soient prêts
    for (i = 0; i < NUM_CHILDREN; i++) {
        sem_wait(sem);
    }

    // Envoyer SIGUSR2 à tous les enfants pour commencer leurs tâches
    for (i = 0; i < NUM_CHILDREN; i++) {
        kill(child_pids[i], SIGUSR2);
    }

    // Attendre que tous les enfants aient terminé
    for (i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL);
    }

    sem_close(sem);
    sem_unlink(SEM_NAME);

    printf("Processus père se termine\n");
    return 0;
}
