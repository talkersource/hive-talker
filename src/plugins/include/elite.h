

typedef struct
{ 
   uint16 w0;
   uint16 w1;
   uint16 w2;
} seedtype;  /* six byte random number used as seed for planets */

typedef struct
{
   uint16 x;
   uint16 y;       /* One byte unsigned */
   uint16 economy; /* These two are actually only 0-7  */
   uint16 govtype;   
   uint16 techlev; /* 0-16 i think */
   uint16 population;   /* One byte */
   uint16 productivity; /* Two byte */
   uint16 radius; /* Two byte (not used by game at all) */
   char name[12];
} plansys ;

