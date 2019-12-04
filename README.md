# cbeauty

A collection of C programs to demonstrate the language, the unix operating system, and the Linux kernel API (system calls).

Code samples are sourced from:
* The internet (Stackoverflow, Geeksforgeeks, etc.)
* My college operating systems class
* [The Linux Programming Interface](https://www.amazon.com/Linux-Programming-Interface-System-Handbook/dp/1593272200)
* [Operating System Concepts (ie. The Dinosaur Book)]()
* [United States Naval Academy course]https://www.usna.edu/Users/cs/aviv/classes/ic221/s18/units.html

Reference Docs:
* The GNU libc manual: https://www.gnu.org/software/libc/manual/

## Man Pages
Linux man pages are organized into different sections.
* Section 1 - user commands
* Section 2 - system calls
* Section 3 - library functions
* Section 4 - special files
* Section 5 - file formats
* Section 6 - games
* Section 7 - conventions and miscellany
* Section 8 - administration and privileged commands
* Section L - math library functions
* Section N - tcl functions

Sometimes you need to specify which page you specifically want. 
To see a list of system calls in the man pages, which is in Section 2 ```man 2 syscalls```

To search through all pages for particular text, use ```man -k <text> | less```

## Job Commands
| Command   | Explanation                                  | Example                                 |
|-----------|----------------------------------------------|-----------------------------------------|
| &         | Run the command in the background            | $ long_cmd &                            |
| Ctrl-z    | Stop the foreground process                  | [Ctrl-z] Stopped                        |
| jobs      | List background processes                    | $ jobs [1] - Stopped vi [2] - big_job & |
| %n        | Refers to the background number n            | $ fg %1                                 |
| %?str     | Refers to the background job containing str  | $ fg %?ls                               |
| bg        | Restart a stopped background process         | $ bg [2] big_job &                      |
| fg        | Bring a background process to the foreground | $ fg %1                                 |
| kill      | Kill a process                               | $ kill %2                               |
| ~ Ctrl-z  | Suspend an rlogin or ssh session             | host2>~[Ctrl-z] Stopped host1>          |
| ~~ Ctrl-z | Suspend a second level rlogin or ssh session | host3>~~[Ctrl-z] Stopped host2>         |

## Process groups & Pipelines Processses
The command "ps -ef | grep foo | less" is an example of pipelined processes.
Child processes automatically inhert process groups of their parents. However, even though bash forks your pipeline, bash runs as a separate process group from pipelined processes. This is because each pipeline is run in a standalone 'job'. Each job runs in it's own process group for the purpose of terminal signaling (ie. you enter Ctrl-C but it shouldn't affect your terminal, just the process group in the foreground of the terminal). In fact, the kill signal is sent to the every process in the foreground group! System calls can be used to programatically manage process ids/groups. 

Reads:
  - pid_t getpid() - get the process id for the calling process
  - pid_t getppid() - get the process id of the parent of the calling proces
  - pid_t getpgrp() - get the prcesso group id of the calling process
  - pid_t getpgid(pid_t pid) - get the process group id for the proces identified by pid

Modifications:
  - pid_t setpgrp() - set the process group of the calling process to iteself, i.e. after a call to setpgr(), the following condition holds getpid() == getpgrp().
  - pid_t setpgid(pid_t pid, pid_t pgid) - set the process group id of the process identified by pid to the pgid, if pid is 0, then set the process group id of the calling process, and if pgid is 0, then the pid of the process identified by pid and is made the same as its process group, i.e., setpgid(0,0) is equivalent to calling setpgrp().
  - pid_t tcsetpgrp() - indicates which process is in the foreground

## Sending Signals
### kill
```
kill -9 [PID] // kills (sends SIGKILL signal) specific process ID
kill -s KILL [PID]
kill -KILL [PID]
```

### killall
By default killall finds all processes by name and sends SIGTERM signal. You can change it with flags.
```
killall [process name] // SIGTERM, the default
killall -s 9 [process name] // SIGKILL
killall -KILL [process name] // SIGKILL
killall -SIGKILL [process name] // SIGKILL
```

### pkill
Similar to killall. You can use ```pgrep``` to find process names.
```
pkill -KILL [process name] // pkill by name
pkill -KILL -u [user] [process name] // pkill by name, but only if its 
running as the effective user.
```