/*
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */    

#include "include/core_headers.h"
#include "ansi.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

char wandlist [] = "aAbBcCdDeEfFgGkKlLnNoOpPrRsStTuUvVwWxXyY^";

char *next_space( char *str )
{
     while( *str && *str != ' ' && *str != '\t' )
          str++;
/*        
     if( *str == ' ' || *str == '\t' )
     {
          while( *str && ( *str == ' ' || *str == '\t' ) )
               str++;
          str--;
     }
*/
     return str;
}

void strlwr( char *str )
{
     while( *str )
     {
          if( *str > 64 && *str < 91 )
               ( *str ) += 32;
          str++;
     }
}
     
/* truncate a string to the specified length, ignoring colour wands */
void str_trunc( char *str, int len )
{    
     char *c;
     int count;

     if( cstrlen( str ) > len )
     {
          c = str;

          count = 0;

          /* only allow len visible characters */
          while( count < len && *c )
          {
               if( *c == '^' )
               {
                    c++;
                    count--;
               }

               if( *c )
               {
                    c++;
                    count++;
               }
          }
          if( *c ) *c = '\0';
     }     
}

char *string_time_short( int32 secs )
{
     static char str[ 10 ];
     int32 days, hours, mins;

     days = secs / 86400;
     secs = secs % 86400;

     hours = secs / 3600;
     secs %= 3600;

     mins = secs / 60;
     secs %= 60;

     if( days )
          sprintf( str, "%2dd%2.2dh", days, hours );
     else if( hours )
          sprintf( str, "%2dh%2.2dm", hours, mins );
     else
          sprintf( str, "%2dm%2.2ds", mins, secs );

     return str;
}

char *string_time( int32 secs )
{
     static char str[ 100 ];
     int32 days, hours, mins;
     string_t *buf;

     if( secs == 0 )
     {
          strcpy( str, "no time at all" );
          return str;
     }

     buf = string_alloc( "" );

     days = secs / 86400;
     secs = secs % 86400;

     hours = secs / 3600;
     secs %= 3600;

     mins = secs / 60;
     secs %= 60;

     if( days )
     {
          string_append( buf, "%d day%s", days, ( days > 1 ) ? "s" : "" );

          if( ( hours && mins ) || ( hours && secs ) || ( mins && secs ) )
               string_append( buf, ", " );
          else if( hours || mins || secs )
               string_append( buf, " and " );
     }

     if( hours )
     {
          string_append( buf, "%d hour%s", hours, ( hours > 1 ) ? "s" : "" );

          if( mins && secs )
               string_append( buf, ", " );
          else if( mins || secs )
               string_append( buf, " and " );
     }

     if( mins )
     {
          string_append( buf, "%d minute%s", mins, ( mins > 1 ) ? "s" : "" );

          if( secs )
               string_append( buf, " and " );
     }

     if( secs )
          string_append( buf, "%d second%s", secs, ( secs > 1 ) ? "s" : "" );

     strncpy( str, buf -> data, 99 );
     str[ 99 ] = '\0';

     string_free( buf );

     return str;
}

/* count the length of a string, stripping out colour codes */
int cstrlen( char *ptr )
{
     int count = 0, b = 0;

     while ( *ptr != '\0' )
     {
          if ( *ptr == '^' )
               b = 1;
          else if ( *ptr != '^' && b )
               b = 0;
          else
               count++;
          ptr++;
     }
     return count;
}

char *paint( user_t *u, char c )
{
     static char code[ 20 ];
     int r;

     if( !( u -> flags & UF_COLOUR_ON ) )
     {
          if( c == '^' )
               return "^";

          return "";
     }

     switch( c )
     {
          case 'a' : r = 1 + ( int ) ( 7.0 * rand() / ( RAND_MAX + 1.0 ) );
                     sprintf( code, "\033[0;3%dm", r );
                     return code;
          case 'A' : r = 1 + ( int ) ( 7.0 * rand() / ( RAND_MAX + 1.0 ) );
                     sprintf( code, "\033[1;3%dm", r );
                     return code;
          case 'b' : return FG_BLUE_F;
          case 'B' : return FG_BLUE_B;
          case 'c' : return FG_CYAN_F;
          case 'C' : return FG_CYAN_B;
          case 'd' : return FG_BLACK_F;
          case 'D' : return FG_BLACK_B;
          case 'e' :
          case 'E' : return BG_RED;
          case 'f' :
          case 'F' : return BG_GREEN;
          case 'g' : return FG_GREEN_F;
          case 'G' : return FG_GREEN_B;
          case 'k' :
          case 'K' : return BLINK;
          case 'l' : return FAINT;
          case 'L' : return BOLD;
          case 'n' :
          case 'N' : return OFF;
          case 'o' :
          case 'O' : return BG_PURPLE;
          case 'p' : return FG_PURPLE_F;
          case 'P' : return FG_PURPLE_B;
          case 'r' : return FG_RED_F;
          case 'R' : return FG_RED_B;
          case 's' :
          case 'S' : return BG_BLACK;
          case 't' :
          case 'T' : return BG_YELLOW;
          case 'u' :
          case 'U' : return UNDERLINE;
          case 'v' :
          case 'V' : return BG_BLUE;
          case 'w' : return FG_GREY;
          case 'W' : return FG_WHITE;
          case 'x' :
          case 'X' : return BG_CYAN;
          case 'y' : return FG_YELLOW_F;
          case 'Y' : return FG_YELLOW_B;
          case '^' : return "^";
     }
     return "(???)";
}


string_t *process_output( user_t *u, char *str )
{
     char *c = str;
     string_t *buf = string_alloc( "" );

     while( *c )
     {
          switch( *c )
          {
               case '^' :
                    if( strchr( wandlist, *( c + 1 ) ) )
                    {
                         c++;
                         string_append( buf, paint( u, *c ) );
                    }
                    else
                         string_append( buf, "%c", *c );
                    break;

               case '\n' :
                    string_append( buf, "\r\n" );
                    break;

               default :                            
                    string_append( buf, "%c", *c );
          }
          c++;
     }

     return buf;
}

void string_append_title( user_t *u, string_t *buf, int format, char *str, ...  )
{
     int a, len;
     va_list argp;
     char *title;

     va_start( argp, str );
     vasprintf( &title, str, argp );
     va_end( argp );

     len = cstrlen( title );

     /* format = LEFT */
     string_append( buf, "^R..(^Y%s^R)", title );
     for( a = 0; a < u -> socket -> x - ( len + 5 ); a++ )
          string_append( buf, "." );

     string_append( buf, "^n\n" );

     free( title );                     

     /* TODO : RIGHT and CENTRE formats */
}

void string_append_line( user_t *u, string_t *buf )
{
     int a;

     string_append( buf, "^R" );

     for( a = 0; a < u -> socket -> x - 1; a++ )
          string_append( buf, "." );

     string_append( buf, "^n\n" );

}       

int get_index( char a )
{               
       return isalpha( a ) ? tolower( a ) - 'a' : 26;
}

#ifdef HPUX

/* HPUX doesn't seem to have vasprintf, so here it is */

#ifdef __STDC__
#define PTR void *
#else
#define PTR char *
#endif

unsigned long strtoul ();
//char *malloc ();

static int
int_vasprintf (result, format, args)
     char **result;
     const char *format;
     va_list *args;
{
  const char *p = format;
  /* Add one to make sure that it is never zero, which might cause malloc
     to return NULL.  */
  int total_width = strlen (format) + 1;
  va_list ap;

  memcpy ((PTR) &ap, (PTR) args, sizeof (va_list));

  while (*p != '\0')
    {
      if (*p++ == '%')
        {
          while (strchr ("-+ #0", *p))
            ++p;
          if (*p == '*')
            {
              ++p;
              total_width += abs (va_arg (ap, int));
            }
          else
            total_width += strtoul ((char*)p, (char **)&p, 10);
          if (*p == '.')
            {
              ++p;
              if (*p == '*')
                {
                  ++p;
                  total_width += abs (va_arg (ap, int));
                }
              else
              total_width += strtoul ((char*)p, (char **)&p, 10);
            }
          while (strchr ("hlL", *p))
            ++p;
          /* Should be big enough for any format specifier except %s.  */
          total_width += 30;
          switch (*p)
            {
            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'x':
            case 'X':
            case 'c':
              (void) va_arg (ap, int);
              break;
            case 'f':
            case 'e':
            case 'E':
            case 'g':
            case 'G':
              (void) va_arg (ap, double);
              break;
            case 's':
              total_width += strlen (va_arg (ap, char *));
              break;
            case 'p':
            case 'n':
              (void) va_arg (ap, char *);
              break;
            }
        }
    }
  *result = malloc (total_width);
  if (*result != NULL)
    return vsprintf (*result, format, *args);
  else
    return 0;
}

int
vasprintf (result, format, args)
     char **result;
     const char *format;
     va_list args;
{
  return int_vasprintf (result, format, &args);
}
#endif

void add_log( char *logfile, char *format, ... )
{                                    
     char *entry;
     FILE *fp;     
     char fullpath[ 255 ];
     char buf[ 80 ];
     struct tm *timeptr;
     time_t timeval;

     va_list argp;

     time( &timeval );
     timeptr = localtime( &timeval );
     strftime( buf, 80, "%d %b %Y - %H:%M - ", timeptr );

     va_start( argp, format );
     vasprintf( &entry, format, argp  );
     va_end( argp );  
                          
     sprintf( fullpath, "%s/logs/%s", BASE_DIR, logfile );

     fp = fopen( fullpath, "a" );
     if( !fp )
     {
          debug( 1, "Unable to open log file \'%s\'", entry );
          return;
     }
                       
     fwrite( buf, 1, strlen( buf ), fp );      
     fwrite( entry, 1, strlen( entry ), fp );      
     fputc( '\n', fp );     

     fclose( fp );

     debug( 3, "[%s] %s", logfile, entry );

     free( entry );
}

void debug( uint8 level, char *format, ... )
{
     char *str;
     ulist_t *mover;

     va_list argp;
     
     va_start( argp, format );
     vasprintf( &str, format, argp  );
     va_end( argp );  
                 
     mover = universe -> connected;

     while( mover )
     {
          if( mover -> u -> debug >= level )
               system_output( mover -> u, "^g[Debug] %s^n\n", str );

          mover = mover -> next;
     }

     free( str );
}

char *user_title( user_t *u )
{          
     static char title[] = "the new bee";

     if( u -> title )
          return u -> title;

     return title;
}
