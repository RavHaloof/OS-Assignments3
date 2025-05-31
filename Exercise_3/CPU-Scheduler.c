#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <limits.h>

#define MAX_NAME 50
#define MAX_DESCRIPTION 100
#define MAX_LINE_LENGTH 256
#define MAX_PROCESSES 1000
#define FILE_VARIABLES 5

int timer = 0;

// A struct for processes
typedef struct {
    char name[MAX_NAME];
    char desc[MAX_DESCRIPTION];
    int arrival;
    int burst;
    int priority;
} Process;

void alarmHandler(int sig) {
}

void contHandler(int sig) {
}

// Sets up a mask which blocks all signals except for the ones we will use
void blockSigsCPU() {
    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGALRM);
    sigdelset(&set, SIGSTOP);
    sigdelset(&set, SIGCONT);
    sigdelset(&set, SIGKILL);
    sigprocmask(SIG_SETMASK, &set, NULL);
}

// Function which simulates a run for a process for a given amount of seconds (burst)
pid_t startProcess() {
    pid_t procPid = fork();
    if (procPid == 0) {
        // The child process activates the cont signal handler, and waits for signals from
        // The parent to activate and deactivate
        signal(SIGCONT, contHandler);
        blockSigsCPU();
        while (1) {
            pause();
            printf("Received a signal from parent\n");
            kill(getppid(), SIGCONT);
        }
    }
    return procPid;
}

// Function which simulates the running of a process with a given burst time
void runProcess(pid_t pid, int burst, Process p) {
    // Waits for the amount of time given in the burst
    alarm(burst);
    pause();
    // Sends a signal to the child to wake it up (not really necessary, but I wanted to simulate
    // Contact between the schedule and the processes themselves
    kill(pid, SIGCONT);
    // Waits for a signal back, indicating that the child indeed got the signal given to it
    pause();
    printf("%d → %d: %s Running %s.\n", timer, (timer + burst), p.name, p.desc);
    // Updates the timer now that we've ran the process
    timer += burst;
}

// The first come, first served scheduler system implementation
void FCFS(Process array[MAX_DESCRIPTION], int processNum) {
    // Setting up variables to use later, live processes is the amount of processes which
    // Have not finished yet
    int liveProcesses = processNum;
    // Arrival time will store the minimum arrival time that we found in the process array
    int arrivalTime = INT_MAX;
    // Current process will store that process that is being run right now
    int currentProcess = 0;
    pid_t currentProcessPID;
    // This array will store the arrival time of each process, so that we
    // Will be able to change it without hurting the original collected data
    int arrivalArr[processNum];
    // Setting the arrays to have their supposed values
    for (int i = 0; i < processNum; ++i) {
        arrivalArr[i] = array[i].arrival;
    }
    // While we still haven't finished all the processes
    while (liveProcesses > 0) {
        // Finding the process which arrived the earliest
        for (int i = 0; i < processNum; ++i) {
            if (arrivalArr[i] < arrivalTime) {
                arrivalTime = arrivalArr[i];
                currentProcess = i;
            }
        }
        // Starting the process, and running it to its full burst time
        currentProcessPID = startProcess();
        runProcess(currentProcessPID, array[currentProcess].burst, array[currentProcess]);
        kill(currentProcessPID, SIGKILL);
        // Once we finished running the process, set its arrival time to be the maximum so that
        // We don't run it again
        arrivalTime = INT_MAX;
        arrivalArr[currentProcess] = INT_MAX;
        liveProcesses--;
    }
    timer = 0;
}

// The shortest job first scheduler system implementation
void SJF(Process array[MAX_DESCRIPTION], int processNum) {

}

// The priority scheduler system implementation
void priority(Process array[MAX_DESCRIPTION], int processNum) {

}

// The Round Robin scheduler system implementation
void roundRobin(Process array[MAX_DESCRIPTION], int processNum, int quantum) {

}

// The function that will read the given CSV, and send it back as a matrix
int readCSV(FILE** csvFile, Process array[MAX_DESCRIPTION]) {
    char line[MAX_LINE_LENGTH];
    char *token;
    int i = 0;

    // Getting rid of the first two lines since they're useless
    //TODO: DELETE THOSE TWO LINES LATER, THEY'RE THERE ONLY FOR TESTING
    fgets(line, sizeof(line), *csvFile);
    fgets(line, sizeof(line), *csvFile);

    // Setting the process array
    while (fgets(line, sizeof(line), *csvFile) != NULL) {
        // Setting the name
        token = strtok(line, ",");
        strcpy(array[i].name, token);

        // Setting the description
        token = strtok(NULL, ",");
        strcpy(array[i].desc, token);
        // Setting the arrival time
        token = strtok(NULL, ",");
        array[i].arrival = atoi(token);

        // Setting the burst time
        token = strtok(NULL, ",");
        array[i].burst = atoi(token);

        // Setting the priority
        token = strtok(NULL, ",");
        array[i].priority = atoi(token);

        i++;
    }
    return i;
}

void runCPUScheduler(char* processesCsvFilePath, int timeQuantum) {
    signal(SIGALRM, alarmHandler);
    signal(SIGCONT, contHandler);
    // Opening the file
    FILE *dataFile = fopen(processesCsvFilePath, "r");
    // Creating an array of processes
    Process processArr[MAX_PROCESSES];
    // Saving all processes in the process array, and saving the total amount of processes
    int processNum = readCSV(&dataFile, processArr);
    FCFS(processArr, processNum);
    SJF(processArr, processNum);
    priority(processArr, processNum);
    roundRobin(processArr, processNum, timeQuantum);
}