/* gruppe navn osv.*/
/*mellemrum mellem forskellige segmenter af koden*/
#include <stdio.h> /*sortering af headers efter alfabetisk rækkefølge */
#include <stdlib.h>

#define N 100 /*defines */
enum week{/* enums, stort første bogstav, hver enum værdi skal stå på hver sin linje*/
  Monday,
  Tuesday,
  Wendsday,
  Thursday,
  Friday,
  Saturday,
  Sunday,
}; 

typedef struct shifts{ /* structs, skal være typedefed*/
    char* date; /* ved struct skal alle variabler samt arrays skrives på hver sin linje */
    char* name;
} shifts;

int n; /* globale variabler/arrays*/

void TimeBetweenShift(shifts); /* herefter prototyper, brug PascalCase ved funktioner ved både deres definition og declaration*/

int main() /* herefter main som den øverste funktion*/
{ /* turbotklammer skal stå på linjen efter funktionen, samme gælder for if statements, while loops osv.*/

    /*alle delcarationer samt intialiseringer der er statiske skal komme før reel kode*/
    shifts shift, shift1; /*variabler/structs skal blive declared på samme linje */
    int longHour; /*camelcase bruges ved variabler*/
    int n = 0, /*hvis structs/variabler bliver initialiserede skal de stå på næste linje*/
    x = 1;    /* ved indrykning skal der være 4 mellem rum */  
    int i, /* ved intialiasering/declaration af hehholdsvis  */
    arr[10];
    return EXIT_SUCCESS; /* return altid en værdi undtagens hvis der er tale om funktioner der retuner void */
}

void TimeBetweenShift(shifts shift) /* herefter funktioner, i den rækkefølge som de bliver kaldt i programmet */
{
    int x = 1;

    if (x == 1)    /*ved if statments der kun skal kører en erklæring skal tuborg klammer ikke bruges, samme med while og forloops*/ 
        return; /*Returner i funktioner med void, hvis funktionen skal stoppe baseret på en condition*/
}
