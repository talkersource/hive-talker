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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <arpa/telnet.h>
#include <string.h>

/* process and write a string to the users socket */
void user_write( user_t *u, char *str )
{
     int r;
     string_t *buf;

     if( u == NULL )
          return;

     buf = process_output( u, str );

     u -> socket -> received_bytes += strlen( buf -> data );

     r = write_sock( u -> socket -> fd, buf -> data, strlen( buf -> data ) );

     string_free( buf );
}

/* output a string to a user */
void user_output( int ctype, user_t *from, user_t *to, char *str, ... )
{
     va_list argp;
     char *buf;

     if( to == NULL )
          return;

     va_start( argp, str );
     vasprintf( &buf, str, argp );
     va_end( argp );

     user_write( to, buf );

     free( buf );
}

/* output a string to the current user */
void command_output( char *str, ... )
{
     va_list argp;
     char *buf;

     if( universe -> current_user == NULL )
          return;

     va_start( argp, str );
     vasprintf( &buf, str, argp );
     va_end( argp );
     user_write( universe -> current_user, buf );

     free( buf );
}

/* output a string to a user */
void system_output( user_t *u, char *str, ... )
{
     va_list argp;
     char *buf;

     if( u == NULL )
          return;

     va_start( argp, str );
     vasprintf( &buf, str, argp );
     va_end( argp );
     user_write( u, buf );

     free( buf );
}

/* output a string to a group */
void group_output( int ctype, ulist_t *to, char *str, ... )
{                             
     va_list argp;
     char *buf;
     ulist_t *mover;

     va_start( argp, str );
     vasprintf( &buf, str, argp );
     va_end( argp );
       
     for( mover = to; mover; mover = mover -> next )
          if( mover -> u && mover -> u -> sys_flags & USF_LOGGED_IN )
               user_write( mover -> u , buf );

     free( buf );
}

/* output a string to a group, excluding the sender */
void group_output_ex( int ctype, user_t *from, ulist_t *to, char *str, ... )
{
     va_list argp;
     char *buf;
     ulist_t *mover;           

     va_start( argp, str );
     vasprintf( &buf, str, argp );
     va_end( argp );
           
     for( mover = to; mover; mover = mover -> next )
          if( mover -> u && 
              mover -> u -> sys_flags & USF_LOGGED_IN &&
              from -> userid != mover -> userid )
               user_write( mover -> u, buf );

     free( buf );
}

void sys_announce( char *str, ... )
{
     va_list argp;
     char *buf;

     va_start( argp, str );
     vasprintf( &buf, str, argp );
     va_end( argp );

     group_output( CSYSTEM, universe -> connected, "^Y-=> %s ^N\n", buf );

     free( buf );

}

void prompt_user( user_t *u, char *str, ... )
{
     va_list argp;
     char *buf;

     va_start( argp, str );
     vasprintf( &buf, str, argp );
     va_end( argp );
     system_output( u, "%s%c%c", buf, IAC, GA );

     free( buf );

}

void do_prompt( user_t *u )
{
     if( u -> prompt )
     {
          if( strcasecmp( u -> prompt, "off" ) )
               system_output( u, "%s%c%c", u -> prompt, IAC, GA );
     }
     else
          system_output( u, "%s%c%c", DEFAULT_PROMPT, IAC, GA );
}
