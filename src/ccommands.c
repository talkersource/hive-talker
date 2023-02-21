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
 *      ccommands.c
 *            
 *      Purpose : core commands
 *
 *      Authors : James Garlick
 */                  

#include <stdio.h>  
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "core_headers.h"

void lp( user_t *u, int argc, char *argv[] )
{                         
     if( argc < 2 )
     {
          command_output( "Format: %s <plugin name>\n", argv[ 0 ] );
          return;
     }

     if( plugin_register( argv[ 1 ] ) == 0 )   
     {
          debug( 4, "Plugin \'%s\' loaded by %s", argv[ 1 ], u -> current_name );
          command_output( "Plugin Loaded\n" );
     }
     else
          command_output( "Failed\n" );
}

void up( user_t *u, int argc, char *argv[] )
{
     plugin_s *found;

     if( argc < 2 )
     {
          command_output( "Format: %s <plugin name>\n", argv[ 0 ] );
          return;
     }

     found = plugin_find( argv[ 1 ] );
     if( !found )
     {
          command_output( "Unknown plugin \'%s\'\n", argv[ 1 ] );
          return;
     }

     if( plugin_remove( found ) == 0 )
     {
          debug( 4, "Plugin \'%s\' unloaded by %s", argv[ 1 ], u -> current_name );
          command_output( "Plugin Unloaded\n" );
     }
     else
          command_output( "Failed\n" );
}

void rp( user_t *u, int argc, char *argv[] )
{
     plugin_s *found;

     if( argc < 2 )
     {
          command_output( "Format: %s <plugin name>\n", argv[ 0 ] );
          return;
     }

     found = plugin_find( argv[ 1 ] );
     if( !found )
     {
          command_output( "Unknown plugin \'%s\'\n", argv[ 1 ] );
          return;
     }

     if( plugin_reload( found ) == 0 )
     {
          debug( 4, "Plugin \'%s\' reloaded by %s", argv[ 1 ], u -> current_name );
          command_output( "Plugin Reloaded\n" );
     }
     else
          command_output( "Failed\n" );
}
        
/* list loaded plugins */
void plugins( user_t *u, int argc, char *argv[] )
{
     plugin_s *mover;
     string_t *buf;          

     mover = plugin_head;

     if( mover == NULL )
     {
          command_output( "There are no plugins currently loaded\n" );
          return;
     }           
                   
     buf = string_alloc( "" );

     string_append_title( u, buf, LEFT, "Loaded Plugins" );

     while( mover )
     {            
          string_append( buf, " %-21.21s %-16.16s v%-8.8s %s",
                         mover -> name,           
                         mover -> path,
                         mover -> version,
                         ctime( &( mover -> loaded ) )
                       );
          mover = mover -> next;
     }

     string_append_line( u, buf );

     command_output( buf -> data );

     string_free( buf );
}            

void ep( user_t *u, int argc, char *argv[] )
{
     plugin_s *found;
     string_t *buf;
     char **mover;
                                
     if( argc < 2 )
     {
          command_output( "Format: %s <plugin name>\n", argv[ 0 ] );
          return;
     }
     
     found = plugin_find( argv[ 1 ] );
     if( !found )
     {
          command_output( "Unknown plugin \'%s\'\n", argv[ 1 ] );
          return;
     }

     buf = string_alloc( "" );

     string_append_title( u, buf, LEFT, "%s Plugin", found -> name );
                              
     string_append( buf, "     Version : %s\n", found -> version );
     string_append( buf, "        Path : %s\n", found -> path );
     string_append( buf, "    Compiled : %s", ctime( &( found -> compiled ) ) );
     string_append( buf, "      Loaded : %s", ctime( &( found -> loaded ) ) );
             
     string_append( buf, "Dependencies : " );
     mover = found -> dependencies;

     if( !mover || !*mover )
     {
          string_append( buf, "None\n" );
     }
     else
     {
          while( *mover )
          {            
               if( mover != found -> dependencies )
                    string_append( buf, ", " );
               string_append( buf, *mover );
               mover++;
          }
          string_append( buf, "\n" );
     }

     string_append( buf, "Authors List : " );
     mover = found -> authors_list;

     if( !mover || !*mover )
     {
          string_append( buf, "None\n" );
     }
     else
     {
          while( *mover )
          {            
               if( mover != found -> authors_list )
                    string_append( buf, ", " );
               string_append( buf, *mover );
               mover++;
          }
          string_append( buf,"\n" );
     }
     string_append_line( u, buf );

     command_output( buf -> data );

     string_free( buf );

}

void add_core_commands( )
{                                    

     add_command( "lp",        lp,              NULL, CMD_ADMIN,  0 );
     add_command( "up",        up,              NULL, CMD_ADMIN,  0 );
     add_command( "rp",        rp,              NULL, CMD_ADMIN,  0 );

     add_command( "plugins",   plugins,         NULL, CMD_SYSTEM, 0 );

     add_command( "ep",        ep,              NULL, CMD_SYSTEM, 0 );

}


