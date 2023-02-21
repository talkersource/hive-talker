/*
 *   purpose : string library
 *
 *   authors : James Garlick
 *          
 * 
 */ 
#include <stdlib.h>
#include <stdarg.h>  
#include <string.h>

#include "core_headers.h"

#define BLOCKSIZE 10

#define CALLOC( x ) ( char * ) calloc( 1, x )

#define REALLOC( ptr, size ) ( char * ) realloc( ptr, size )

string_t *string_alloc( char *format, ... )
{
     int len;
     int blocks;
     string_t *new;
     char *str;

     va_list argp;
     
     va_start( argp, format );
     vasprintf( &str, format, argp  );
     va_end( argp );  

     len = strlen( str );

     blocks = ( len / BLOCKSIZE ) + 1;

     new = ALLOC( 1, string_t );

     if( !new ) return NULL;
                               
     new -> data = CALLOC( blocks * BLOCKSIZE );
     if( !new -> data ) { FREE( new ); return NULL; }

     strcpy( new -> data, str );

     new -> allocated = blocks * BLOCKSIZE;

     free( str );
     return new;
}

void string_free( string_t *target )
{
     FREE( target -> data );
     FREE( target );     
} 

void string_append( string_t *target, char *format, ... )
{       
     char *new;
     int required;

     va_list argp;
     
     va_start( argp, format );
     vasprintf( &new, format, argp  );
     va_end( argp );  

     required = ( strlen( target -> data ) + strlen( new ) ) - target -> allocated;

     if( required >= 0 )
     {
          // round to the nearest block
          required = ( required / BLOCKSIZE ) + 1;
          required = required * BLOCKSIZE;                  

          target -> data = REALLOC( target -> data, target -> allocated + required );
          target -> allocated += required;
     }

     strcat( target -> data, new );
                   
     free( new );
}

void string_insert( string_t *target, int pos, char *format, ... )
{
     char *new;
     int required;
     int a;     
     int padding;
     int oldlen;
     int newlen;

     va_list argp;
     
     va_start( argp, format );
     vasprintf( &new, format, argp  );
     va_end( argp );  

     oldlen = strlen( target -> data );
     newlen = strlen( new );

     padding = 0;
     if( pos > oldlen ) 
          padding = pos - oldlen;

     required = ( oldlen + newlen + padding ) - target -> allocated;

     if( required >= 0 )
     {
          // round to the nearest block
          required = ( required / BLOCKSIZE ) + 1;
          required = required * BLOCKSIZE;

          target -> data = REALLOC( target -> data, target -> allocated + required );
          target -> allocated += required;
     }                   

     if( pos <= oldlen ) 
     {
          for( a = oldlen; a > ( pos - 1 ); a-- )
               *( target -> data + a + newlen ) = *( target -> data + a );
     }
     else
     {
          for( a = oldlen; a < pos; a++ )
               *( target -> data + a ) = ' ';
          *( target -> data + pos + newlen ) = 0;
     }

     memcpy( target -> data + pos, new, newlen );

     free( new );
}

void string_remove( string_t *target, int start, int len )
{    
     int a;
     int oldlen;
           
     oldlen = strlen( target -> data );

     if( start + len > oldlen )
          string_truncate( target, start );
     else if( start < oldlen )
          for( a = start + len; a < oldlen + 1; a++ )
               *( target -> data + a - len ) = *( target -> data + a);
}

void string_truncate( string_t *target, int len )
{
     *( target -> data + len ) = 0;
}

string_t* string_dup( string_t *source )
{
     return string_alloc( source -> data );
}
          
