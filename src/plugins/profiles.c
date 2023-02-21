/*
 *   User Profiles
 *
 */

#include <time.h>

#include "plugin_headers.h"
#include "user_settings.h"

PLUGIN_NAME( "User Profiles" )
PLUGIN_VERSION( "0.3"  )
PLUGIN_DEPENDENCIES( "user_settings.so" )
PLUGIN_AUTHORS_LIST( "galak" )

plugin_s *user_settings_h; /* a pointer to the user_settings plugin */

void profile_stats( user_t *u, int argc, char *argv[] )
{
     user_t *target;
     string_t *buf;
     time_t t;

     if( argc < 2 )
     {
          target = u;
     }
     else
     {
          target = user_find_on_by_name( u, argv[ 1 ] );

          if( target == NULL )
               return;
     }

     buf = string_alloc( "" );

     string_append_title( u, buf, LEFT, "User Profile for %s (id %d)", target -> current_name, target -> userid );

     if( target -> first_save_stamp )
     {
          t = ( time_t ) target -> first_save_stamp;
          string_append( buf, "      First save : %s", ctime( &t ) );
     }
     else
          string_append( buf, "      First save : Unknown\n" );

     string_append( buf, "      Login time : %s\n", string_time( login_time( target ) ) );
     string_append( buf, " Inactivity time : %s\n", string_time( inactive_time( target ) ) );

     string_append( buf, "Total login time : %s\n", string_time( target -> total_time + login_time( target ) ) );
     string_append( buf, "     Connections : %d\n", target -> total_connections );
     string_append( buf, "      Bytes sent : %d (this session),  %d (total)\n",  target -> socket -> sent_bytes, target -> socket -> sent_bytes + target -> total_bytes_in );
     string_append( buf, "  Bytes received : %d (this session),  %d (total)\n",  target -> socket -> received_bytes, target -> socket -> received_bytes + target -> total_bytes_out );

     string_append_title( u, buf, LEFT, "Page 1 - Statistics" );
     
     command_output( buf -> data );

     string_free( buf );
}

void profile_personal( user_t *u, int argc, char *argv[] )
{
     user_t *target;
     string_t *buf;
     int count;
     udpersonal_t *udata;

     if( argc < 2 )
     {
          target = u;
     }
     else
     {
          target = user_find_on_by_name( u, argv[ 1 ] );

          if( target == NULL )
               return;
     }

     udata = ( udpersonal_t * ) udata_get( target, user_settings_h, sizeof( udpersonal_t ) );
     if( udata == NULL )
          return;

     count = 0;

     buf = string_alloc( "" );

     string_append_title( u, buf, LEFT, "User Profile for %s (id %d)", target -> current_name, target -> userid );

     if( udata -> email )
     {
          string_append( buf, "     E-Mail : %s\n", udata -> email );
          count++;
     }

     if( udata -> location )
     {
          string_append( buf, "   Location : %s\n", udata -> location );
          count++;
     }

     if( udata -> irl_name )
     {
          string_append( buf, "   IRL Name : %s\n", udata -> irl_name );
          count++;
     }

     if( udata -> occupation )
     {
          string_append( buf, " Occupation : %s\n", udata -> occupation );
          count++;
     }            

     if( udata -> homepage )
     {
          string_append( buf, "   Homepage : %s\n", udata -> homepage );
          count++;
     }

     if( udata -> photo )
     {           
          string_append( buf, "      Photo : %s\n", udata -> photo );
          count++;
     }

     if( udata -> interests )
     {
          string_append( buf, "  Interests : %s\n", udata -> interests );
          count++;
     }

     string_append_title( u, buf, LEFT, "Page 2 - Personal Info" );

     if( !count )
     {                                                                     
          if( target == u )
               command_output( "You must first set some personal information with the 'set' command\n" );
          else
               command_output( "%s has not set any personal information yet\n", target -> current_name );
     }
     else
     {
          command_output( buf -> data );
     }                                  

     string_free( buf );
}

void profile( user_t *u, int argc, char *argv[] )
{
     command_output( "Format: profile <page> [<user>]\n"
                     "  Page 1 (stats)    - User statistics (shortcut 'finger' or 'f')\n"
                     "  Page 2 (personal) - Personal info   (shortcut 'examine' or 'x')\n" 
                   );
}

void plugin_init( plugin_s *p_handle )
{          
      
     user_settings_h = plugin_find( "user_settings.so" );
     if( !user_settings_h )
     {
          debug( 1, "user_settings.so not loaded! (needed by profiles.so)" );
          /* TODO - fail */
     }

     add_command(    "profile",             profile,          p_handle, CMD_INFO,   1 );
     add_subcommand( "profile", "stats",    profile_stats,    p_handle, CMD_INFO,   0 );
     add_subcommand( "profile", "personal", profile_personal, p_handle, CMD_INFO,   0 );
     add_subcommand( "profile", "1",        profile_stats,    p_handle, CMD_INFO,   0 );
     add_subcommand( "profile", "2",        profile_personal, p_handle, CMD_INFO,   0 );

     add_command( "p",       profile,          p_handle, CMD_INVIS, 1 );
     add_command( "finger",  profile_stats,    p_handle, CMD_INVIS, 0 );
     add_command( "f",       profile_stats,    p_handle, CMD_INVIS, 0 );
     add_command( "examine", profile_personal, p_handle, CMD_INVIS, 0 );
     add_command( "x",       profile_personal, p_handle, CMD_INVIS, 0 );

}


