/*
 *  Projekt: TimeSentinel
 *  Gruppe: A412
 *  Email: sw1a412@student.aau.dk
 *  Studieretning: Software
 */

#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#ifndef REVISION
#define REVISION "Unknown Revision"
#endif

// Skal måske have endnu bedre navne :(
#define SECONDS_IN_DAY        86400 // Antal sekunder pr. dag
#define SECONDS_IN_HOUR       3600       // Antal sekunder pr. time 
#define SECONDS_IN_WEEK       604800     // Antal sekunder pr. uge
#define DAYS_IN_WEEK          7          // Antal dage pr. uge
#define WEEKS_IN_ADVANCE      16         // Antal uger som vagtplanen skal mindst planlægges

#define HOURS_IN_DAY          24         // Antal timer på et døgn
#define MAX_SECONDS_BETWEEN_SHIFTS_SUNDAY 126000   // Antal sekunder der skal gå mellem en vagt før et fridøgn og vagten derefter
#define MAX_SECONDS_WORK 18000           // Antal sekunder før en obligatorisk pause skal indlægges
#define MAX_HOURS_AVERAGE 37             // Antal arbejdstimer i gennemsnit pr. uge
#define MAX_SECONDS_BETWEEN_SHIFTS 39600 // Antal sekunder der skal gå mellem hver vagt

#define OUTPUT_FILENAME "error.txt"

enum CsvIndex
{
    INDEX_NAME,
    INDEX_SHIFT_START
};

enum TimestampDay
{
    TIMESTAMP_DAY,
    TIMESTAMP_MONTH,
    TIMESTAMP_YEAR,
    TIMESTAMP_HOUR,
    TIMESTAMP_MINUTE
};

enum TimeWeekDayIndex
{
    SUNDAY,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
};

typedef struct Time
{
    unsigned long long startTimeWeek;
    unsigned long long accHoursWorkedWeek;
    unsigned long long hoursWorkedWeekTotal;
} Time;

// en struct der indeholder et af csv values samt antal employees og vagter
typedef struct CsvField
{       
    unsigned int shiftCount;
    char** fieldsPerLine;
} CsvField;

typedef struct Shift
{
    unsigned long long unixStartTime;
    unsigned long long unixEndTime;
    unsigned long long TimeFromLastShift; // beskriver tiden i sekunder fra sidste vagt, hvor første vagt bliver her sat til ULONG_MAX da at det er realistisk at forvente at der ikke går så mange sekunder imellem 2 vagter

} Shift;

#define MAX_EMPLOYEE_NAME_LENGTH 1023 // verdens længste navn, worst-case scenario :)

typedef struct Employee
{   
    char employeeName[MAX_EMPLOYEE_NAME_LENGTH]; // medarbejderens navn
    Shift* shift;
} Employee;

void StartProgram(void); /*måske smart*/

/* CSV Parsing */
const char* GetInputFileName(void); // input navn på csv filen
FILE* OpenCsvFile(const char* fileName); // åbner csv filen og returner pointere til FILE structen
void PrintAssert(int expression, char* error); // bruges til at stoppe programmet i at kører samt at printe en error message ud på skærmen
CsvField* CsvFieldFromFile(FILE* csvFile); // funktion til at modtage csv string
char** ParseCsv(int numberofchars, FILE* csvFile);

int GetNumberOfLines(FILE* csvFile);
int* GetNumberOfCharactersPerLine(FILE* csvFile); /*kalkuler antallet af carachters */
Employee* AllocateStructs(CsvField* csvField); /*allokere plads for structs */
int CountTotalShifts(char** csvString); /*tæl det totale antal vagter */

void InitializeEmployeeTable(CsvField* csvField, Employee* employee); /*putter de rette felter ind de rette steder */
void InitializeEmployeeName(CsvField* csvField, Employee* employee);
void LoadShifts(CsvField* csvField, Employee* employee);
void ParseShiftForEmployee(CsvField* csvField, Employee* employee, int employeeIndex);

void DropChar(char* str, char c);  /*bruges til at fjerne det angivet bogstav i en char */
void ScanTimestamp(const char*, int*);

unsigned long long CalculateTimeWorked(int* timestamp); /*udregn antal timer og minuter en given vagt varer samt ligge det ind i shift samt employees */
void CalculateTimeBetweenShifts(Employee* employee, CsvField* csvField); /*udregner tiden mellem to vagter */
void DeallocateStructs(Employee* employee, CsvField* csvField); /*deallokere struct her */

// Moduler
void PrintError(const char* employeeName, const char* error, unsigned long long shiftTime);
void PrintUnixTimeAsDate(FILE* errorFile, time_t unixTime);

// Modul 1
void Module_Check11Hours(CsvField* csvField, Employee* employee);
bool HasNoPauseBeforeSunday(time_t current);

// Modul 2
void Module_CheckSunday(CsvField* csvField, Employee* employee);

// Modul 3
void Module_Check37Hours(CsvField* csvField, Employee* employee);
void FirstShiftCase(Employee* employee, struct tm* timeCurrent, Time* time, int i, int j);
unsigned long long CalculateShiftTime(int i, int j, Employee* employee);
void LoadCurrentAndLastShift(Employee* employee, struct tm* timeCurrent, struct tm* timeLastShift, int i, int j);
void LastShiftCase(Employee* employee, Time time, int employeeIndex);
void CheckIfNewWeekAndReset(Employee* employee, struct tm* timeCurrent, struct tm* timeLastShift, Time* time, int i, int j);
void IfNewWeekResetAndPrintError(Employee* employee, Time time, int employeeIndex);

// Modul 4
void Module_CheckPause(CsvField* csvField, Employee* employee);
int NumberofPauseNeeded(Shift* shifts, int shiftCount);
void PrintAllBreakWarnings(Employee employee, int shiftCount);

// Modul 5
void Module_Check16Weeks(CsvField* csvField, Employee* employee);

// userinteraction
void RunErrorChecks(CsvField* csvField, Employee* employee);
void PrintOptions();
void ScanInput(char* option);
int RunModule(CsvField* csvField, Employee* employee, char option);
void RunAllErrors(CsvField* csvField, Employee* employee);
