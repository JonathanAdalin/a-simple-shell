// Jonathan Adalin
// 260636904

// Assumptions:
// The user will only input the commands that we
// were asked to implement 


// TODO: 
// fg (just make the shell wait for the program?)
// jobs
// output redirection
// piping
// memory leak problems i.e. 
// allocate memory to pointers using malloc()
// free pointers using free()
// uncomment ctrl C handler

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
// Added libraries
#include <sys/wait.h>
#include <signal.h>

// Function Declaratons
int getcmd(char *prompt, char *args[], int *background);
static void sigHandlerKill(int sig);
static void sigHandlerIgnore(int sig);
static void shellCd(char *args[]);
static void shellPwd();
static void shellExit();
static void shellJobs(char *jobList[]);
static void addJob(char *arg, char job[20]);
static void removeJob(char job[20]);

// Global Variables
int jobCounter = 0;

int main(void) {
	char *args[20];
	char jobList[20][20];

	// clean JobList
	for (int i = 0; i<20; i++){
		snprintf(jobList[i], 5,"NA");
	}

	int bg;

	// TODO Uncomment before submission
	// Signal Handlers

	// Ctrl C
	// if (signal(SIGINT, sigHandlerKill) == SIG_ERR) {
	// 	printf("ERROR! Could not bind the signal hander\n");
	// 	exit(1);
	// }

	//Ctrl Z
	if (signal(SIGTSTP, sigHandlerIgnore) == SIG_ERR) {
		printf("ERROR! Could not bind the signal hander \n");
		exit(1);
	}

	while(1) {
		bg = 0;
		int cnt = getcmd("\n>> ", args, &bg);
		
		int pid = fork();
		if (pid == 0){
			int jobNumber = jobCounter;			
			jobCounter = jobCounter + 1;
			addJob(args[0], jobList[jobNumber]);
			// Child goes here
			if(strcmp(args[0], "cd") == 0){
				shellCd(args);
				removeJob(jobList[jobNumber]);
			}else if(strcmp(args[0], "pwd") == 0){
				shellPwd();
				removeJob(jobList[jobNumber]);
			}else if(strcmp(args[0], "exit") == 0){
				for (int i = 0; i < 20; i++){
					free(jobList[i]);
				}
				shellExit();
			}else if(strcmp(args[0], "jobs") == 0){
				printf("Listing jobs:\n");
				for (int i = 0; i<20; i++){	
					if (strcmp(jobList[i], "NA") != 0)
						printf("%s\n", jobList[i]);
				}
				removeJob(jobList[jobNumber]);
			}else{
				execvp(args[0], args);
				// No code is executed past this point
			}
		} else {
			// Parent goes here
			if (!bg){
				waitpid(pid, NULL, 0);	
			}
		}
	}
}

int getcmd(char *prompt, char *args[], int *background)
{
	// Clean args
	for (int i = 0; i<20; i++){
		args[i] = 0;
	}

	int length, i = 0;
	char *token, *loc;
	char *line = NULL;
	size_t linecap = 0;

	printf("%s", prompt);
	length = getline(&line, &linecap, stdin);

	if (length <= 0) { 
		exit(-1);
	}

	// Check if background is specified.. 
	if ((loc = index(line, '&')) != NULL) {
		*background = 1;
		*loc = ' '; 
	} else
		*background = 0;

	while ((token = strsep(&line, " \t\n")) != NULL) {
		for (int j = 0; j < strlen(token); j++)
			if (token[j] <= 32)
				token[j] = '\0';
		if (strlen(token) > 0)
			args[i++] = token;
	}
return i;
}

static void sigHandlerKill(int sig){
	printf(" Caught signal %d, Ctrl+C Acknowledged, killing process.\n", sig);
	kill(0, 0); // Kill child
}

static void sigHandlerIgnore(int sig){
	printf(" Caught signal %d, Ctrl+Z Acknowledged, ignoring.\n", sig);
}

static void shellCd(char *args[]){
	printf("Going to: %s", args[1]);
	chdir(args[1]);
}

static void shellPwd(){
	printf("Present working directory is: %s", getcwd(NULL,0));
}

static void shellExit(){
	printf("Leaving shell\n");
	exit(0);
}

static void addJob(char *arg, char job[20]){
	snprintf(job, 20,"%d %s",jobCounter, arg);
}

static void removeJob(char job[20]){
	snprintf(job, 20,"%s", "NA");
}