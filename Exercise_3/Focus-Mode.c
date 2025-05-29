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

// Handling the first signal
void handleSig1() {
    printf("[Outcome:] The TA announced: Everyone get 100 on the exercise!\n");
}

// Handling the second signal
void handleSig2() {
    printf("[Outcome:] You picked it up just in time.\n");
}

// Handling the third signal
void handleSig3() {
    printf("[Outcome:] Food delivery is here.\n");
}

void inputResponse(char *c) {
    printf("%c", *c);
    if (*c == '1') {
        raise(SIG1);
    }else if (*c == '2') {
        raise(SIG2);
    } else if (*c == '3') {
        raise(SIG3);
    }
}

// This function runs the text loop
void focusModeRound(int duration) {
    char chr;
    for (int i = 0; i < duration; ++i) {
        printf("Entering Focus Mode. All distractions are blocked.\n "
               "══════════════════════════════════════════════\n"
               "                Focus Round %d\n"
               "──────────────────────────────────────────────\n", (i + 1));
        printf("Simulate a distraction:\n"
               "  1 = Email notification\n"
               "  2 = Reminder to pick up delivery\n"
               "  3 = Doorbell Ringing\n"
               "  q = Quit\n");

        scanf(" %c", &chr);
        if (chr == 'q') {
            return;
        }
        inputResponse(&chr);
    }
}

void blockSigs(sigset_t *set) {
    sigaddset(set, SIG1);
    sigaddset(set, SIG2);
    sigaddset(set, SIG3);
    return;
}

// Main Focus function
void runFocusMode(int numOfRounds, int duration) {
    sigset_t set;
    // Setting up the signal handlers
    signal(SIG1, handleSig1);
    signal(SIG2, handleSig2);
    signal(SIG3, handleSig3);
    blockSigs(&set);
    // main focus mode loop
    for (int i = 0; i < numOfRounds; ++i) {
        // Blocking the three defined signals and setting the mask on
        sigprocmask(SIG_SETMASK, &set, NULL);
        focusModeRound(duration);
        // Unblocking the three defined signals
        sigprocmask(SIG_UNBLOCK, &set, NULL);
    }
}