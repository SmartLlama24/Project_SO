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
    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);

    char input[256];
    printf("Treasure Hub functions:\nstart_monitor\nlist_hunts\nlist_treasures <hunt_id>\nview_treasure <hunt_id> <treasure_id>\nstop_monitor\nexit\n\n");

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
                execl("./monitor", "monitor", NULL);
                perror("Failed to start monitor");
                exit(1);
            }
            monitor_running = true;
            printf("Monitor started (PID: %d)\n", monitor_pid);
        }

        else if (strncmp(input, "list_hunts", 10) == 0) {
            if (!monitor_running) { printf("Monitor not running.\n"); continue; }
            write_command("list_hunts");
        }

        else if (strncmp(input, "list_treasures", 14) == 0) {
            if (!monitor_running) { printf("Monitor not running.\n"); continue; }
            write_command(input);
        }

        else if (strncmp(input, "view_treasure", 13) == 0) {
            if (!monitor_running) { printf("Monitor not running.\n"); continue; }
            write_command(input);
        }

        else if (strncmp(input, "stop_monitor", 12) == 0) {
            if (!monitor_running) { printf("Monitor not running.\n"); continue; }
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
