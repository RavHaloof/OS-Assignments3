#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_NAME 50
#define MAX_DESCRIPTION 100
#define MAX_LINE_LENGTH 256
#define MAX_PROCESSES 1000
#define FILE_VARIABLES 5

// A struct for processes
typedef struct {
    char name[MAX_NAME];
    char desc[MAX_DESCRIPTION];
    int arrival;
    int burst;
    int priority;
} Process;

// The first come, first served scheduler system implementation
void FCFS(char ***processMatrix) {

}

// The shortest job first scheduler system implementation
void SJF(char ***processMatrix) {

}

// The priority scheduler system implementation
void priority(char ***processMatrix) {

}

// The Round Robin scheduler system implementation
void roundRobin(char ***processMatrix, int quantum) {

}

// The function that will read the given CSV, and send it back as a matrix
void readCSV(FILE** csvFile, Process array[MAX_DESCRIPTION]) {
    char line[MAX_LINE_LENGTH];
    char *token;
    int i = 0;
    // Getting rid of the first two lines since they're useless
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
}

void runCPUScheduler(char* processesCsvFilePath, int timeQuantum) {
    FILE *dataFile = fopen(processesCsvFilePath, "r");
    Process processArr[MAX_PROCESSES];
    readCSV(&dataFile, processArr);
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        if (strcmp(processArr[i].name, "") == 0) {
            break;
        }
        printf("name: %s\n"
               "desc: %s\n"
               "arrival time: %d\n"
               "burst time: %d\n"
               "priority: %d\n\n", processArr[i].name, processArr[i].desc, processArr[i].arrival,
               processArr[i].burst, processArr[i].priority);
    }
}