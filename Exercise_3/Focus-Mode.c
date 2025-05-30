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
    printf(" - Email notification is waiting.\n");
    printf("[Outcome:] The TA announced: Everyone get 100 on the exercise!\n");
}

// Handling the second signal
void handleSig2() {
    printf(" - You have a reminder to pick up your delivery.\n");
    printf("[Outcome:] You picked it up just in time.\n");
}

// Handling the third signal
void handleSig3() {
    printf(" - The doorbell is ringing.\n");
    printf("[Outcome:] Food delivery is here.\n");
}

void inputResponse(char *c) {
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
        printf("\nSimulate a distraction:\n"
               "  1 = Email notification\n"
               "  2 = Reminder to pick up delivery\n"
               "  3 = Doorbell Ringing\n"
               "  q = Quit\n"
               ">> ");

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
    sigprocmask(SIG_SETMASK, set, NULL);
}

// Setting up signal handlers
void sigSetup(sigset_t *set) {
    struct sigaction usr_action;
    usr_action.sa_handler = handleSig1;
    usr_action.sa_mask = *set;
    usr_action.sa_flags = 0;
    sigaction(SIG1, &usr_action, NULL);

    usr_action.sa_handler = handleSig2;
    sigaction(SIG2, &usr_action, NULL);

    usr_action.sa_handler = handleSig3;
    sigaction(SIG3, &usr_action, NULL);
}

void handleSignals() {
    sigset_t s;
    sigpending(&s);
    int flag = 0;

    if (sigismember(&s, SIG1)) {
        sigwaitinfo(&s, NULL);
        handleSig1();
        flag = 1;
    }
    if (sigismember(&s, SIG2)) {
        sigwaitinfo(&s, NULL);
        handleSig2();
        flag = 1;
    }
    if (sigismember(&s, SIG3)) {
        sigwaitinfo(&s, NULL);
        handleSig3();
        flag = 1;
    }
    if (flag == 0) {
        printf("No distractions reached you this round.\n");
    }
}

// Main Focus function
void runFocusMode(int numOfRounds, int duration) {
    // Setting up the blocking sets and the sigactions
    sigset_t set;
    sigSetup(&set);
    blockSigs(&set);
    sigprocmask(SIG_BLOCK, &set, NULL);
    printf("Entering Focus Mode. All distractions are blocked.\n");
    // main focus mode loop
    for (int i = 0; i < numOfRounds; ++i) {
        // Blocking the three defined signals and setting the mask on
        printf("══════════════════════════════════════════════\n"
               "                Focus Round %d                \n"
               "──────────────────────────────────────────────\n", (i + 1));
        focusModeRound(duration);
        printf("──────────────────────────────────────────────\n"
               "        Checking pending distractions...        \n"
               "──────────────────────────────────────────────\n");
        handleSignals();
        printf("──────────────────────────────────────────────\n"
               "             Back to Focus Mode.\n"
               "══════════════════════════════════════════════\n");
    }
    printf("\nFocus Mode complete. All distractions are now unblocked.\n");
}