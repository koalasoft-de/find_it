#include <stdio.h>  //C-Standardlibrary und ermöglich die Nutzung von "scanf()" und "printf()" (input-, output-Operationen)
#include <stdlib.h> //Diese Library fügt viele nützliche Makros/Funktionen hinzu und erlaubt es leichter den Speicher zu verwalten (Ohne diese Lib wäre der leakCheck etwa doppelt bis dreimal so groß)
#include <string.h> //Hiermit werden einige String-Manipulations Methoden hinzugefügt.

#define MAX_PASSWORDS 1000   // Die maximale Anzahl von Passwoertern, welche gespeichert werden, wird "global" festgelegt. Diese Variable wird für nachfolgende Funktionen gebraucht und wird hiermit schon im Vorfeld definiert.
#define MAX_LINE_LENGTH 1024 // Die maximale Anzahl (-1) von Chars wird pro Zeile festgelegt.

// Diese Funktion kriegt den Pointer des filename übergeben und speichert die Zeilen in einem char-Array
void read_passwords(char passwords[][MAX_LINE_LENGTH], int *password_count, const char *filename)
{
  FILE *file = fopen(filename, "r"); // Die File mit dem übergebenen Namen/Adresse wird im "read" Modus geoeffnet
  if (file == NULL)                  // Sollte die file nicht gefunden worden sein, oder ein Fehler bei der Uebergabe passiert sein, wird ein Fehler ausgegeben.
  {
    perror("Could not open passwords file");
    exit(1); // Es gibt eine Konvention, dass eine "0" für erfolg steht und ungleich "0" einen unerwarteten Programmausgang oder Fehler bedeutet.
  }

  char line[MAX_LINE_LENGTH];                                                // Die maximale Char-Länge einer Line wird festgelegt mit dem definierten Wert dafür.
  while (fgets(line, sizeof(line), file) && *password_count < MAX_PASSWORDS) // Eine Schleife, welche die file durchläuft solange die Line nicht mehr als 1024 Zeichen enthält und die Password-Liste maximal 1000 Eintraege enthält.
  {
    char *newline = strtok(line, "\n"); // Aus der string.h library wird strtok verwendet um den String am Ende einer Zeile zu unterbrechen (Es wird ein '\0' am Ende der Zeile eingefügt und ein Pointer an den Anfang der Zeile zurückgegeben)
    if (newline != NULL)                // Solange die "newline nicht leer ist wird der if-Block ausgeführt"
    {
      strcpy(passwords[*password_count], newline);               // Das Passwort wird von der newline in den password_count kopiert. Wobei der Iterator den Pointer stellt um die richtige Stelle im Array zu finden.
      printf("Read password: %s\n", passwords[*password_count]); // Ausgabe in die Konsole
      (*password_count)++;                                       // Iteration
    }
  }

  fclose(file); // Datei wird geschlossen
}
// Diese Funktion durchsucht nun die Logs nach matches für den passwords-Array. Es wird die maximale Zeilenlänge festgelegt und die Anzahl der Passwörter für zu durchsuchende Durchgänge mitgeteilt.
void search_logs(const char passwords[][MAX_LINE_LENGTH], int password_count)
{
  FILE *output_file = fopen("leaked.txt", "w"); // Es wird die Datei leaked.txt erstellt oder überschrieben und als output_file festgelegt. "w" steht hier für write.
  if (output_file == NULL)                     // Sollte etwas schiefgehen oder die Datei nicht existieren geht es ab in den if-Block
  {
    perror("Could not open output file"); // Error wird in die Konsole geschrieben und Programm mit 1 beendet.
    exit(1);
  }

  for (int i = 1; i <= 5; i++) // Da wir den Namen der Logs kennen und wieviele es zu durchlaufen gilt, werden 5 Durchgänge festgelegt.
  {
    char log_filename[20];                  // Es wird eine Variable erzeugt, welche bis zu 20 Zeichen haben kann.
    sprintf(log_filename, "log.%d.txt", i); // Es wird ein String erzeugt, welcher in log_fielname geschrieben wird, den Iterator übergeben bekommt und an diesen an die Stelle "%d" einsetzt. Bei i = 1 -> log_filename = "log.1.txt".

    FILE *log_file = fopen(log_filename, "r"); // Es wird die Datei log_filename übergeben und mit "r" gelesen. Der Inhalt wird in log_file gespeichert.
    if (log_file == NULL)                      // Ist log_file leer gehen wir wieder in den if-Block
    {
      perror("Could not open log file"); // Fehler wird in die Konsole ausgegeben
      continue;                          // Rest wird übersprungen und nächste Iteration begonnen
    }

    char line[MAX_LINE_LENGTH];                 //
    while (fgets(line, sizeof(line), log_file)) // Wenn die log_file vorhanden ist, wird die Schleife durchlaufen
    {
      char *newline = strtok(line, "\n"); // Am Ende der Zeile wird der String für die newline gekappt
      if (newline != NULL)                // newline vorhanden? dann gib ihm
      {
        for (int j = 0; j < password_count; j++) // 5mal durchlaufen
        {
          char *found = strstr(newline, passwords[j]); // Wenn das Passwort in newline vorkommt, wird das Ergebnis in found gespeichert
          if (found != NULL)                           // found ist nicht leer?
          {
            char *name_start = strstr(newline, ": "); // Der Start wird festgelegt. Hier Beginn bei ": "
            if (name_start != NULL)
            {
              name_start += 2;                                // Es sollen zwei Stellen übersprungen werden. Array von Chars etc
              char *name_end = strstr(name_start, " failed"); // Mit der Variable name_end wird der Anfang des Vorkommens von " failed" gespeichert

              if (name_end != NULL)
              {
                char name[name_end - name_start + 1];                                                                     // Differenz der Variablen wird berechnet und eine Stelle für "\0" hinzugefügt um das Ende der Zeichenkette zu markieren
                strncpy(name, name_start, name_end - name_start);                                                         // name ist das Zielarray, name_Start ist Anfang der Quellzeichenkette und name_end - name_start ergibt die Anzahl der zu kopierenden Zeichen an
                name[name_end - name_start] = '\0';                                                                       // Das Ende des name-Arrays wird berechnet und das Null-Terminierungszeichen hinzugefügt.
                fprintf(output_file, "Leak found: %s | Password: %s | Log File: %s\n", name, passwords[j], log_filename); // Wir schreiben in die output_file
              }
            }
          }
        }
      }
    }

    fclose(log_file); // log_file wird geschlossen
  }

  fclose(output_file); // output_file wird geschlossen
}
int main() // Startpunkt unseres Programmes. Von hier werden die Funktionen aufgerufen
{
  char passwords[MAX_PASSWORDS][MAX_LINE_LENGTH]; // Ein 2D-Char-Array wird erstellt
  int password_count = 0;                         // Integer-Variable deklariert und mit 0 initialisiert

  read_passwords(passwords, &password_count, "passwords.txt"); // Die Funktion read_passwords wird mit dem leeren passwords-Array, dem password_count und einem filename gefüttert
  search_logs(passwords, password_count);                      // Die Funktion search_logs wird mit den befüllten Arrays ausgefüllt.

  return 0; // Der Wert 0 wird als "erfolgreich" zurückgegeben und das Programm ist durchlaufen.
}