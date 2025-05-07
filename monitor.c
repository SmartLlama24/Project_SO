#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define CMD_FILE "/tmp/monitor_cmd.txt"

volatile sig_atomic_t got_command = 0;
volatile sig_atomic_t stop_requested = 0;

void handle_sigusr1(int sig) {
    got_command = 1;
}

void handle_sigusr2(int sig) {
    stop_requested = 1;
}

void setup_signals() {
    struct sigaction sa1, sa2;
    sa1.sa_handler = handle_sigusr1;
    sigemptyset(&sa1.sa_mask);
    sa1.sa_flags = 0;
    sigaction(SIGUSR1, &sa1, NULL);

    sa2.sa_handler = handle_sigusr2;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;
    sigaction(SIGUSR2, &sa2, NULL);
}

void process_command(const char *cmd) {
    printf("[Monitor] Processing: %s\n", cmd);

    if (strncmp(cmd, "list_hunts", 10) == 0) {
        system(
            "for dir in Hunts/* ; do "
            "if [ -d \"$dir\" ]; then "
            "hunt_id=$(basename \"$dir\"); "
            "count=$(./treasure_manager --list \"$hunt_id\" 2>/dev/null | grep -c 'Treasure ID'); "
            "echo \"$hunt_id: $count treasures\"; "
            "fi; "
            "done"
        );
    }

    else if (strncmp(cmd, "list_treasures", 14) == 0) {
        char hunt_id[100];
        sscanf(cmd, "list_treasures %s", hunt_id);
        char syscmd[256];
        snprintf(syscmd, sizeof(syscmd), "./treasure_manager --list %s", hunt_id);
        system(syscmd);
    }

    else if (strncmp(cmd, "view_treasure", 13) == 0) {
        char hunt_id[100], treasure_id[100];
        sscanf(cmd, "view_treasure %s %s", hunt_id, treasure_id);
        char syscmd[256];
        snprintf(syscmd, sizeof(syscmd), "./treasure_manager --view %s %s", hunt_id, treasure_id);
        system(syscmd);
    }

    else {
        printf("[Monitor] Unknown command.\n");
    }
}

int main() {
    setup_signals();
    printf("[Monitor] Running (PID: %d)\n", getpid());

    while (1) {
        pause();

        if (stop_requested) {
            printf("[Monitor] Stopping in 3 seconds...\n");
            usleep(3000000);
            break;
        }

        if (got_command) {
            got_command = 0;

            FILE *fp = fopen(CMD_FILE, "r");
            if (!fp) {
                perror("[Monitor] Failed to read command file");
                continue;
            }

            char command[256];
            if (fgets(command, sizeof(command), fp)) {
                command[strcspn(command, "\n")] = '\0';
                process_command(command);
            }
            fclose(fp);
        }
    }

    printf("[Monitor] Terminated.\n");
    return 0;
}
