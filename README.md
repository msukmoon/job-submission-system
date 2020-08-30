# Job Submission System

#### https://github.com/msukmoon/job-submission-system

This is a job submission system for Linux. A client could submit jobs with setting options, and a server in a same system will execute these jobs asynchronously and report their status. A client could also issue various commands to the server to list one or all jobs, get an output of an old job, get a job status, get an exit code of a job, kill, terminate, suspend or continue a job, update nice value of a job and set a CPU running time limit, memory/stack size limit or nice value of a new job. The server could be launched with setting a maximum number of jobs that could be run concurrently.

For example, a server will list all jobs when a client issues the command ```./client --list all```:

```
PID     PNAME   STATUS  ETCODE  NICEVL  TMELIM  MEMLIM  
2195    sleep   KILLED  137     10      100000  1000000000  
2190    sleep   COMPLED 0       15      100000  1000000000  
2188    ls      COMPLED 0       10      100000  1000000000  
2160    sleep   RUNNING -1      12      400000  2000000000  
2158    cat     COMPLED 0       10      100000  1000000000  
2156    rm      COMPLED 0       10      30000   1000000000  
2154    ls      COMPLED 0       10      100000  100000000  
2150    mkdir   COMPLED 0       10      100000  1000000000  
2148    pwd     COMPLED 0       10      100000  300000000  
2143    echo    COMPLED 0       10      5000    1000000000
```

## Getting Started

### Running the Application

#### Requirements

- Linux (preferably Ubuntu 18.04 or higher)
- GNU Make
- GNU Compiler Collection (GCC)

#### Instructions

This system is divided into the server and client program.

##### Running the Server

- To compile it with the Makefile:
	```shell
	make
	```
- To compile it manually with gcc:
	```shell
	gcc server.c -o server -Wall -Werror
	```
- To run it:
	> **Note:** The server will not have any output when running.
	```shell
	./server [-l, --limit pid]
	```

##### Running the Client

- To compile it with the Makefile:
	```shell
	make
	```
- To compile it manually with gcc:
	```shell
	gcc client.c -o client -Wall -Werror
	```
- To run it:
	```shell
	./client [-l, --list pid or “all”] 
	[-o, --output pid] [-s, --status pid] 
	[-e, --exitcode pid] [-k, - -kill pid] 
	[-r, --terminate pid] [-z, --suspend pid] 
	[-c, --continue pid] [-u -n, --update pid – nice val] 
	[-t, --time val] [-m, --memory val] 
	[-n, --nice val] your-command
	```

##### Cleaning up the Program

```shell
make clean
```

##### Running the Test Scripts

```shell
make tests
```

#### Program Options

##### Server Options

- ```-l, --limit pid```
	- Set the max job number configuration

##### Client Options

- ```-l, --list pid or “all”```
	- Used to list one or all jobs
- ```-o, --output pid```
	- Used to get an output of a one job with a matching pid
- ```-s, --status pid```
	- Used to get a status (killed, terminated, completed, suspended or running) of a one job with a matching pid
- ```-e, --exitcode pid```
	- Used to get an exit code of a one job with a matching pid
- ```-k, --kill pid```
	- Used to kill a running job with a matching pid
- ```-r, --terminate pid```
	- Used to terminate a running job with a matching pid
- ```-z, --suspend pid```
	- Used to suspend a running job with a matching pid
- ```-c, --continue pid```
	- Used to continue a suspended job with a matching pid
- ```-u -n, --update pid –nice val```
	- Used to update a pid job’s nice value to val
- ```-t, --time val```
	- Used to set a cpu running time limit of a job pid
- ```-m, --memory val```
	- Used to set a memory/stack size limit of a job pid
- ```-n, --nice val```
	- Used to set a nice value of a job pid

## Author

- **Myungsuk (Jay) Moon** - [msukmoon](https://github.com/msukmoon) - jaymoon9876@gmail.com
