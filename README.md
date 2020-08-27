# Job Submission System

#### https://github.com/msukmoon/job-submission-system

This is a job submission system for Linux.

## Getting Started

TBA

### Running the Application

#### Requirements

TBA

#### Instructions

##### Running the Server

- To compile it with the Makefile
	```shell
	make
	```
- To compile it manually with gcc
	```shell
	gcc server.c -o server -Wall -Werror
	```
- To run it
	```shell
	./server [-l, --limit pid]
	```

##### Running the Client

- To compile it with the Makefile
	```shell
	make
	```
- To compile it manually with gcc
	```shell
	gcc client.c -o client -Wall -Werror
	```
- To run it
	```shell
	./client [-l, --list pid or “all”] 
	[-o, --output pid] [-s, --status pid] 
	[-e, --exitcode pid] [-k, - -kill pid] 
	[-r, --terminate pid] [-z, --suspend pid] 
	[-c, --continue pid] [-u -n, --update pid – nice val] 
	[-t, --time val] [-m, --memory val] 
	[-n, --nice val] command
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
