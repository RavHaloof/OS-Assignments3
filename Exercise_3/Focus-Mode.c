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

// Raising signals corresponding to the user's input
void inputResponse(char *c) {
    if (*c == '1') {
        raise(SIG1);
    } else if (*c == '2') {
        raise(SIG2);
    } else if (*c == '3') {
        raise(SIG3);
    }
}

// This function runs the user's input loop, showing the menu and scanning input
void focusModeRound(int duration) {
    char chr;
    for (int i = 0; i < duration; ++i) {
        // Menu
        printf("\n"
               "Simulate a distraction:\n"
               "  1 = Email notification\n"
               "  2 = Reminder to pick up delivery\n"
               "  3 = Doorbell Ringing\n"
               "  q = Quit\n"
               ">> ");

        // Scanning input and raising signals accordingly
        scanf(" %c", &chr);
        if (chr == 'q') {
            return;
        }
        inputResponse(&chr);
    }
}

// Makes a set, and adds all predefined signals to it, then blocks them using a mask
void blockSigs(sigset_t *set) {
    sigaddset(set, SIG1);
    sigaddset(set, SIG2);
    sigaddset(set, SIG3);
    sigprocmask(SIG_SETMASK, set, NULL);
}

// Setting up signal handlers
void sigSetup(sigset_t *set) {
    struct sigaction usr_action;
    // Using sigaction to change the signal's default handling to my own custom ones
    usr_action.sa_handler = handleSig1;
    usr_action.sa_mask = *set;
    usr_action.sa_flags = 0;
    sigaction(SIG1, &usr_action, NULL);

    usr_action.sa_handler = handleSig2;
    sigaction(SIG2, &usr_action, NULL);

    usr_action.sa_handler = handleSig3;
    sigaction(SIG3, &usr_action, NULL);
}

// Using sigismember and sigpending (since I have to)
void handleSignals(){
    // Saving all recieved signals in set s
    sigset_t s;
    sigpending(&s);
    // Setting up a flag to know whether any signals have been received at all
    int flag = 0;

    // Checking for signal 1 (SIGUSR1)
    if (sigismember(&s, SIG1)) {
        sigwaitinfo(&s, NULL);
        handleSig1();
        flag = 1;
    }
    // Checking for signal 2 (SIGUSR2)
    if (sigismember(&s, SIG2)) {
        sigwaitinfo(&s, NULL);
        handleSig2();
        flag = 1;
    }
    // Checking for signal 3 (SYSSIG)
    if (sigismember(&s, SIG3)) {
        sigwaitinfo(&s, NULL);
        handleSig3();
        flag = 1;
    }
    // In case none of them were received
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