/* Jay Moon
 * 110983806
 * myungsuk.moon@stonybrook.edu
 * CSE 376 HW4
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MSG_SIZE 8192

int main(int argc, char **argv, char **envp) {
    // Initialize variables
    struct sockaddr_un sock_addr;
    int sock_fd = -1;
    char cmd_type[3] = "j";
    char cmd_pid[32] = "-1";
    char cmd_value[64] = "-1";
    char *msg_in = NULL;
    char *msg_out = NULL;
    int argv_pos = 1;
    int envp_pos = 0;

    // Check if there are more than one argument
    if (2 > argc) {
        fprintf(stderr, "%s: at least one argument is needed\n", argv[0]);
        exit(EINVAL);
    }

    // If the options were given
    if ('-' == argv[1][0]) {
        if (3 == argc) {
            if (strcmp(argv[1], "-l") == 0 || strcmp(argv[1], "--list") == 0) {
                cmd_type[0] = 'l';
            }
            else if (strcmp(argv[1], "-o") == 0 || strcmp(argv[1], "--output") == 0) {
                cmd_type[0] = 'o';
            }
            else if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--status") == 0) {
                cmd_type[0] = 's';
            }
            else if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "--exitcode") == 0) {
                cmd_type[0] = 'e';
            }
            else if (strcmp(argv[1], "-k") == 0 || strcmp(argv[1], "--kill") == 0) {
                cmd_type[0] = 'k';
            }
            else if (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "--terminate") == 0) {
                cmd_type[0] = 'r';
            }
            else if (strcmp(argv[1], "-z") == 0 || strcmp(argv[1], "--suspend") == 0) {
                cmd_type[0] = 'z';
            }
            else if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--continue") == 0) {
                cmd_type[0] = 'c';
            }
            else {
                fprintf(stderr, "%s: invalid option\n", argv[0]);
                exit(EINVAL);
            }
            strcpy(cmd_pid, argv[2]);
            argv_pos = argv_pos + 2;
        }
        else if (3 < argc) {
            if (5 == argc && (strcmp(argv[1], "-u") == 0 || strcmp(argv[1], "--update")) == 0) {
                if (strcmp(argv[3], "-n") == 0 || strcmp(argv[3], "--nice") == 0) {
                    cmd_type[0] = 'u';
                    cmd_type[1] = 'n';
                }
                else {
                    fprintf(stderr, "%s: invalid option\n", argv[0]);
                    exit(EINVAL);
                }
                strcpy(cmd_pid, argv[2]);
                strcpy(cmd_value, argv[4]);
                argv_pos = argv_pos + 2;
            }
            else if (strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--time") == 0) {
                cmd_type[1] = 't';
                strcpy(cmd_value, argv[2]);
            }
            else if (strcmp(argv[1], "-m") == 0 || strcmp(argv[1], "--memory") == 0) {
                cmd_type[1] = 'm';
                strcpy(cmd_value, argv[2]);
            }
            else if (strcmp(argv[1], "-n") == 0 || strcmp(argv[1], "--nice") == 0) {
                cmd_type[1] = 'n';
                strcpy(cmd_value, argv[2]);
            }
            else {
                fprintf(stderr, "%s: invalid option\n", argv[0]);
                exit(EINVAL);
            }
            argv_pos = argv_pos + 2;
        }
        else {
            fprintf(stderr, "%s: invalid option\n", argv[0]);
            exit(EINVAL);
        }
    }

    // Create a socket and get its file descriptor
    sock_fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (0 > sock_fd) {
        perror("socket");
        exit(errno);
    }

    // Assign values to the sock_addr struct
    sock_addr.sun_family = AF_LOCAL;
    strcpy(sock_addr.sun_path, "/tmp/mymoon.sock");

    // Connect to a socket with its file descriptor
    if (connect(sock_fd, (struct sockaddr *) &sock_addr, sizeof(struct sockaddr_un)) != 0) {
        perror("connect");
        exit(errno);
    }

    // Allocate a memory for msg_in
    msg_in = malloc(MSG_SIZE);
    if (NULL == msg_in) {
        perror("malloc");
        exit(errno);
    }

    // Build a message
    strcat(msg_in, cmd_type);
    strcat(msg_in, "\n");
    strcat(msg_in, cmd_pid);
    strcat(msg_in, "\n");
    strcat(msg_in, cmd_value);
    strcat(msg_in, "\n");
    while (argv_pos < argc) {
        strcat(msg_in, argv[argv_pos]);
        strcat(msg_in, "\t");
        argv_pos++;
    }
    strcat(msg_in, "\n");
    while (NULL != envp[envp_pos]) {
        strcat(msg_in, envp[envp_pos]);
        strcat(msg_in, "\t");
        envp_pos++;
    }

    // Allocate a memory for msg_out
    msg_out = malloc(MSG_SIZE);
    if (NULL == msg_out) {
        perror("malloc");
        exit(errno);
    }

    // Send a message on a socket
    if (send(sock_fd, msg_in, strlen(msg_in) + 1, 0) < 0) {
        perror("send");
        exit(errno);
    }

    // Receive a message from a socket
    if (recv(sock_fd, msg_out, MSG_SIZE, 0) >= 0) {
        fprintf(stdout, "%s\n", msg_out);
    }

    // Close a socket's file descriptor
    close(sock_fd);

    exit(0);
}