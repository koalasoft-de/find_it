#include <stdio.h>  //C-Standardlibrary und ermöglich die Nutzung von "scanf()" und "printf()" (input-, output-Operationen)
#include <stdlib.h> //Diese Library fügt viele nützliche Makros/Funktionen hinzu und erlaubt es leichter den Speicher zu verwalten (Ohne diese Lib wäre der leakCheck etwa doppelt bis dreimal so groß)
#include <string.h> //Hiermit werden einige String-Manipulations Methoden hinzugefügt.

#define MAX_PASSWORDS 1000   // Die maximale Anzahl von Passwoertern, welche gespeichert werden, wird "global" festgelegt. Diese Variable wird für nachfolgende Funktionen gebraucht und wird hiermit schon im Vorfeld definiert.
#define MAX_LINE_LENGTH 1024 // Die maximale Anzahl von Chars wird pro Zeile festgelegt.

// Diese Funktion kriegt den Pointer des filename übergeben und speichert die Zeilen in einem char-Array
void read_passwords(char passwords[][MAX_LINE_LENGTH], int *password_count, const char *filename)
{
    FILE *file = fopen(filename, "r"); // Die File mit dem übergebenen Namen/Adresse wird im "read" Modus geoeffnet
    if (file == NULL)                  // Sollte die file nicht gefunden worden sein, oder ein Fehler bei der Uebergabe passiert sein, wird ein Fehler ausgegeben.
    {
        perror("Could not open passwords file");
        exit(1); // Es gibt eine Konvention, dass eine "0" für erfolg steht und ungleich "0" einen unerwarteten Programmausgang oder Fehler bedeutet.
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
