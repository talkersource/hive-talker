/* 
 * admin.c 
 * Plugin for administration commands
 *
 * Author: James Garlick
 */

#include "plugin_headers.h"
  
PLUGIN_NAME( "Administration" )
PLUGIN_VERSION( "0.1"  )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "galak" )

void got_admin_password( user_t *u, char *str )
{
     password_mode( u, 0 );

     if( !strcmp( str, ADMIN_PASSWORD ) )
     {
          command_output( "\nAdmin mode on\n" );
          u->sys_flags |= USF_ADMIN;
     }
     else
     {
          command_output( "\nInvalid Password!\n" );
          add_log( "admin", "Failed admin password: %s (user id %d)", u -> current_name, u -> userid );
     }
     u->func = 0;
     do_prompt( u );
}

void admin( user_t *u, int argc, char *argv[] )
{
     if( u->sys_flags & USF_ADMIN )
     {
          command_output( "Admin mode off\n" );
          u->sys_flags &= ~USF_ADMIN;
     }
     else
     {
          password_mode( u, 1 );

          command_output( "Enter administration password\n" );
          prompt_user( u, "Admin Password > " );
          u->func = got_admin_password;
     }
}

void set_shutdown( user_t *u, int argc, char *argv[] )
{
     int t;

     if( argc < 3 )
     {
          command_output( "Format : %s <time> <reason>\n", argv[ 0 ] );
          return;
     }
     t = atoi( argv[ 1 ] );
     universe -> shutdown = ( int ) time( 0 ) + t;
     universe -> shutdown_message = strdup( argv[ 2 ] );

     command_output( "\nShutdown scheduled for %s\n", string_time( t ) );
}

void abort_shutdown( user_t *u, int argc, char *argv[] )
{
     universe -> shutdown = -1;
     if( universe -> shutdown_message ) 
          free( universe -> shutdown_message );

     sys_announce( "Shutdown Aborted" );
}

void plugin_init( plugin_s *p_handle )
{            
     add_command( "admin",     admin,           p_handle, CMD_INVIS,  0 );
     add_command( "shutdown",  set_shutdown,    p_handle, CMD_ADMIN,  1 );
     add_command( "abort",     abort_shutdown,  p_handle, CMD_ADMIN,  0 );

     return;
}           


