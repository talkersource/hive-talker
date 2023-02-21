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
 *      main.c
 *            
 *      Authors : James Garlick
 *      Purpose : main program, initialisation
 *
 */    
                   
#include <stdio.h>
#include <stdlib.h>  
#include <signal.h>
#include <ctype.h>
#include <sys/stat.h>

#ifdef HPUX
#include <time.h>    
#endif
#ifdef LINUX
#include <sys/time.h>
#endif

#include "core_headers.h"

universe_t *universe;
 
void universe_init( char *bin_name )
{        
     FILE *f;
     char str[ 255 ];
     char *c;
     struct stat sbuf;

     universe = ALLOC( 1, universe_t );

     sprintf( str, "%s/bin/%s", BASE_DIR, bin_name );
     stat( str, &sbuf );

     universe -> compile_time = sbuf.st_mtime;

     universe -> shutdown = -1;

     if( !load_userids( ) )
          sig_die( "Unable to continue\n" );

     handling_error = 0;

     /* load default plugins */
     f = fopen( BASE_DIR "/bin/default_plugins", "r" );
     if( f )
     {
          while( fgets( str, 255, f ) )
          {
               c = str;
               while( *c != '\n' && *c )
                    c++;
               *c = 0;
              
               c = str;
               while( isspace( *c ) ) c++;
               
               if( *c && *c != '#' )
                    plugin_register( c );
          }
          
          fclose( f );
     }
     else
          printf( "WARNING: NO default_plugins FILE!!\n" );

}    

void universe_fini( )
{
     ulist_t *mover;

     mover = universe -> connected;

     while( mover )
     {    
          if( mover -> u -> sys_flags & USF_SAVED )
               save_user( mover -> u );
          mover = mover -> next;
     }
}

     
int main( int argc, char *argv [] )
{
        printf( "\n\n%s version %s booting...\n\n", SERVER_NAME, VERSION );

        universe_init( argv[ 0 ] );

        sig_init( );

        add_core_commands( );

        bind_server( PRIMARY_PORT );

        while( !( universe -> sys_flags & SHUTDOWN ) )
                serve_connections( );

        universe_fini( );
        return 0;
}
