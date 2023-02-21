#include <stdio.h>

/* available data types */

#define INT8   1
#define UINT8  2
#define INT16  3
#define UINT16 4
#define INT32  5
#define UINT32 6
#define CHAR   7
#define UCHAR  8
#define STRING 9

void fwrite_int8( uint16, int8 *, FILE * );
void fwrite_uint8( uint16 , uint8 *, FILE * );
void fwrite_int16( uint16 , int16 *, FILE * );
void fwrite_uint16( uint16 , uint16 *, FILE * );
void fwrite_int32( uint16 , int32 *, FILE * );
void fwrite_uint32( uint16 , uint32 *, FILE * );
void fwrite_char( uint16 , char *, FILE * );
void fwrite_uchar( uint16 , unsigned char *, FILE * );
void fwrite_string( uint16 , char *, FILE * );
void fwritew( uint16, int, void *, FILE * );

int8 fread_int8( FILE * );
uint8 fread_uint8( FILE * );
int16 fread_int16( FILE * );
uint16 fread_uint16( FILE * );
int32 fread_int32( FILE * );
uint32 fread_uint32( FILE * );
char fread_char( FILE * );
unsigned char fread_uchar( FILE * );
char *fread_string( FILE * );
     
void save_userids( );
int load_userids( );

void save_user( user_t * );
int load_user( user_t * );




