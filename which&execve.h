
int getOS()
{
    struct utsname info;
    int osCode;
    if (uname(&info) != -1)
    {
        if (strcmp(info.sysname, "Windows") == 0)
        {
            // Windows-specific code
            osCode = 1;
            // printf("Operating System Code: %d\n", osCode);
        }
        else
        {
            // Non-Windows code
            osCode = 0;

            // printf("Operating System Code: %d\n", osCode);
        }
    }
    else
    {
        // Error handling
        puts("Failed to determine the operating system.\n");
        exit(0);
    }

    return osCode;
}

char *whichImp(char **env, char **userInput,int m)  //  "m" for  mode (if 'which' search for 2nd word in the                                             
{                                                   //  userInput, else search for command starting at 0)
    DIR *dir;                                      
    struct dirent *entry;
    char **line;
    int i = 0;
    // printf("check");
    char splitter;
    char *slash;
    if (getOS() == 1)
    {
        splitter = ';';
        slash = "\\";
    }
    else
    {
        splitter = ':';
        slash = "/";
    }
    while (1)
    {
        line = mySplit(env[i], '=');
        if (strcmp(line[0], "PATH") == 0)
        {
            break;
        }

        freeStringArray(line);
        i++;
    }

    char **pathBank = mySplit(line[1], splitter);
    char *temp;
    int found = 0;
    char *res;

    for (int i = 0; pathBank[i] != NULL; i++)
    {
        // Open the directory
        dir = opendir(pathBank[i]);
        if (dir == NULL)
        {
            // Directory opening failed, continue to the next iteration
            continue;
        }

        // Read directory entries
        while ((entry = readdir(dir)) != NULL)
        {
            temp = malloc(sizeof(char) * strlen(userInput[m]));
            strcpy(temp, userInput[m]);
            if (getOS() == 1)
            {
                // If j is greater than 0, it means it's the second iteration,
                // so modify the search term
                free(temp);
                temp = NULL;
                temp = malloc(sizeof(char) * strlen(userInput[m]) + 5);
                strcpy(temp, userInput[m]);
                strcat(temp, ".exe");
            }


            // Compare the search term with the current entry name
            if (strcmp(temp, entry->d_name) == 0)
            {

                res = (char *)malloc(
                    sizeof(char) * (strlen(pathBank[i]) + strlen(temp)) + 2);
                strcpy(res, pathBank[i]);
                strcat(res, slash);
                strcat(res, temp);

                found++;
                break;
            }
            free(temp);
            temp = NULL;

            if (found > 0)
            {
                break;
            }
        }

        // Close the directory
        closedir(dir);

        if (found > 0)
        {
            break;
        }
    }

    if (found > 0)
    {
        return res;
    }
    else
    {
        perror("Search failed");
        return NULL;
    }
}
int execMD(char **env, char **userInput)
{
    char *path;
    if ((path = whichImp(env, userInput, 0)) == NULL)
    {
        return -1;
    }

    char **args = userInput;  // The arguments to be passed to the executable,
                              // including the command itself
    args[0] = path;           // Set the command as the path to the executable

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Fork failed");
        return -1;
    }
    else if (pid == 0)
    {
        // Child process
        execve(path, args, env);
        perror("Execve failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            int exit_status = WEXITSTATUS(status);
            return exit_status;
        }
        else
        {
            // Child process did not terminate normally
            return -1;
        }
    }
}