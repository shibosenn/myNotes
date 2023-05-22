#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>

#define SEM_NAME "/test_sem"
#define COUNTER_FILE "counter.txt"

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void run_child(int semid) {
    struct sembuf sops;

    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;

    for (int i = 0; i < 10; i++) {
        if (semop(semid, &sops, 1) == -1) {
            perror("semop");
            exit(EXIT_FAILURE);
        }

        FILE *fp = fopen(COUNTER_FILE, "r+");
        if (fp == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        int counter;
        if (fscanf(fp, "%d", &counter) != 1) {
            perror("fscanf");
            exit(EXIT_FAILURE);
        }

        printf("[Child] Read counter: %d\n", counter);

        counter++;

        fseek(fp, 0, SEEK_SET);
        fprintf(fp, "%d", counter);

        fclose(fp);

        sops.sem_op = 1;
        if (semop(semid, &sops, 1) == -1) {
            perror("semop");
            exit(EXIT_FAILURE);
        }

        printf("[Child] Incremented counter: %d\n", counter);
    }
}

void run_parent(int semid) {
    struct sembuf sops;

    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;

    for (int i = 0; i < 10; i++) {
        if (semop(semid, &sops, 1) == -1) {
            perror("semop");
            exit(EXIT_FAILURE);
        }

        FILE *fp = fopen(COUNTER_FILE, "r+");
        if (fp == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        int counter;
        if (fscanf(fp, "%d", &counter) != 1) {
            perror("fscanf");
            exit(EXIT_FAILURE);
        }

        printf("[Parent] Read counter: %d\n", counter);

        counter--;

        fseek(fp, 0, SEEK_SET);
        fprintf(fp, "%d", counter);

        fclose(fp);

        sops.sem_op = 1;
        if (semop(semid, &sops, 1) == -1) {
            perror("semop");
            exit(EXIT_FAILURE);
        }

        printf("[Parent] Decremented counter: %d\n", counter);
    }
}

int main() {
    key_t key;
    int semid, counter_fd;

    if ((key = ftok(SEM_NAME, 1)) == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    union semun init_value;
    init_value.val = 1;

    if (semctl(semid, 0, SETVAL, init_value) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    if ((counter_fd = creat(COUNTER_FILE, 0666)) == -1) {
        perror("creat");
        exit(EXIT_FAILURE);
    }

    if (close(counter_fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    if ((pid = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        run_child(semid);
    } else {
        run_parent(semid);
    }

    if (semctl(semid, 0, IPC_RMID, 0) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    if (remove(COUNTER_FILE) == -1) {
        perror("remove");
        exit(EXIT_FAILURE);
    }

    return 0;
}
