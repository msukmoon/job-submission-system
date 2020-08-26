/* Jay Moon
 * 110983806
 * myungsuk.moon@stonybrook.edu
 * CSE 376 HW4
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <getopt.h>
#include <sys/queue.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MSG_SIZE 8192

// Head of the list
struct ListHead *head_ptr;

// Structure that maintains information about a job
struct Job {
    pid_t pid;
    char prog_name[64];
    int fd[2];
    int acpt_sock_fd;
    char status[32];
    int exit_code;
    rlim_t time_lim;
    rlim_t mem_lim;
    int nice_val;
    char *output;
    char **argv;
    char **envp;
    LIST_ENTRY(Job) entries;
};

// Initialize head of the list
LIST_HEAD(ListHead, Job) head = LIST_HEAD_INITIALIZER(head);

void handle_command(char *type, char *pid, char *val, char **output) {
    // Initialize variables
    struct Job *curr_job;
    int parsed_pid = 0;
    long int parsed_val = 0;

    // Handle "list" command
    if ('l' == type[0]) {
        if (strcmp(pid, "all") == 0) {
            snprintf(*output, MSG_SIZE, "PID\tPNAME\tSTATUS\tETCODE\tNICEVL\tTMELIM\tMEMLIM\n");
            LIST_FOREACH(curr_job, &head, entries) {
                snprintf(*output + strlen(*output), MSG_SIZE, "%d\t%s\t%s\t%d\t%d\t%ld\t%ld\n", 
                            curr_job->pid, curr_job->prog_name, curr_job->status,
                            curr_job->exit_code, curr_job->nice_val, curr_job->time_lim,
                            curr_job->mem_lim);
            }
        }
        else {
            parsed_pid = strtol(pid, NULL, 10);
            if (LONG_MIN == parsed_pid || LONG_MAX == parsed_pid) {
                perror("strtol");
                // Do not exit from the program
                return;
            }
            LIST_FOREACH(curr_job, &head, entries) {                
                if (parsed_pid == curr_job->pid) {
                    snprintf(*output, MSG_SIZE, "PID\tPNAME\tSTATUS\tETCODE\tNICEVL\tTMELIM\tMEMLIM\n");
                    snprintf(*output + strlen(*output), MSG_SIZE, "%d\t%s\t%s\t%d\t%d\t%ld\t%ld\n", 
                                curr_job->pid, curr_job->prog_name, curr_job->status,
                                curr_job->exit_code, curr_job->nice_val, curr_job->time_lim,
                                curr_job->mem_lim);
                    break;
                }
            }
        }
    }
    // Handle "output" command
    else if ('o' == type[0]) {
        parsed_pid = strtol(pid, NULL, 10);
        if (LONG_MIN == parsed_pid || LONG_MAX == parsed_pid) {
            perror("strtol");
            // Do not exit from the program
            return;
        }
        LIST_FOREACH(curr_job, &head, entries) {                
            if (parsed_pid == curr_job->pid) {
                strcat(*output, curr_job->output);
                break;
            }
        }
    }
    // Handle "status" command
    else if ('s' == type[0]) {
        parsed_pid = strtol(pid, NULL, 10);
        if (LONG_MIN == parsed_pid || LONG_MAX == parsed_pid) {
            perror("strtol");
            // Do not exit from the program
            return;
        }
        LIST_FOREACH(curr_job, &head, entries) {                
            if (parsed_pid == curr_job->pid) {
                snprintf(*output, MSG_SIZE, "%s\n", curr_job->status);
                break;
            }
        }
    }
    // Handle "exitcode" command
    else if ('e' == type[0]) {
        parsed_pid = strtol(pid, NULL, 10);
        if (LONG_MIN == parsed_pid || LONG_MAX == parsed_pid) {
            perror("strtol");
            // Do not exit from the program
            return;
        }
        LIST_FOREACH(curr_job, &head, entries) {                
            if (parsed_pid == curr_job->pid) {
                snprintf(*output, MSG_SIZE, "%d\n", curr_job->exit_code);
                break;
            }
        }
    }
    // Handle "nice" command
    else if (strcmp(type, "un") == 0) {
        parsed_pid = strtol(pid, NULL, 10);
        if (LONG_MIN == parsed_pid || LONG_MAX == parsed_pid) {
            perror("strtol");
            // Do not exit from the program
            return;
        }
        parsed_val = strtol(val, NULL, 10);
        if (LONG_MIN == parsed_pid || LONG_MAX == parsed_pid) {
            perror("strtol");
            // Do not exit from the program
            return;
        }
        LIST_FOREACH(curr_job, &head, entries) {                
            if (parsed_pid == curr_job->pid) {   
                if (setpriority(PRIO_PROCESS, parsed_pid, parsed_val) == -1) {
                    snprintf(*output, MSG_SIZE, 
                                "The nice value for the process %d could not be updated to %ld\n",
                                curr_job->pid, parsed_val);
                }
                else {
                    errno = 0;
                    curr_job->nice_val = getpriority(PRIO_PROCESS, parsed_pid);
                    if (-1 == curr_job->nice_val && 0 != errno) {
                        perror("getpriority");
                        exit(errno);
                    }
                    snprintf(*output, MSG_SIZE, "The nice value for the process %d updated to %d\n",
                                curr_job->pid, curr_job->nice_val);
                }
                break;
            }
        }
    }
    // Handle "kill" command
    else if ('k' == type[0]) {
        parsed_pid = strtol(pid, NULL, 10);
        if (LONG_MIN == parsed_pid || LONG_MAX == parsed_pid) {
            perror("strtol");
            // Do not exit from the program
            return;
        }
        LIST_FOREACH(curr_job, &head, entries) {                
            if (parsed_pid == curr_job->pid) {
                if (-1 == kill(parsed_pid, SIGKILL)) {
                    snprintf(*output, MSG_SIZE, "The process %d has failed to be killed\n", curr_job->pid);
                }
                else {
                    strcpy(curr_job->status, "KILLED");
                    curr_job->exit_code = 128 + SIGKILL;
                    snprintf(*output, MSG_SIZE, "The process %d has been killed successfully\n", curr_job->pid);
                }
                break;
            }
        }
    }
    // Handle "terminate" command
    else if ('r' == type[0]) {
        parsed_pid = strtol(pid, NULL, 10);
        if (LONG_MIN == parsed_pid || LONG_MAX == parsed_pid) {
            perror("strtol");
            // Do not exit from the program
            return;
        }
        LIST_FOREACH(curr_job, &head, entries) {                
            if (parsed_pid == curr_job->pid) {
                if (-1 == kill(parsed_pid, SIGTERM)) {
                    snprintf(*output, MSG_SIZE, "The process %d has failed to terminate\n", curr_job->pid);
                }
                else {
                    strcpy(curr_job->status, "TERMNED");
                    curr_job->exit_code = 128 + SIGTERM;
                    snprintf(*output, MSG_SIZE, "The process %d has been terminated successfully\n", curr_job->pid);
                }
                break;
            }
        }
    }
    // Handle "suspend" command
    else if ('z' == type[0]) {
        parsed_pid = strtol(pid, NULL, 10);
        if (LONG_MIN == parsed_pid || LONG_MAX == parsed_pid) {
            perror("strtol");
            // Do not exit from the program
            return;
        }
        LIST_FOREACH(curr_job, &head, entries) {                
            if (parsed_pid == curr_job->pid) {
                if (-1 == kill(parsed_pid, SIGSTOP)) {
                    snprintf(*output, MSG_SIZE, "The process %d has failed to suspend\n", curr_job->pid);
                }
                else {
                    strcpy(curr_job->status, "SUSPEND");
                    snprintf(*output, MSG_SIZE, "The process %d has been suspended successfully\n", curr_job->pid);
                }
                break;
            }
        }
    }
    // Handle "continue" command
    else if ('c' == type[0]) {
        parsed_pid = strtol(pid, NULL, 10);
        if (LONG_MIN == parsed_pid || LONG_MAX == parsed_pid) {
            perror("strtol");
            // Do not exit from the program
            return;
        }
        LIST_FOREACH(curr_job, &head, entries) {                
            if (parsed_pid == curr_job->pid) {
                if (-1 == kill(parsed_pid, SIGCONT)) {
                    snprintf(*output, MSG_SIZE, "The process %d has failed to continue\n", curr_job->pid);
                }
                else {
                    strcpy(curr_job->status, "RUNNING");
                    snprintf(*output, MSG_SIZE, "The process %d has been continued successfully\n", curr_job->pid);
                }
                break;
            }
        }
    }
    // Handle invalid commands
    else {
        fprintf(stderr, "server: received invalid option(s)\n");
        // Do not exit from the program
        return;
    }
}

void execute_job(struct Job *job) {
    // Initialize variables
    struct rlimit time_lims;
    struct rlimit mem_lims;

    // Create a pipe
    if (pipe(job->fd) != 0) {
        perror("pipe");
        exit(errno);
    }

    // Fork to create a child process
    job->pid = fork();
    if (-1 == job->pid) {
        perror("fork");
        exit(errno);
    }

    // If this process is a child
    if (0 == job->pid) {
        // Close a child's read end pipe
        if (0 != close(job->fd[0])) {
            perror("close");
            exit(errno);
        }

        // Duplicate stdout to fd[1]
        if (dup2(job->fd[1], 1) < 0) {
            perror("dup2");
            exit(errno);
        }

        // Duplicate stderr to fd[1]
        if (dup2(job->fd[1], 2) < 0) {
            perror("dup2");
            exit(errno);
        }

        // Close a child's write end pipe
        if (close(job->fd[1]) != 0) {
            perror("close");
            exit(errno);
        }

        // Set time limit
        time_lims.rlim_cur = job->time_lim;
        time_lims.rlim_max = job->time_lim;
        if (setrlimit(RLIMIT_CPU, &time_lims) != 0) {
            perror("setrlimit");
            exit(errno);
        }

        // Set memory limit
        mem_lims.rlim_cur = job->mem_lim;
        mem_lims.rlim_max = job->mem_lim;
        if (setrlimit(RLIMIT_STACK, &mem_lims) != 0) {
            perror("setrlimit");
            exit(errno);
        }

        // Set nice value
        errno = 0;
        job->nice_val = nice(job->nice_val);
        if (-1 == job->nice_val && 0 != errno) {
            perror("nice");
            exit(errno);
        }

        // Execute a command
        if (execvpe(job->argv[0], job->argv, job->envp) == -1) {
            // Ignore the ENOENT error
            if (ENOENT != errno) {
                perror("execvpe");
                exit(errno);
            }
        }
        exit(errno);
    }
    // If this process is a parent
    else {
        // Close a parent's write end pipe
        if (close(job->fd[1]) != 0) {
            perror("close");
            exit(errno);
        }
    }
}

void tokenize_message(struct Job *job, char *input, char **save_ptr) {
    // Initialize variables
    char *token_out = NULL;
    char *token_in = NULL;
    char *save_ptr_in = NULL;
    int argv_pos = 0;
    int envp_pos = 0;

    // Tokenize an argv section
    token_out = strtok_r(NULL, "\n", save_ptr);
    token_in = strtok_r(token_out, "\t", &save_ptr_in);
    while (NULL != token_in) {
        job->argv[argv_pos] = token_in;
        token_in = strtok_r(NULL, "\t", &save_ptr_in);
        argv_pos++;
    }

    // Tokenize an envp section
    token_out = strtok_r(NULL, "\n", save_ptr);
    token_in = strtok_r(token_out, "\t", &save_ptr_in);
    while (NULL != token_in) {
        job->envp[envp_pos] = token_in;
        token_in = strtok_r(NULL, "\t", &save_ptr_in);
        envp_pos++;
    }
}

int main(int argc, char **argv) {
    // Initialize variables
    static struct option long_opt[] = {
        {"limit", required_argument, 0, 'l'},
        {0, 0, 0, 0}
    };
    struct Job *new_job;
    struct Job *curr_job;
    struct sockaddr_un sock_addr;
    pid_t exited_pid = -1;
    int sock_fd = -1;
    int acpt_sock_fd = -1;
    int opt = 0;
    int max_num_jobs = 5;
    int curr_num_jobs = 0;
    int wait_status = 0;
    char *cmd_type = NULL;
    char *cmd_pid = NULL;
    char *cmd_val = NULL;
    char *save_ptr = NULL;
    char *msg_in = NULL;
    char *msg_out = NULL;

    // Handle option
    while ((opt = getopt_long(argc, argv, "l:", long_opt, NULL)) != -1) {
        if (opt == 'l') {
            sscanf(optarg, "%i", &max_num_jobs);
        }
        else {
            exit(EINVAL);
        }
    }

    // Check if the argument(s) is(are) valid
    if (optind < argc) {
        fprintf(stderr, "%s: invalid argument(s)\n", argv[0]);
        exit(EINVAL);
    }

    // Create a socket and get its file descriptor
    sock_fd = socket(PF_LOCAL, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (0 > sock_fd) {
        perror("socket");
        exit(errno);
    }

    // Assign values to the sock_addr struct
    sock_addr.sun_family = AF_LOCAL;
    strcpy(sock_addr.sun_path, "/tmp/mymoon.sock");

    // Remove the existing socket file
    unlink("/tmp/mymoon.sock");

    // Assign a name to socket
    if (bind(sock_fd, (struct sockaddr *) &sock_addr, sizeof(struct sockaddr_un)) != 0) {
        perror("bind");
        exit(errno);
    }
    
    // Make a socket to accept incoming connections
    if (listen(sock_fd, 5) != 0) {
        perror("listen");
        exit(errno);
    }

    // Allocate memory for messages
    msg_in = malloc(MSG_SIZE);
    if (NULL == msg_in) {
        perror("malloc");
        exit(errno);
    }
    msg_out = malloc(MSG_SIZE);
    if (NULL == msg_out) {
        perror("malloc");
        exit(errno);
    }

    while (1) {
        // Accept an incoming connection and create its new file descriptor
        acpt_sock_fd = accept(sock_fd, NULL, NULL);
        if (0 > acpt_sock_fd && EAGAIN != errno && EWOULDBLOCK != errno) {
            perror("accept");
            exit(errno);
        }
        // Receive a message from a socket
        if (recv(acpt_sock_fd, msg_in, MSG_SIZE, 0) >= 0) {
            // Tokenize a command type and argument sections
            cmd_type = strtok_r(msg_in, "\n", &save_ptr);
            cmd_pid = strtok_r(NULL, "\n", &save_ptr);
            cmd_val = strtok_r(NULL, "\n", &save_ptr);

            // If the command type is a "job"
            if ('j' == cmd_type[0]) {
                // Count the number of current jobs
                curr_num_jobs = 0;
                LIST_FOREACH(curr_job, &head, entries) {
                    if (strcmp(curr_job->status, "RUNNING") == 0) {
                        curr_num_jobs++;
                    }
                }
                
                // Do not execute if the maximum number of concurrent jobs has been reached
                if (curr_num_jobs >= max_num_jobs) {
                    strcpy(msg_out, "server: the server is currently running at its maximum "
                                        "capacity -- please try again later.");

                    // Send a message back on a socket
                    if (send(acpt_sock_fd, msg_out, strlen(msg_out) + 1, 0) < 0) {
                        perror("send");
                        exit(errno);
                    }

                    // Zero out memory for messages
                    memset(msg_in, 0, MSG_SIZE);
                    memset(msg_out, 0, MSG_SIZE);
                }
                // Execute if the maximum number of concurrent jobs has not been reached
                else {
                    // Allocate memories for the new job
                    new_job = calloc(1, sizeof(struct Job));
                    if (NULL == new_job) {
                        perror("calloc");
                        exit(errno);
                    }
                    new_job->output = calloc(MSG_SIZE, sizeof(char));
                    if (NULL == new_job->output) {
                        perror("calloc");
                        exit(errno);
                    }
                    new_job->argv = calloc(MSG_SIZE, sizeof(char));
                    if (NULL == new_job->argv) {
                        perror("calloc");
                        exit(errno);
                    }
                    new_job->envp = calloc(MSG_SIZE, sizeof(char));
                    if (NULL == new_job->envp) {
                        perror("calloc");
                        exit(errno);
                    }
                    if ('t' == cmd_type[1]) {
                        new_job->time_lim = strtol(cmd_val, NULL, 10);
                        new_job->mem_lim = 1000000000;          // 1GB
                        new_job->nice_val = 10;                 // Default nice value
                    }
                    else if ('m' == cmd_type[1]) {
                        new_job->time_lim = 100000;             // ~ 27 Hours
                        new_job->mem_lim = strtol(cmd_val, NULL, 10);
                        new_job->nice_val = 10;                 // Default nice value
                    }
                    else if ('n' == cmd_type[1]) {
                        new_job->time_lim = 100000;             // ~ 27 Hours
                        new_job->mem_lim = 1000000000;          // 1GB
                        new_job->nice_val = strtol(cmd_val, NULL, 10);
                    }
                    else {
                        new_job->time_lim = 100000;             // ~ 27 Hours
                        new_job->mem_lim = 1000000000;          // 1GB
                        new_job->nice_val = 10;                 // Default nice value
                    }

                    // Tokenize a message
                    tokenize_message(new_job, msg_in, &save_ptr); 

                    // Update new job and then save it to the linked list
                    strcpy(new_job->prog_name, new_job->argv[0]);
                    strcpy(new_job->status, "RUNNING");
                    new_job->acpt_sock_fd = acpt_sock_fd;
                    new_job->exit_code = -1;
                    LIST_INSERT_HEAD(&head, new_job, entries);

                    // Execute a new job
                    execute_job(new_job);

                    // Free memory for argv and envp
                    free(new_job->argv);
                    free(new_job->envp);

                    // Zero out memory for msg_in
                    memset(msg_in, 0, MSG_SIZE);
                }
            }
            // If the command type is not a "job"
            else {
                handle_command(cmd_type, cmd_pid, cmd_val, &msg_out);

                // Send a message back on a socket
                if (send(acpt_sock_fd, msg_out, strlen(msg_out) + 1, 0) < 0) {
                    perror("send");
                    exit(errno);
                }

                // Zero out memory for messages
                memset(msg_in, 0, MSG_SIZE);
                memset(msg_out, 0, MSG_SIZE);
            } 
        }

        // Check if any child processes have been terminated
        exited_pid = waitpid(-1, &wait_status, WNOHANG);
        if (-1 == exited_pid && ECHILD != errno) {
            perror("waitpid");
            exit(errno);
        }

        // If one of a child processes has been terminated
        if (0 < exited_pid) {
            // Iterate through the list to find a job with a matching pid
            LIST_FOREACH(curr_job, &head, entries) {
                if (exited_pid == curr_job->pid && strcmp(curr_job->status, "RUNNING") == 0) {
                    // Read from fd[0] into curr_job->output
                    if (read(curr_job->fd[0], curr_job->output, MSG_SIZE) < 0) {
                        perror("read");
                        exit(errno);
                    }

                    // Update status and exit_code
                    strcpy(curr_job->status, "COMPLED");
                    if (WIFEXITED(wait_status) == 1) {
                        curr_job->exit_code = WEXITSTATUS(wait_status);
                    }
                    
                    // Send a message back on a socket
                    if (send(curr_job->acpt_sock_fd, curr_job->output, strlen(curr_job->output) + 1, 0) < 0) {
                        perror("send");
                        exit(errno);
                    }

                    // Close a newly created file descriptor
                    close(curr_job->acpt_sock_fd);

                    break;
                }
            }
        }
    }

    // Close a socket's file descriptor
    close(sock_fd);

    // Remove the existing socket file
    unlink("/tmp/mymoon.sock");

    exit(0);
}