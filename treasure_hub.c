#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdbool.h>

#define CMD_FILE "/tmp/monitor_cmd.txt"

pid_t monitor_pid = -1;
bool monitor_running = false;

int pipefd[2];

void read_monitor_output() {
    char buffer[1024];
    ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);

    if (n > 0) {
        buffer[n] = '\0';
        printf("%s", buffer);
    } else if (n == 0) {
    } else {
        perror("read");
    }
}

void handle_sigchld(int sig) {
    int status;
    waitpid(monitor_pid, &status, 0);
    printf("Monitor process terminated with status %d\n", status);
    monitor_running = false;
}

void write_command(const char *cmd) {
    FILE *fp = fopen(CMD_FILE, "w");
    if (!fp) {
        perror("Failed to write command file");
        return;
    }
    fprintf(fp, "%s\n", cmd);
    fclose(fp);
    kill(monitor_pid, SIGUSR1);
}

int main() {
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);

    char input[256];
    printf("Treasure Hub functions:\nstart_monitor\nlist_hunts\nlist_treasures <hunt_id>\nview_treasure <hunt_id> <treasure_id>\ncalculate_score <hunt_id>\nstop_monitor\nexit\n\n");

    while (1) {
        printf("> ");
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = '\0';

        if (strncmp(input, "start_monitor", 13) == 0) {
            if (monitor_running) {
                printf("Monitor already running.\n");
                continue;
            }

            monitor_pid = fork();
            if (monitor_pid == 0) {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
                execl("./monitor", "monitor", NULL);
                perror("Failed to start monitor");
                exit(1);
            }

            monitor_running = true;
            close(pipefd[1]);
            printf("Monitor started (PID: %d)\n", monitor_pid);
        }

        else if (strncmp(input, "list_hunts", 10) == 0 ||
                 strncmp(input, "list_treasures", 14) == 0 ||
                 strncmp(input, "view_treasure", 13) == 0 ||
                 strncmp(input, "calculate_score", 15) == 0) {
            if (!monitor_running) {
                printf("Monitor not running.\n");
                continue;
            }
            write_command(input);
            sleep(1); 
            read_monitor_output();
        }

        else if (strncmp(input, "stop_monitor", 12) == 0) {
            if (!monitor_running) {
                printf("Monitor not running.\n");
                continue;
            }
            kill(monitor_pid, SIGUSR2);
            printf("Requested monitor to stop. Waiting for it to exit...\n");
        }

        else if (strncmp(input, "exit", 4) == 0) {
            if (monitor_running) {
                printf("Cannot exit. Monitor is still running.\n");
            } else {
                break;
            }
        }

        else {
            printf("Unknown command.\n");
        }
    }

    return 0;
}
