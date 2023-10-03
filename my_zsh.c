#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "readline.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include "stringedits.h"
#include "which&execve.h"
#include <signal.h>
// echo, cd, setenv, unsetenv, env, xt, pwd and which

char **setParsingValues()
{
    char **values = malloc(sizeof(char) * 8);

    for (int i = 0; i < 8; i++)
    {
        values[i] = (char *)malloc(sizeof(char) * 8);
    }
    values[0] = "echo";
    values[1] = "cd";
    values[2] = "setenv";
    values[3] = "unsetenv";
    values[4] = "env";
    values[5] = "exit";
    values[6] = "pwd";
    values[7] = "which";
    values[8] = NULL;

    return values;
}
int shouldInterruptTask = 0;

void sigintHandler(int signum)
{
    printf("\nCtrl+C (SIGINT) received. Ongoing task will be interrupted.\n");

    shouldInterruptTask = 1;
    (void)signum;
}
void sigquitHandler(int signum)
{
    (void)signum;
    // Handle Ctrl+\ (Ctrl+D) as the end of input and exit gracefully.
    printf("\nCtrl+\\ (SIGQUIT) received. Exiting...\n");
    exit(0);
}

// Signal handler for segmentation fault (SIGSEGV)
void sigsegvHandler(int signum)
{
    my_puts("Segmentation fault (SIGSEGV) received. Exiting...\n");
    (void)signum;
    return;
}

int my_setenv(const char *name, const char *value, int overwrite)
{
    if (name == NULL || name[0] == '\0' || strchr(name, '=') != NULL)
    {
        return -1;  // Invalid name or contains '='
    }

    // Check if the variable already exists
    if (!overwrite && getenv(name) != NULL)
    {
        my_puts(
            "Variable exists, please add overwrite( 1) to the command, if you "
            "wish to do so");
        return 0;  // Variable exists, and we're not allowed to overwrite it
    }

    // Construct the environment variable string
    size_t name_len = strlen(name);
    size_t value_len = strlen(value);
    size_t var_len =
        name_len + value_len + 2;  // +1 for '=' and +1 for NULL terminator
    char *env_var = (char *)malloc(var_len);
    if (env_var == NULL)
    {
        return -1;  // Memory allocation error
    }

    strcpy(env_var, name);
    env_var[name_len] = '=';
    strcpy(env_var + name_len + 1, value);

    if (putenv(env_var) != 0)
    {
        free(env_var);
        perror("Error setting the environment variable");
        return -1;
    }

    return 0;  // Success
}

int my_unsetenv(const char *name, char **env)
{
    if (name == NULL || name[0] == '\0' || strchr(name, '=') != NULL)
    {
        my_puts("Invalid name or contains = ");
        return -1;
    }

    // Iterate through the environment variables
    int index = 0;
    while (env[index] != NULL)
    {
        char *env_var = env[index];
        // Check if the current environment variable starts with the given name
        if (strncmp(env_var, name, strlen(name)) == 0 &&
            env_var[strlen(name)] == '=')
        {
            // Shift the environment variables to remove the one to unset
            for (int i = index; env[i] != NULL; i++)
            {
                env[i] = env[i + 1];
            }
            return 0;  // Success
        }
        index++;
    }

    return 0;  // Variable not found, consider it unset
}

int isBuiltin(char *command, char **bltn)
{
    int i = 0;

    while (bltn[i] != NULL)
    {
        if (strcmp(bltn[i], command) == 0)
        {
            return i;
        }
        i++;
    }
    return -1;
}

void runBuiltins(int bct, char **userInput, char **env)
{
    char cwd[PATH_MAX];
    int i = 0;
    char *res;
    switch (bct)
    {
        case 0:
            for (int i = 1; userInput[i]; i++)
            {
                my_puts(userInput[i]);
                my_putchar(' ');
            }
            my_putchar('\n');
            break;

        case 1:
            // cd
            if (chdir(userInput[1]) != 0)
            {
                perror("no such directory");
            }
            // chdir(userInput[2]);
            break;

        case 2:

            if (userInput[3] != NULL)
            {
                i = atoi(userInput[3]);
            }
            else
            {
                i = 0;
            }
            my_setenv(userInput[1], userInput[2], i);
            break;

        case 3:
            my_unsetenv(userInput[1], env);
            break;

        case 4:
            while (env[i])
            {
                puts(env[i]);
                // putchar('\n');
                i++;
            }
            break;

        case 5:

            exit(0);
            break;

        case 6:
            // pwd

            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                puts(cwd);
            }
            else
            {
                perror("there was an error getting your path");
                exit(0);
            }
            break;

        case 7:
            res = whichImp(env, userInput, 1);
            puts(res);
            free(res);
            res = NULL;
            // which
            break;
    }
    // echo, cd, setenv, unsetenv, env, xt, pwd and which
}

int main(int ac, char **av, char **env)
{
    (void)ac;
    (void)av;

    int fd = 0;
    int bct = 0;  // builtins count
    char *userInput;
    char **builtins = setParsingValues();

    // Register signal handlers
    signal(SIGINT, SIG_IGN);  // Ignore SIGINT (Ctrl+C)
    signal(SIGSEGV, SIG_DFL); // Default behavior for SIGSEGV
    signal(SIGQUIT, SIG_IGN); // Ignore SIGQUIT (Ctrl+\)

    while (1)
    {
        write(1, "GTN - zsh ", 10);

        userInput = (char *)malloc(sizeof(char) * 1024); // Adjust the buffer size as needed

        userInput = my_readline(fd);
        if (userInput == NULL)
        {
            my_puts("\nCtrl+D (EOF) received. Exiting...\n");
            exit(0);
        }
        else if (userInput[0] == '\0')
        {
            continue;
        }

        userInput = removeSpace(userInput);
        userInput = removeBackN(userInput);

        char **line = mySplit(userInput, ' ');
        bct = isBuiltin(line[0], builtins);

        if (bct >= 0)
        {
            runBuiltins(bct, line, env);
        }
        else
        {
            if (whichImp(env, line, 0) != NULL)
            {
                
                execMD(env, line);
            }
        }

        free(userInput);
        userInput = NULL;
        freeStringArray(line);
    }

    freeStringArray(builtins);
    return 0;
}