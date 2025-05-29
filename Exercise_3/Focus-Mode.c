#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define SIG1 10
#define SIG2 12
#define SIG3 31


void inputResponse(char *c) {
    printf("%c", *c);
    if (*c == '1') {

    }else if (*c == '2') {

    } else if (*c == '3') {

    } else if (*c == 'q') {

    } else {
        printf("Bad input!\n");
    }
}

// This function runs the text loop
void focusModeRound(int duration) {
    char chr;
    for (int i = 0; i < duration; ++i) {
        printf("Entering Focus Mode. All distractions are blocked.\n "
               "══════════════════════════════════════════════\n"
               "                Focus Round %d\n"
               "──────────────────────────────────────────────\n", (i+1));
        printf("Simulate a distraction:\n"
               "  1 = Email notification\n"
               "  2 = Reminder to pick up delivery\n"
               "  3 = Doorbell Ringing\n"
               "  q = Quit\n");

        scanf(" %c", &chr);
        inputResponse(&chr);
    }
}

void blockSigs(sigset_t set) {
    sigaddset(&set, SIG1);
    sigaddset(&set, SIG2);
    sigaddset(&set, SIG3);
    return;
}

// Main Focus function
void runFocusMode(int numOfRounds, int duration) {
    sigset_t set;

    for (int i = 0; i < numOfRounds; ++i) {
        blockSigs(set);
        sigprocmask(SIG_BLOCK, &set, NULL);
        focusModeRound(duration);
        sigemptyset(&set);
        sigprocmask(SIG_BLOCK, &set, NULL);
        printf("alive\n");
    }
}

