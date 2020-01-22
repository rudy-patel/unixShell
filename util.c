#include "util.h"

bool helperText = true;

int numProcesses = 0;
int userTime = 0;
int sysTime = 0;
bool exitSignal = false;
int pids[100];

// can i have a pointer to the start??
int pidIndex = 0;



// List of builtin commands, followed by their corresponding functions.
char *builtinNames[] = 
{
    "jobs",
    "resume",
    "kill",
    "sleep",
    "suspend",
    "wait",
    "help",
    "exit"
};

int (*builtinFunc[]) (char **) = 
{
  &displayJobs,
  &resume,
  &killProcess,
  &sleepProcess,
  &suspendProcess,
  &waitProcess,
  &help,
  &exitCommand
};

int numFunctions() 
{
  return sizeof(builtinNames) / sizeof(char *);
}

// TODO: check args length, if more than 2, handle error accordingly...
bool resume(char **args) 
{
    printf("resume function call\n");
    if (checkNoArgs(args)) {
        printf("\nexpected another parameter...\nfollow this format: <cmd> <int>\n\n");
        return true;
    }

    return true;

}

bool valueInArray(int val, int *arr, int size){
    int i;
    for (i=0; i < size; i++) {
        if (arr[i] == val)
            return true;
    }
    return false;
}

bool killProcess(char **args) 
{
    printf("kill function call\n");
    //printf("the second arg: %s\n", args[1]);
    if (checkNoArgs(args)) {
        printf("\nexpected another parameter...\nfollow this format: <cmd> <int>\n\n");
        return true;
    }

    // NOT WORKING...
    if (valueInArray(args[1], pids, pidIndex + 1)) {
        kill(args[1]);
        printf("process killed.\n");
    } else {
        printf("invalid pid entered...\n");
    }

    return true;

}

bool sleepProcess(char **args) 
{
    printf("sleep function call\n");
    if (checkNoArgs(args)) {
        printf("\nexpected another parameter...\nfollow this format: <cmd> <int>\n\n");
        return true;
    }

    return true;

}

bool suspendProcess(char **args) 
{
    printf("suspend function call\n");
    if (checkNoArgs(args)) {
        printf("\nexpected another parameter...\nfollow this format: <cmd> <int>\n\n");
        return true;
    }

    return true;

}

bool waitProcess(char **args) 
{
    printf("wait function call\n");
    if (checkNoArgs(args)) {
        printf("\nexpected another parameter...\nfollow this format: <cmd> <int>\n\n");
        return true;
    }

    return true;

}

void help() 
{
    printf("help function call\n");
}

bool displayJobs(char **args)
{
    if (!checkNoArgs(args)) {
        printf("JOBS command does not take any arguments.\n");
        return true;
    }
    printf("\nRunning processes:\n");
    if (numProcesses > 0) {
        printf("  #     PID S SEC COMMAND\n");
        for (int i = 0; i < numProcesses; i++) {
            printf("  %d:   %d R SEC  %s\n", i, pids[i], args[0]);
        }
    }
    printf("Processes =    %d active\n", numProcesses);
    printf("Completed processes:\n");
    printf("User time =    %d seconds\n", userTime);
    printf("Sys  time =    %d seconds\n\n", sysTime);

    return true;
}

bool exitCommand(char **args) 
{
    if (!checkNoArgs(args)) {
        printf("EXIT command does not take any arguments.\n");
        return true;
    }
    printf("\nResources used\n");
    printf("User time =    %d seconds\n", userTime);
    printf("Sys  time =    %d seconds\n\n", sysTime);

    exitSignal = true;
    return false;
}

char *readLine() 
{
    char *inputLine = NULL;
    size_t bufferSize = BUFFER_SIZE;  // getLine allocates buffer size automatically
    getline(&inputLine, &bufferSize, stdin);
    return inputLine;
}

char **splitLine(char *line) 
{
    int tokenBufferSize = TOKEN_BUFFER_SIZE;
    int pos = 0;
    char **tokens = malloc(tokenBufferSize * sizeof(char*));
    char **tok;
    char * delims = " \t\r\n\a";

    tok = strtok(line, delims);
    while (tok != NULL) {
        tokens[pos] = tok;
        pos++;

        if (pos >= tokenBufferSize) {
            tokenBufferSize += TOKEN_BUFFER_SIZE;
            tokens = realloc(tokens, tokenBufferSize * sizeof(char*));
        }
        tok = strtok(NULL, delims);
    }
    tokens[pos] = NULL;
    
    return tokens;    
}

bool checkTooManyArgs(char **args) {
    if (args[2] != NULL) {
        printf("too many commands entered!\n");
        return true;
    }
    return false;
}

bool checkNoArgs(char **args) {
    if (args[1] == NULL) {
        return true;
    }
    return false;
}

bool makeProcess(char **args) {
    pid_t pid;
    pid_t wait_pid;
    int status;

    pid = fork();

    if (pid == 0) {
        // Child process
        numProcesses++;
        printf("Running child process with pid: %d\n", pid);
        if (execvp(args[0], args) < 0) {
            perror("SHELL379: ");
        }
        exit(EXIT_FAILURE);
        } else if (pid < 0) {
            // Error forking
            perror("SHELL379: ");
        } else {
            // Parent process

            // add pid to pid array
            pids[pidIndex] = pid;
            pidIndex++;
            // increment number of processes
            numProcesses++;
            printf("Running parent process with pid: %d\n", getpid());

            // if pid == (pid from a wait command entered)
            //      wait(NULL)

            do {
            wait_pid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    

    return 1;
    /*
    pid_t childPid = fork();
        
    
    if (!childPid) {
        // child process
        printf("Child:\nCurrent PID: %d and Child PID: %d\n",
               getpid(), childPid);
    } else {
        printf("### Parent ###\nCurrent PID: %d and Child PID: %d\n",
               getpid(), childPid);
    }
    return true;
    */
}

/*
void makeProcess(char **args) {
    pid_t pid;
    if (fork()) {
        // child process
        if (execvp(args[0], args) == -1) {
            perror(args[0]);
        }
        return;
    } else if (pid < 0) {
        // error forking
        perror(args[0]);
    } else {
        // parent process


    }
    return;
}
*/

bool runCommand(char **args) 
{
    bool argFlag = false;
    //bool builtinFlag = false;
    // no commands entered...
    if (args[0] == NULL) {
        return true;
    }
    
    //printf("running the command...\n");
    for (int i = 0; i < numFunctions(); i++) {
        if (strcmp(args[0], builtinNames[i]) == 0) {
            argFlag = checkTooManyArgs(args);
            //if (argFlag && strcmp(args[0], "exit") && strcmp(args[0], "jobs")) {
            // too many arguments, don't execute anything, simply run the shell again
            if (argFlag) {
                return argFlag;
            }
            //builtinFlag = true;
            return (*builtinFunc[i])(args);
        }
    }
    //builtinFlag = false;

    printf("newProcess function...\n");
    return makeProcess(args);
    // not a built in command, must be executed
    //return newProcess(args);
}

void startShell(int argc, char *argv[]) 
{
    char *line;
    char **arguments;
    bool shellRunning = true;

    while (shellRunning) {
        printf("SHELL379: ");
        line = readLine();
        arguments = splitLine(line);
        shellRunning = runCommand(arguments);

        /*
        if (exitSignal) {
            shellRunning = false;
        }
        */
        /*
        int len = sizeof(arguments)/sizeof(arguments[0]);
        printf("%d\n", sizeof(arguments));
        //printf("\n");

        printf("length of arguments array: %d\n", len);
        for (int i = 0; i < len; i++)
        {
            printf(arguments[i]);
            printf(" ");
        }
        printf("\n");

        if (strcmp(arguments[0], "exit") == 0)  // add functionality to strip spaces..
        {
            // wait until all processes are completed

            // print out total user/system time for each process
            exitCommand();
            shellRunning = false;
        } else if (strcmp(arguments[0], "jobs") == 0) 
        {
            displayJobs();
        }
        */
    }
    /*
    char command[LINE_LENGTH];
    bool shellRunning = true;
    int loopCount = 0;
    char moreCommands[5][10] = {
                                "kill",
                                "resume",
                                "sleep",
                                "suspend",
                                "wait"
                                };
    int moreCommandsLen = sizeof(moreCommands)/sizeof(moreCommands[0]);
    bool moreArgs = false;
    

    //TODO: find a way to read by LINE not each WORD...
    // if you press enter... another "SHELL379:"" should show up...
    while (shellRunning) 
    {
        printf("SHELL379: ");
        scanf ("%s", command);
        //printf("number of commands captured: %d\n", argc);
        for (int i = 0; i < moreCommandsLen; i++)
        {
            if(!strcmp(moreCommands[i], command))
            {
                printf("hit, keep reading more arguments!\n");
                moreArgs = true;
            }
        }
        if (helperText)
        {
            printf("command entered: %s\n", command);
        }
        if (strcmp(command, "exit") == 0)  // add functionality to strip spaces..
        {
            // wait until all processes are completed

            // print out total user/system time for each process
            exitCommand();
            shellRunning = false;
        } else if (strcmp(command, "jobs") == 0) 
        {
            displayJobs();
        } else if(moreArgs) {
            printf("looping again for more one more argument.\n");
        }
        if (helperText)
        {
            printf("times loop ran: %d\n\n", loopCount++);
        }
    }
    */
}
