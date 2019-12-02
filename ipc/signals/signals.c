#include <signal.h> /*for signal() and raise()*/
#include <stdio.h>
#include <stdlib.h>

/**
 * Each process has a list of signal handlers. Almost any signal handle can be
 * overridden, besides SIGSTOP & SIGKILL. A signal handler function can have any
 * name, but must have return type void and have one int parameter.
 */
/*
 * ======== TABLE OF COMMON SIGNALS =======
 * Signal     Value     Action   Comment
 * ----------------------------------------------------------------------
 * SIGHUP        1       Term    Hangup detected on controlling terminal
 *                               or death of controlling process
 * SIGINT        2       Term    Interrupt from keyboard
 * SIGQUIT       3       Core    Quit from keyboard
 * SIGILL        4       Core    Illegal Instruction
 * SIGABRT       6       Core    Abort signal from abort(3)
 * SIGFPE        8       Core    Floating point exception
 * SIGKILL       9       Term    Kill signal
 * SIGSEGV      11       Core    Invalid memory reference
 * SIGPIPE      13       Term    Broken pipe: write to pipe with no readers
 * SIGALRM      14       Term    Timer signal from alarm(2)
 * SIGTERM      15       Term    Termination signal
 * SIGUSR1   30,10,16    Term    User-defined signal 1
 * SIGUSR2   31,12,17    Term    User-defined signal 2
 * SIGCHLD   20,17,18    Ign     Child stopped or terminated
 * SIGCONT   19,18,25    Cont    Continue if stopped
 * SIGSTOP   17,19,23    Stop    Stop process
 * SIGTSTP   18,20,24    Stop    Stop typed at tty
 * SIGTTIN   21,21,26    Stop    tty input for background process
 * SIGTTOU   22,22,27    Stop    tty output for background process
 */
/**
 * ======== SIGNAL ACTIONS ========
 * Term : The process will terminate
 * Core : The process will terminate and produce a core dump file that traces
the process state at the time of termination.
 * Ign  : The process will ignore the signal
 * Stop : The process will stop, like with a Ctrl-Z
 * Cont : The process will continue from being stopped
*/
void hello(int signum) {
  printf("Signal handler for signal number: %d\n", signum);
}

int main() {
  // execute hello() when receiving signal SIGUSR1.
  // Failure can happen like if you try to override SIGKILL/SIGSTOP.
  if (signal(SIGUSR1, hello) == SIG_ERR) {
    perror("error trying to override signal handler.");
    exit(1);
  }

  // explicitly send SIGUSR1 to the current process.
  raise(SIGUSR1);
}