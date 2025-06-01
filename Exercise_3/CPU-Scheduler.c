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

void alarmHandler(int sig) {}

void contHandler(int sig) {}

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
    // Updates the timer now that we've run the process
    timer += burst;
}

// Idles the CPU for a given amount of time
void idling(int burst) {
    alarm(burst);
    pause();
    printf("%d → %d: Idle.\n", timer, (timer+burst));
    timer += burst;
}

// Checks to see whether the process has arrived yet or not, if it has, we run it
// Also returns the time when the process started running
int execProcess(int minArrivalTime, int burst, Process p) {
    pid_t currentProcessPID;

    // In case the process has not arrived yet
    if (minArrivalTime > timer) {
        idling(minArrivalTime - timer);
    }
    int startTime = timer;
    // Starting the process, and running it to its full burst time
    currentProcessPID = startProcess();
    runProcess(currentProcessPID, burst, p);
    // Killing the process now that we've finished running it
    kill(currentProcessPID, SIGKILL);
    return startTime;
}

// The initial printing output
void FCFSStart() {
    printf("══════════════════════════════════════════════\n"
           ">> Scheduler Mode : FCFS\n"
           ">> Engine Status  : Initialized\n"
           "──────────────────────────────────────────────\n"
           "\n");
}

// The output at the end of the program, calculating the average waiting time
void schedulerEnd(int *startTime, Process *pArray, int processCount) {
    double totalWait = 0;
    // Calculates the total amout of time waited
    for (int i = 0; i < processCount; ++i) {
        totalWait += (double) (startTime[i] - pArray[i].arrival);
    }

    printf("\n"
           "──────────────────────────────────────────────\n"
           ">> Engine Status  : Completed\n"
           ">> Summary        :\n"
           "   └─ Average Waiting Time : %.2f time units\n"
           ">> End of Report\n"
           "══════════════════════════════════════════════\n"
           "\n", (totalWait/(double) processCount));
}

// The first come, first served scheduler system implementation
void FCFS(Process array[MAX_DESCRIPTION], int processNum) {
    FCFSStart();
    /* Setting up variables to use later, they are for tracking the current process and its PID, and saving the minimum
     Saving the minimum arrival time
     */
    int minArrivalTime = INT_MAX;
    int currentProcess = 0;
    int arrivalArr[processNum];
    int startTime[processNum];

    // Setting the arrays to have their supposed values
    for (int i = 0; i < processNum; ++i) {
        arrivalArr[i] = array[i].arrival;
    }
    // While we still haven't finished all the processes
    for (int j = processNum; j > 0; j--) {
        // Finding the process which arrived the earliest
        for (int i = 0; i < processNum; ++i) {
            if (arrivalArr[i] < minArrivalTime) {
                minArrivalTime = arrivalArr[i];
                currentProcess = i;
            }
        }
        startTime[currentProcess] = execProcess(minArrivalTime, array[currentProcess].burst, array[currentProcess]);
        // Once we finished running the process, set its arrival time to be the maximum so that
        // We don't run it again
        minArrivalTime = INT_MAX;
        arrivalArr[currentProcess] = INT_MAX;
    }
    schedulerEnd(startTime, array, processNum);
    timer = 0;
}

// Finds the minimal value in an array
int findMin(int *array, int len) {
    int min = INT_MAX;
    for (int i = 0; i < len; ++i) {
        if (array[i] < min) {
            min = array[i];
        }
    }
    return min;
}

// Finds the process we should run next in case none of the processes have arrived yet
// Sending flag 1 is for SJF, anything else for priority
int chooseProcess(int *arrivalArray, int len, Process array[100], int flag) {
    int arrivalTime = findMin(arrivalArray, len);
    int chosenProcess;
    int chosenField = INT_MAX;
    // For SJF:
    if (flag == 1) {
        for (int i = 0; i < len; ++i) {
            if (arrivalArray[i] == arrivalTime) {
                if (array[i].burst < chosenField) {
                    chosenField = array[i].burst;
                    chosenProcess = i;
                }
            }
        }
    }
    // For priority:
    else {
        for (int i = 0; i < len; ++i) {
            if (arrivalArray[i] == arrivalTime) {
                if (array[i].priority < chosenField) {
                    chosenField = array[i].priority;
                    chosenProcess = i;
                }
            }
        }
    }
    return chosenProcess;
}

// First thing we print when we start SJF
void SJFStart() {
    printf("══════════════════════════════════════════════\n"
           ">> Scheduler Mode : SJF\n"
           ">> Engine Status  : Initialized\n"
           "──────────────────────────────────────────────\n"
           "\n");
}

// The shortest job first scheduler system implementation
void SJF(Process array[MAX_DESCRIPTION], int processNum) {
    SJFStart();
    /* Setting up parameters to save the current running process, its PID, and an array of the arrival time, and
    The amount of time left to finish each process*/
    int currentProcess;
    int shortestBurst = INT_MAX;
    int minArrivalTime = INT_MAX;
    int arrivalArr[processNum];
    int startTime[processNum];

    // Setting the arrays to have their supposed values
    for (int i = 0; i < processNum; ++i) {
        arrivalArr[i] = array[i].arrival;
    }
    // While we still haven't finished running all the processes
    for (int j = processNum; j > 0; j--) {
        for (int i = 0; i < processNum; ++i) {
            // Checking whether the process has arrived yet
            if (arrivalArr[i] <= timer) {
                // In case the process' burst is the same as the current minimum, we choose the one who arrived first
                if (shortestBurst == array[i].burst) {
                    if (minArrivalTime > arrivalArr[i]) {
                        shortestBurst = array[i].burst;
                        currentProcess = i;
                        minArrivalTime = array[i].arrival;
                    }
                }
                // In case the current process' burst is shorter, we just pick it
                else if(shortestBurst > array[i].burst) {
                    shortestBurst = array[i].burst;
                    currentProcess = i;
                    minArrivalTime = array[i].arrival;
                }
            }
        }
        if (minArrivalTime == INT_MAX) {
            minArrivalTime = findMin(arrivalArr, processNum);
            currentProcess = chooseProcess(arrivalArr, processNum, array, 1);
        }
        startTime[currentProcess] = execProcess(minArrivalTime, array[currentProcess].burst, array[currentProcess]);
        // Once we finished running the process, set its arrival time to be the maximum so that
        // We don't run it again
        minArrivalTime = INT_MAX;
        arrivalArr[currentProcess] = INT_MAX;
        shortestBurst = INT_MAX;
    }
    schedulerEnd(startTime, array, processNum);
    timer = 0;
}

void priorityStart() {
    printf("══════════════════════════════════════════════\n"
           ">> Scheduler Mode : Priority\n"
           ">> Engine Status  : Initialized\n"
           "──────────────────────────────────────────────\n"
           "\n");
}

// The priority scheduler system implementation
void priority(Process array[MAX_DESCRIPTION], int processNum) {
    priorityStart();
    /* Setting up parameters to save the current running process, its PID, and an array of the arrival time, and
    The amount of time left to finish each process*/
    int currentProcess;
    int highestPrio = INT_MIN;
    int minArrivalTime = INT_MAX;
    int arrivalArr[processNum];
    int startTime[processNum];

    // Setting the arrays to have their supposed values
    for (int i = 0; i < processNum; ++i) {
        arrivalArr[i] = array[i].arrival;
    }

    // While we still haven't finished running all the processes
    for (int j = processNum; j > 0; j--) {
        // Running over all available processes
        for (int i = 0; i < processNum; ++i) {
            // Checking whether the process has arrived yet
            if (arrivalArr[i] <= timer) {
                // In case the process' priority is the same as the current maximum, we choose the one who arrived first
                if (highestPrio == array[i].priority) {
                    if (minArrivalTime > arrivalArr[i]) {
                        highestPrio = array[i].priority;
                        currentProcess = i;
                        minArrivalTime = array[i].arrival;
                    }
                }
                // In case the current process' priority is lower, we just pick it
                else if(highestPrio > array[i].priority) {
                    highestPrio = array[i].priority;
                    currentProcess = i;
                    minArrivalTime = array[i].arrival;
                }
            }
        }
        // In case no process has arrived yet
        if (minArrivalTime == INT_MAX) {
            minArrivalTime = findMin(arrivalArr, processNum);
            currentProcess = chooseProcess(arrivalArr, processNum, array, -1);
        }
        startTime[currentProcess] = execProcess(minArrivalTime, array[currentProcess].burst, array[currentProcess]);
        // Once we finished running the process, set its arrival time to be the maximum so that
        // We don't run it again
        minArrivalTime = INT_MAX;
        arrivalArr[currentProcess] = INT_MAX;
        highestPrio = INT_MIN;
    }
    schedulerEnd(startTime, array, processNum);
}

// The Round Robin scheduler system implementation
void roundRobin(Process array[MAX_DESCRIPTION], int processNum, int quantum) {
    // Arrival array updates the process's arrival, burstArr updates their remaining bursts, activeProcesses checks
    // which processes can be run in the round-robin
    int arrivalArr[processNum];
    int burstArr[processNum];
    int activeProcesses[processNum];
    int burst = quantum;
    // Setting the arrays to have their supposed values
    for (int i = 0; i < processNum; ++i) {
        arrivalArr[i] = array[i].arrival;
        burstArr[i] = array[i].burst;
        activeProcesses[i] = 0;
    }

    for (int i = 0; i < processNum; ++i) {
        if (arrivalArr[i] < timer && burstArr[i] > 0) {
            activeProcesses[i] = 1;
        }
        for (int j = 0; j < processNum; ++j) {
            if (activeProcesses[i] == 1) {
                if (quantum > burstArr[i]) {
                    burst = burstArr[i];
                }
            }
            burst = quantum;
        }
    }
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