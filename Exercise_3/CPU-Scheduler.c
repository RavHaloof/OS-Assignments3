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
void readCSV(FILE** csvFile) {
    char line[MAX_LINE_LENGTH];
    if (fgets(line, sizeof(line), *csvFile) != NULL) {
        printf("%s\n", line);
    }
}


void runCPUScheduler(char* processesCsvFilePath, int timeQuantum) {
    printf("hi\n");
    FILE *dataFile = fopen(processesCsvFilePath, "r");
    readCSV(&dataFile);
}