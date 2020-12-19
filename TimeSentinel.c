/*
 *  Projekt: TimeSentinel
 *  Gruppe: A412
 *  Email: sw1a412@student.aau.dk
 *  Studieretning: Software
 */

#include "TimeSentinel.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "csv_parser/csv.h"

/* Ja det er en global variabel.
   Ja det må vi ikke.   
   Nej du må ikke ændre det. */ 
unsigned int employeeCount = 0;

int main(void)
{
    printf("Revision: %s\n", REVISION);
    StartProgram();

    return EXIT_SUCCESS;
}

void StartProgram(void) 
{
    const char* fileName = GetInputFileName();

    FILE* csvFile = OpenCsvFile(fileName);
    CsvField* csvField = CsvFieldFromFile(csvFile);
    fclose(csvFile);

    Employee* employee = AllocateStructs(csvField);

    InitializeEmployeeTable(csvField, employee);
    RunErrorChecks(csvField, employee);
    DeallocateStructs(employee, csvField);
}

    /* Funktion til at promote useren for navnet på filen, hvorefter dette retuneres.
    const char* bruges her, da at char array her kun skal bruges som input efter det er indlæst. */
const char* GetInputFileName(void)
{
    static char fileName[500];
    printf("please type in file name: ");
    fgets(fileName, 500, stdin);
    strtok(fileName, "\n"); // fjerner newlinen i slutningen af char arrayet, da at filnavnet ikke indeholder en newline
    return fileName;
}

// Åbner csv filen med filnavet svarende til fileName input paramteren og returne en FILE pointer til denne fil.
FILE* OpenCsvFile(const char* fileName)
{
    FILE* csvFile = fopen(fileName, "r"); // r står for read
    PrintAssert(csvFile != NULL, "File cannot be read");
    return csvFile;
}

// Hjælpe funktion til at printe specifik fejlmeddelse ud
void PrintAssert(int expression, char* error) 
{
    if (!expression)
    {
        printf("%s", error);
        exit(EXIT_FAILURE);
    }
}
// Læser og parser csv filen og lægger informationen over i et array af CsvFields som retuneres som en pointer til det første element.
CsvField* CsvFieldFromFile(FILE* csvFile)
{
    CsvField* csvField;

    // da at hver linje burde repræsentere en medarbejder i csv filerne svarer numberOfLines her til antal medarbejdere hvor dette gemmes i et globalt variabel
    employeeCount = GetNumberOfLines(csvFile);

    // allokere et array af integer der svarer til hvor mange characters hver linje har
    int* numberOfChars = GetNumberOfCharactersPerLine(csvFile);

    // allokere array af CsvFields Structs svarende til antal medarbejder i csv filen
    csvField = malloc(employeeCount * sizeof(CsvField));
   
    // itere her over alle linjer i csv filen og splitter dem op i et array af char strenge som er opdelt ud fra en delimiter, som her er ';'
    for (int i = 0; i < employeeCount; i++)
        csvField[i].fieldsPerLine = ParseCsv(numberOfChars[i], csvFile);

    // da at antallet af carachters per linje ikke bruges i gennem resten af programmets kører tid.
    free(numberOfChars);

    //returner arrayet af struct der hver indeholder et array af csv strenge per linje som er opdelt efter den overnævnte delimiter 
    return csvField;
}

/* Funktion der læser fra csv filen og parser denne.
   numberOfChars svarer til antal bogstaver i en given linje, csvFile er en pointer til en given Csv fil */
char** ParseCsv(int numberOfChars, FILE* csvFile)           
{
    char* csvLine = calloc(numberOfChars + 1, sizeof(char));
    char** parsedLine;

    /* allokere plads til et char array med en længde svarende til længden på den givne linje i csv filen.
       Der ligges en til for at tage højde for den nulterminerede char, som bliver tilføjet af fgets ved indlæsning af tekst linen */
    fgets(csvLine, numberOfChars + 1, csvFile);
    strtok(csvLine, "\n"); 

    // dobbelt pointer som pointer til at array af char arrays som hver indeholder et csv felt fra den respektive linje
    parsedLine = parse_csv(csvLine);

    free(csvLine); // da csv lines ikke bruges andre steder end her free's dens plads også her. 
    
    return parsedLine;
}

/* Nedenstående kode er afhængig af at csvFile afsluttes med en newline
   læser newlines i filen som  her returneres som en værdi der svarer til antallet af linjer i filen. */
int GetNumberOfLines(FILE* csvFile)
{
    int numberOfCsvLines = 0;
    char ch;

    while ((ch = fgetc(csvFile)) != EOF)
    {
        if (ch == '\n')
            numberOfCsvLines++;
    }
       
    rewind(csvFile); // bruges til at sætte sætte file pointeren til starten af filen igen

    return numberOfCsvLines;
}

// Retunere et array af antallet af carachter for hver linje
int* GetNumberOfCharactersPerLine(FILE* csvFile)
{
    int n = 0;
    int* numberOfChars = malloc(sizeof(int) * employeeCount);
    char ch;

    for (int i = 0; i < employeeCount; i++)
    {
        n = 0;

        while ((ch = fgetc(csvFile)) != EOF)
        {
            n++;
            if (ch == '\n')
                break;
        } 

        numberOfChars[i] = n;
    }

    rewind(csvFile);

    return numberOfChars;
}

// her bliver både både medarbjeder og vagt structs allokeret
Employee* AllocateStructs(CsvField* csvField)
{
    int numberOfShifts = 0;

    // allokere plads for et array af medarbjeder structs der svarer til antallet af medarbjedere
    Employee* employee = malloc(sizeof(Employee) * employeeCount);

    for (int i = 0; i < employeeCount; i++)
    {
        numberOfShifts = CountTotalShifts(csvField[i].fieldsPerLine);
        employee[i].shift = malloc(numberOfShifts * sizeof(Shift)); // allokere plads for et array af vagter svarende til antallet af vagter
        
        // vi ligger antallet af vagter over i csvField Struct felte shiftCount for at kunne iterere over vagterne i senere funktioner
        csvField[i].shiftCount = numberOfShifts;
    }

    // retunere array af structs 
    return employee;
}

// Tæller her antallet af vagter 
int CountTotalShifts(char** CsvString)
{
    int n = 0;

    while (*CsvString)
    {
        /* Hvis situations tegn findes, er der tale om et medarbjeder navn og ikke en vagt. 
           der sammenlignes også med NULL da at denne indikere at der here ikke er tale om en vagt */
        if (!strchr(*CsvString, '"') && strcmp(*CsvString, "NULL"))
            n++;

        CsvString++;
    }

    // Retunere antallet af vagter for en given medarbejder
    return n;
}

// Indlæser alt data fra CsvField structs ind i medarbjeder structs 
void InitializeEmployeeTable(CsvField* csvField, Employee* employee)
{
    // indlæser her navnene ind i de inviduelle medarbejder structs
    InitializeEmployeeName(csvField, employee);
    // indlæser her tiderne for de forskellige vagter ind i vagt structs 
    LoadShifts(csvField, employee);
    // indlæser her tiden mellem vagterne ind i vagt structs    
    CalculateTimeBetweenShifts(employee, csvField);
}
// her indlæses medarbjeder navne
void InitializeEmployeeName(CsvField* csvField, Employee* employee)
{
    for (int i = 0; i < employeeCount; i++)
    {
        // navnet kopieres her over i employname feltet fra csvField struc
        strcpy(employee[i].employeeName, csvField[i].fieldsPerLine[INDEX_NAME]);

        // sitautions tegn fjernes fra navnet
        DropChar(employee[i].employeeName, '"');
    }
}

// Fjerner en given char for et given char array.
void DropChar(char* str, char c)
{
    char* pr = str;

    while (*pr)
    {
        *str = *pr++;  // putter char i pr over i str og inkrementere derefter pr
        if (*str != c) // check om char ved str ikke er ligmed den char du vil fjerne
            str++;     // hvis de ikke er lig hinaden inkrementeres str også
    }

    *str = '\0';
}

// Indlæser her tiderne ind i vagt structs
void LoadShifts(CsvField* csvField, Employee* employee)
{
    // konverter her de forskellige datoer til sekunder i unix tid og læser dem ind i vagt structs
    for (int i = 0; i < employeeCount; i++)
        ParseShiftForEmployee(csvField, employee, i);
}

void ParseShiftForEmployee(CsvField* csvField, Employee* employee, int employeeIndex)
{
    char* startTime, *endTime, *tmp;
    int timestamp[5];
 

    // itere over alle vagte for en given medarbjeder
    for (int i = 0; i < csvField[employeeIndex].shiftCount; i++)
    {
        /* tjeker om der er en vagt til stede ved den givne csvField.
           Addere her med INDEX_SHIFT_START som svarer til 1, for at skippe det første csv felt som svarer til medarbejder navnet */
        if (strcmp(csvField[employeeIndex].fieldsPerLine[i + INDEX_SHIFT_START], "NULL"))
        {
            // assigner her en midlertidig pointer til det givne csv felt der svarer til en vagt
            tmp = csvField[employeeIndex].fieldsPerLine[i + INDEX_SHIFT_START];

            // splitter her strengen op i startiden på en vagt og sluttiden på en vagt ud fra bindestregs tegnet
            startTime = strtok(tmp, "-");
            endTime = strtok(NULL, "-");

            /* scanner her tiden ind i arrayet af timestamps som svarer til det givne tidspukt
               dernæst kalkuleres starttiden i unix tid ud fra timestamp integer arrayet */
            ScanTimestamp(startTime, timestamp);
            employee[employeeIndex].shift[i].unixStartTime = CalculateTimeWorked(timestamp);

            // samme som overstående, her gøres det blot for sluttiden for en vagt   
            ScanTimestamp(endTime, timestamp);
            employee[employeeIndex].shift[i].unixEndTime = CalculateTimeWorked(timestamp);
        }
    }
}

void ScanTimestamp(const char* csvTimestamp, int* timestamp)
{   
    // indlæser her tiden fra dato tekst strengen ind i det statisk allokeret timestamp array
    sscanf(csvTimestamp,"%d/%d/%d %d.%d",
            &timestamp[TIMESTAMP_DAY],
            &timestamp[TIMESTAMP_MONTH],
            &timestamp[TIMESTAMP_YEAR], 
            &timestamp[TIMESTAMP_HOUR],
            &timestamp[TIMESTAMP_MINUTE]);
}

// her beregnes tiden i unix tid for et givent statisk allokere integer array der repræsentere et tidspunkt
unsigned long long CalculateTimeWorked(int* timestamp)
{
    struct tm UnixTime;

    // ligger her værdierne over i tm time structen 
    UnixTime.tm_sec   = 0;
    UnixTime.tm_min   = timestamp[TIMESTAMP_MINUTE];
    UnixTime.tm_hour  = timestamp[TIMESTAMP_HOUR];
    UnixTime.tm_mday  = timestamp[TIMESTAMP_DAY];
    UnixTime.tm_mon   = timestamp[TIMESTAMP_MONTH] - 1; 
    UnixTime.tm_year  = timestamp[TIMESTAMP_YEAR] - 1900;
    UnixTime.tm_isdst = 0;
    
    return mktime(&UnixTime); //bruger mk time til at returnere unix tiden ud fra struct tm
}

// Her udregnes tiden mellem 2 vagter
void CalculateTimeBetweenShifts(Employee* employee, CsvField* csvField)
{
    // Itere over antallet af medarbejder og deres individuelle vagter
    for (int i = 0; i < employeeCount; i++)
    {
        for (int j = 0; j < csvField[i].shiftCount; j++)
        {
            // Tiden mellem sluxttidspunktet på en vagt og starttidspunkt på den forrige vagt i unix tid udregnes her, hvor man så får antallet af sekunder mellem de 2 vagter
            if (j == 0)
            // ved den første vagt sættes tiden før sidste vagt til ULLONG_MAX da at der ikke er nogen vagt før den første vagt
                employee[i].shift[j].TimeFromLastShift = ULLONG_MAX; 
            else
                employee[i].shift[j].TimeFromLastShift = employee[i].shift[j].unixStartTime - employee[i].shift[j - 1].unixEndTime;
        }
    }
}
// her deallokeres alle structs da at vi her er ved programmets ende for at undgå memory leaks
void DeallocateStructs(Employee* Employee, CsvField* csvField)
{
    int i;
    
    for (i = 0; i < employeeCount; i++)
        free(Employee[i].shift);

    free(Employee);

    for (i = 0; i < employeeCount; i++)
        free_csv_line(csvField[i].fieldsPerLine);

    free(csvField);
}

/*
#######
MODULER
#######
*/

// Output for fejlmeddelser
void PrintError(const char* employeeName, const char* error, unsigned long long shiftTime)
{
    FILE* errorFile = fopen(OUTPUT_FILENAME, "a+");

    printf("%s ", error);
    fprintf(errorFile, "%s ", error);

    // er et employeeName angivet, skal det medfølges
    if (strcmp(employeeName, ""))
    {
        printf("for employee %s ", employeeName);
        fprintf(errorFile, "for employee %s ", employeeName);
    }

    // er en tid angivet, skal det medfølges
    if (shiftTime != 0)
        PrintUnixTimeAsDate(errorFile, shiftTime);
    else
    {
        // dette er nødvendigt, idéet asctime() printer en newline
        printf("\n");
        fprintf(errorFile, "\n");
    }
    
    fclose(errorFile);
}

// konverter og printer unix time til konsollen og errorFile
void PrintUnixTimeAsDate(FILE* errorFile, time_t unixTime)
{
    printf("at: %s", asctime(localtime(&unixTime)));
    fprintf(errorFile,"at: %s", asctime(localtime(&unixTime)));
}

/*
############################################
MODUL 1: 11 timers konsekutiv hvile per døgn
############################################
*/

// Funktion til at tjekke om der er 11 timer mellem hver vagt.
void Module_Check11Hours(CsvField* csvField, Employee* employee)
{
    // Her tages der højde for de forskellige medarbejdere.
    for (int i = 0; i < employeeCount; i++)
    {
        // Her tjekkes hver vagt for de forskellige medarbejdere.
        for (int j = 0; j < csvField[i].shiftCount; j++)
        {
            /* Der laves en if statement for at tjekke om tiden mellem vagterne har en hvile periode på 11 timer, mellem hinanden.
               ULLONG_MAX får ikke den samme værdi som TimeFromLastShift, da der gerne burde gå mindre tid før man får sin næste vagt */
            if (employee[i].shift[j].TimeFromLastShift < MAX_SECONDS_BETWEEN_SHIFTS &&
                    employee[i].shift[j].TimeFromLastShift != ULLONG_MAX)
            {
                PrintError(employee[i].employeeName, "less than 11 hours between shifts", employee[i].shift[j].unixStartTime);
            }
        }
    }
}

/*
#######################################################################################################################################
MODUL 2: Alle medarbejdere skal have et fridøgn (24 timers fri), hvor dette fridøgn helst skal ligge på samme dag for alle medarbejdere
#######################################################################################################################################
*/

// Funktionen her tjekker om man arbejder på en søndag og hvis man gør printer den fejlmeddelelse
void Module_CheckSunday(CsvField* csvField, Employee* employee)
{
    struct tm timeCurrent;

    // for løkke som kører for alle medarbejderne i vagtplanen
    for (int i = 0; i < employeeCount; i++)
    {
        // for løkke som kører for alle vagter en medarbejder har
        for (int j = 0; j < csvField[i].shiftCount; j++)
        {
            /* Funktionen localtime er en del at time.h og udregner Unixtime om til den specifikke dato.
               Dette gøres for starttidspunktet af vagten */
            timeCurrent = *localtime((time_t*)&employee[i].shift[j].unixStartTime);         
            
            // Hvis tm_wday er == 0 printer den en fejl da 0 er den værdi søndag har
            if (timeCurrent.tm_wday == SUNDAY)
                PrintError(employee[i].employeeName, "The start time of this shift is on a sunday", employee[i].shift[j].unixStartTime);
            else if (j != 0 && HasNoPauseBeforeSunday(employee[i].shift[j].unixEndTime))
                PrintError(employee[i].employeeName, "end time Less than 11 hours before sunday", employee[i].shift[j].unixStartTime);   
                
            // Her gøres det samme som ovenfor, bare for sluttidspunktet. Dette sikre at man ikke arbejder ind i en søndag heller.
            timeCurrent = *localtime((time_t*)&employee[i].shift[j].unixEndTime);
          
            if (timeCurrent.tm_wday == SUNDAY)
                PrintError(employee[i].employeeName, "The end time of this shift is on a sunday", employee[i].shift[j].unixEndTime);
        }
    }
}

/*  Denne funktion tjekker om man har 11 timers pause før en 24 timers pause på en søndag. 
    Dens input er den nuværende vagt der tjekkes for og den forhenværende vagt. */
bool HasNoPauseBeforeSunday(time_t current)
{
    /* Hvis tiden i Unixtid for nuværende vagt minus Unixtid for den forhenværende vagt er mindre end 35 timer,
       vil det hele blive falskt uanset hvad så snart man har pause mere end 35 timer. */
    // Disse vagter omregnes så til et dato vha. localtime funktionen og lægges over på deres respektive plads af typen struct tm.
    struct tm timeCurrent = *localtime(&current);
    
    // hvis der er tale om en lørdag og at der mindre end 11 timer til næste vagt skal denne funktion returne true og i alle andre tilfælde skal den returne false
    if( timeCurrent.tm_wday == SATURDAY && HOURS_IN_DAY-timeCurrent.tm_hour >= MAX_SECONDS_BETWEEN_SHIFTS/SECONDS_IN_HOUR)
        return true;
    else 
        return false;
}

/*
############################################################
MODUL 3: Gennemsnit 37 timer pr. uge over en 4 ugers periode
############################################################
*/

void Module_Check37Hours(CsvField* csvField, Employee* employee)
{
    Time time;
    struct tm timeCurrent;
    struct tm timeLastShift;

    // for løkke som kører for alle medarbejderne i vagtplanen.
    for (int i = 0; i < employeeCount; i++)
    {
        // Sætter alle værdier i structen til at være 0. Dette sker hver gang der et tale om en ny iteration af medarbejder løkken
        time = (struct Time){ 0 };

        // for løkke som kører for alle vagter en medarbejder har
        for (int j = 0; j < csvField[i].shiftCount; j++)
        {
            // if else som tager højde for det første tilfælde af løkken hvor der ikke findes et indeks for -1
            if (j == 0)
                FirstShiftCase(employee, &timeCurrent, &time, i, j);
            else
            {
                LoadCurrentAndLastShift(employee, &timeCurrent, &timeLastShift, i, j);
                CheckIfNewWeekAndReset(employee, &timeCurrent, &timeLastShift, &time, i, j);
                IfNewWeekResetAndPrintError(employee, time, i);

                // Assigner startTimeWeek til en ny værdi hvis det er en ny uge, som det er hvis den akkumulerede tid er lig 0
                if (time.accHoursWorkedWeek == 0)
                    time.startTimeWeek = employee[i].shift[j].unixStartTime;
            }

            // Den beregnede tid for et skift ligges over i den akkumulerede tid for ugen
            time.accHoursWorkedWeek += CalculateShiftTime(i, j, employee);
            if (j==csvField[i].shiftCount)
            {
                LastShiftCase(employee, time, i);
            }
        }
    }
}

/* Her assignes timecurrent til at blive værdien af funktionen local time kørt på den indsatte vagt som er den første i dette tilfælde. 
   Værdien er tiden omregnet vha. localtime funktionen til de forskellige enheder som en dato består.
   starttiden for ugen assignes til at være starttiden for den første vagt i dette tilfælde, da denne funktion kun kører for den allerførste vagt for en person.
   sidst lægges tiden man har arbejdet for vagten ind i den akkumulerede tid man har arbejdet i en uge. */
void FirstShiftCase(Employee* employee, struct tm* timeCurrent, Time* time, int i, int j)
{
    *timeCurrent = *localtime((time_t *)&employee[i].shift[j].unixStartTime);
    time->startTimeWeek = employee[i].shift[j].unixStartTime;
}

/* Funktionen her lægger den nuværende vagt som løkken kører for, og den forhenværende vagt ind i vores programs time struct. 
   Dette bruges til at bestemme om der tale om en ny uge for medarbejderen. */
void LoadCurrentAndLastShift(Employee* employee, struct tm* timeCurrent, struct tm* timeLastShift, int i, int j)
{   
    time_t current = employee[i].shift[j].unixStartTime;
    time_t last = employee[i].shift[j - 1].unixStartTime;
    *timeCurrent = *localtime(&current);
    *timeLastShift = *localtime(&last);
}

/* Funktionen her tjekker om der er tale om en ny uge. Dette gøres ved brugen af et if statement. 
   Hvis der er tale om en ny uge som er tm-wday for det nuværende skift enten lig 1 (1 svarer til mandag i den struct som er en del af time.h)
   eller tm_wday for det nuværende skift ikke er lig 0 (da dette er en søndag og derfor ikke ny uge) og er mindre en tm_wday for det forrige skift
   eller hvis forskellen mellem de to vagters tm_yday er større eller lig 7 (i tilfælde af at man har en hel uge man ikke arbejder). */
void CheckIfNewWeekAndReset(Employee* employee, struct tm* timeCurrent, struct tm* timeLastShift, Time* time, int i, int j)
{
    if ((timeCurrent->tm_wday == MONDAY) ||
            ((timeCurrent->tm_wday < timeLastShift->tm_wday && timeCurrent->tm_wday != SUNDAY) ||
            (employee[i].shift[j].TimeFromLastShift)/SECONDS_IN_DAY >= DAYS_IN_WEEK))
    {
        /* Hvis udtrykket er sandt så vil hoursWorkWedeekTotal assignes til værdien af den akkumulerede tid,
           derefter assignes værdien af den akkumulerede tid til at være 0. */
        time->hoursWorkedWeekTotal = time->accHoursWorkedWeek;
        time->accHoursWorkedWeek = 0;
    }
}

/* Funktionen her tjekker om man har arbejdet over det 37 timer. Dette gøres ved et logisk udtryk der kræver følgende:
   1. At den totale tid for ugen er 37 timer men der er en rest som dermed gør man har overarbejdet og der er tale om en ny uge.
   2. At den totale tid for ugen i timer er over 37 timer og der er tale om en ny uge. 
   Hvis det logiske udtryk er sandt printer den fejlmeddelelse */
void IfNewWeekResetAndPrintError(Employee* employee, Time time, int employeeIndex)
{
    if (time.accHoursWorkedWeek == 0)
    {
        if ((time.hoursWorkedWeekTotal / SECONDS_IN_HOUR == MAX_HOURS_AVERAGE && time.hoursWorkedWeekTotal % SECONDS_IN_HOUR > 0) ||
                 time.hoursWorkedWeekTotal / SECONDS_IN_HOUR > MAX_HOURS_AVERAGE)
        {
          
            PrintError(employee[employeeIndex].employeeName, "More than 37 hours", time.startTimeWeek);
        }
    }
}

/* Simpel funktion som udregner mængden af tid man har arbejdet for et skift i Unixtime, som funktionen så returnere */
unsigned long long CalculateShiftTime(int i, int j, Employee *employee)
{
    return employee[i].shift[j].unixEndTime - employee[i].shift[j].unixStartTime;
}

void LastShiftCase(Employee* employee, Time time, int employeeIndex)
{
    time.accHoursWorkedWeek = 0;
    IfNewWeekResetAndPrintError(employee, time, employeeIndex);
}

/*
###################################################################
MODUL 4: Mellem 30-60 minutters pause efter 5 en halv times arbejde
###################################################################
*/

void Module_CheckPause(CsvField* csvField, Employee* employee)
{
    for (int i = 0; i < employeeCount; i++)
    {
        if (csvField[i].shiftCount>NumberofPauseNeeded(employee[i].shift, csvField[i].shiftCount))
            PrintAllBreakWarnings(employee[i], csvField[i].shiftCount);
        else
            PrintError(employee[i].employeeName, "needs break at all shifts", 0);
    }
}

int NumberofPauseNeeded(Shift* shift, int shiftCount)
{
    int count = 0;
    for (int j = 0; j < shiftCount; j++)  
        if ((shift[j].unixEndTime - shift[j].unixStartTime) >= MAX_SECONDS_WORK)
            count++;

    return count;
}

void PrintAllBreakWarnings(Employee employee, int shiftCount)
{
    for (int j = 0; j < shiftCount; j++) 
    {
        if ((employee.shift[j].unixEndTime - employee.shift[j].unixStartTime) >= MAX_SECONDS_WORK)
            PrintError(employee.employeeName, "break needed", employee.shift[j].unixStartTime);
    }
}

/*
#######################################################
MODUL 5: Om vagtplanen er minimum planlagt 16 uger frem
#######################################################
*/

void Module_Check16Weeks(CsvField* csvField, Employee* employee)
{
    time_t shiftDiff;
    unsigned long long startTime, endTime;

    // for løkke som kører for alle medarbejderne i vagtplanen.
    for (int i = 0; i < employeeCount; i++)
    {
        // for løkke som kører alle vagter igennem udfra hvilken medarbejder man er ved i den ydre for løkke.
        for (int j = 0; j < csvField[i].shiftCount; j++)
        {
            if (j == 0) //Tager start tidspunktet for den første vagt, lægges i lokal variable
                startTime = employee[i].shift[i].unixStartTime;

            if (j == csvField[i].shiftCount-1) //Tager starttidspunktet for den sidste vagt, lægges i lokal variable
                endTime = employee[i].shift[i].unixStartTime;
        }
        
        /* Forkellen mellem start og slut tiderne deles med defineret variable for sekunder på en uge. 
           Dermed bliver antal uger der er planlagt udregnet */
        shiftDiff = (endTime - startTime) / SECONDS_IN_WEEK;

        // Print funktion kaldes, HVIS der ikke er 16 uger planlagt
        if (shiftDiff < WEEKS_IN_ADVANCE)
        {
            PrintError(employee[i].employeeName, "ERROR: There has NOT been planned for 16 weeks", 0);
            return;
        }
    }
}
