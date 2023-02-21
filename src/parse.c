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
 *
 *      Prototypes should only be placed in here if the function should
 *      only be accessible to the core, and not the plugins, otherwise it
 *      should be placed in plugin_externs.h
 *
 */  

#include "core_headers.h"
                        
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <time.h>

void parse_user_input( user_t *u )
{
     char *str = u -> socket -> ibuf,
          *body;
     char special[10],
          *sptr = special;

     if( inactive_time( u ) > INACTIVITY_TIMEOUT )
     {
          group_output_ex( CINFORM, u, universe -> connected, "^c>^C> ^n%s reappears after %s ^C<^c<^n\n", u -> current_name, string_time( inactive_time( u ) ) );
          system_output( u, "^c>^C> ^nYou reappear after %s ^C<^c<^n\n", string_time( inactive_time( u ) ) );
     }

     if( u -> inactivemsg )
     {
          free( u -> inactivemsg );
          u -> inactivemsg = 0;
     }

     while( *str == ' ' )
          str++;
     
     if( !cstrlen( str ) )
     {
          u -> last_activity = time( 0 );
          return;
     }
     
     memset( special, 0, 10 );
     while( ( ispunct( *str ) && *str != '^' ) && ( sptr - special ) < 9 )
          *sptr++ = *str++;

     if( *special )
     {
          while( *str == ' ' )
               str++;

          if( !do_command( u, special, str, universe->command_ptrs ) )
               system_output( u, "Unknown command \'%s^n\'\n", special );
     }
     else
     {
          body = next_space( str );
          if( *body )
               *body++ = '\0';
          while( *body == ' ' )
               body++;

          if( !do_command( u, str, body, universe -> command_ptrs ) )
               system_output( u, "Unknown command \'%s^n\'\n", str );
     }
     
     u -> last_activity = time( 0 );
}
