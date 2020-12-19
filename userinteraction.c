/*
 *  Projekt: TimeSentinel
 *  Gruppe: A412
 *  Email: sw1a412@student.aau.dk
 *  Studieretning: Software
 */

#include "TimeSentinel.h"
// Her håndteres alt interaktion med brugeren og kalding af modul funktioner
void RunErrorChecks(CsvField* csvField, Employee* employee)
{

    char option;
    // her printes alle mulige fejl der kan tjekkes for
    PrintOptions();
    // do while loop således en bruger kan tjekke for samtlige fejl
    do
    {
        printf("Please enter an valid option: ");
        // her scannes data ind som her svare til den givne valmulighed
        ScanInput(&option);
    }
    while (RunModule(csvField, employee, option));
}

void PrintOptions()
{
    printf("t: 11 hour rule\n");
    printf("f: free time\n");
    printf("h: 37 Hour rule\n");
    printf("p: Pause warnings\n");
    printf("s: Sixteen weeks\n");
    printf("a: all errors and warnings\n");
    printf("o: to see all options again\n");
    printf("q: for stopping\n");
}

void ScanInput(char* option)
{
    char buf[200];
    // bruger fgets for at tage højde forr ekstra ugyldig input brugeren ved en fejl indtaster så det ikke bliver i input bufferen
    fgets(buf,200,stdin);
    *option = buf[0];
}
// funktion til at tjekke for error som returnere en hvis brugere ønsker at fortsætte programmet og nul hvis bruges ønsker at stoppe programmet
int RunModule(CsvField* csvField,Employee* employee,char option)
{
    switch (option)
    {
        case 't': remove(OUTPUT_FILENAME); Module_Check11Hours(csvField, employee); break;
        case 'f': remove(OUTPUT_FILENAME); Module_CheckSunday(csvField,employee);   break;
        case 'h': remove(OUTPUT_FILENAME); Module_Check37Hours(csvField,employee);  break;
        case 'p': remove(OUTPUT_FILENAME); Module_CheckPause(csvField, employee);   break;
        case 's': remove(OUTPUT_FILENAME); Module_Check16Weeks(csvField, employee); break;
        case 'a': remove(OUTPUT_FILENAME); RunAllErrors(csvField, employee);        break;
        case 'o': PrintOptions();                                                   return 1;
        case 'q':                                                                   return 0;
        default:  printf("not proper input");                                       return 1;
    }

    /* Vi tjekker om error filen eksistere eller ej. Hvis den gør, formodes det at der var nogle problemer.
       Hvis ikke, var der umiddelbart ingen fejl var til slutbrugeren. 
       Og nej, dette er klart ikke den bedste måde at gøre det her på. */
    FILE* errorFile = fopen(OUTPUT_FILENAME, "r");
    if (errorFile == NULL)
        printf("No errors found.\n");
    else
        fclose(errorFile);

    return 1;
}
// utility funktion til at kalde alle error funktioner
void RunAllErrors(CsvField* csvField, Employee* employee)
{
    Module_Check11Hours(csvField, employee);
    Module_CheckSunday(csvField, employee);
    Module_Check37Hours(csvField, employee);
    Module_CheckPause(csvField, employee);
    Module_Check16Weeks(csvField, employee);
}
