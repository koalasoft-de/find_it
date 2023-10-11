#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PASSWORDS 1000
#define MAX_LINE_LENGTH 1024

void read_passwords(char passwords[][MAX_LINE_LENGTH], int *password_count, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Could not open passwords file");
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) && *password_count < MAX_PASSWORDS)
    {
        char *newline = strtok(line, "\n");
        if (newline != NULL)
        {
            strcpy(passwords[*password_count], newline);
            printf("Read password: %s\n", passwords[*password_count]); // Debug output
            (*password_count)++;
        }
    }

    fclose(file);
}

void search_logs(const char passwords[][MAX_LINE_LENGTH], int password_count)
{
    FILE *output_file = fopen("Leaks.txt", "w");
    if (output_file == NULL)
    {
        perror("Could not open output file");
        exit(1);
    }

    for (int i = 1; i <= 5; i++)
    {
        char log_filename[20];
        sprintf(log_filename, "log.%d.txt", i);

        FILE *log_file = fopen(log_filename, "r");
        if (log_file == NULL)
        {
            perror("Could not open log file");
            continue;
        }

        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), log_file))
        {
            char *newline = strtok(line, "\n");
            if (newline != NULL)
            {
                for (int j = 0; j < password_count; j++)
                {
                    char *found = strstr(newline, passwords[j]);
                    if (found != NULL)
                    {
                        char *name_start = strstr(newline, ": ");
                        if (name_start != NULL)
                        {
                            name_start += 2;
                            char *name_end = strstr(name_start, " failed");

                            if (name_end != NULL)
                            {
                                char name[name_end - name_start + 1];
                                strncpy(name, name_start, name_end - name_start);
                                name[name_end - name_start] = '\0';
                                fprintf(output_file, "Leak found: %s | Password: %s | Log File: %s\n", name, passwords[j], log_filename);
                            }
                        }
                    }
                }
            }
        }

        fclose(log_file);
    }

    fclose(output_file);
}
int main()
{
    char passwords[MAX_PASSWORDS][MAX_LINE_LENGTH];
    int password_count = 0;

    read_passwords(passwords, &password_count, "passwords.txt");
    search_logs(passwords, password_count);

    return 0;
}
