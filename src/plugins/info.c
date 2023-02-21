#include <signal.h>             

#include "plugin_headers.h"

PLUGIN_NAME( "Global Info" )
PLUGIN_VERSION( "0.2"  )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "galak" )

void whot( user_t *u, int argc, char *argv[] )
{
     ulist_t *mover = universe -> connected;
     user_t *user;
     int user_count = 0;
     string_t *buf;
     time_t t;

     t = time( 0 );

     buf = string_alloc( "" );

     string_append_title( u, buf, LEFT, "uid^R)..(^Yfd^R).(^Yname^R)...............(^Yin^R)...(^Yout^R)....(^Yx^R).(^Yy^R).(^Ynaws^R).(^Ycolour" );
/*                                       xxxx  xxx  xxxxxxxxxxxxxxxxxxxx xxxxxx xxxxxxxx xxx xxx xxx    xxx  */

     while( mover )
     {
          user = mover -> u;
          
          if( user -> sys_flags & USF_LOGGED_IN && ( t - user -> last_activity ) < INACTIVITY_TIMEOUT )
          {
               string_append( buf, "   %-4d   %-3d  %-20.20s %-6d %-8d %-3d %-3d %s    %s\n",
                                       user -> userid,
                                       user -> socket -> fd,
                                       user -> current_name,
                                       user -> socket -> received_bytes,
                                       user -> socket -> sent_bytes,
                                       user -> socket -> x,
                                       user -> socket -> y,
                                       ( user-> socket -> flags & SOCK_NAWS ) ? "on " : "off",
                                       ( user -> flags & UF_COLOUR_ON )      ? "on " : "off" );
               user_count++;
          }
          mover = mover -> next;
     }
     string_append_title( u, buf, LEFT, "%d user%s", user_count, ( user_count > 1 ) ? "s" : "" );

     command_output( buf -> data );

     string_free( buf );
     
}

void who( user_t *u, int argc, char *argv[] )
{
     ulist_t *mover = universe -> connected;
     user_t *user;
     int user_count = 0;
     string_t *buf;
     char colc;
     char activec;
     int inactivity;

     buf = string_alloc( "" );

     string_append_title( u, buf, LEFT, "Online Users" );

     while( mover )
     {
          user = mover -> u;
     
          inactivity = inactive_time( user );
          if( user -> sys_flags & USF_LOGGED_IN && inactivity < INACTIVITY_TIMEOUT )
          {
               if( inactivity < ( INACTIVITY_TIMEOUT / 3 ) )
               {
                    colc = 'g'; activec = '+'; 
               }
               else if( inactivity < ( ( INACTIVITY_TIMEOUT / 3 ) * 2 ) )
               {
                    colc = 'y'; activec = '?';
               }
               else
               {
                    colc = 'r'; activec = '!';
               }
               
               string_append( buf, " ^%c%c^n %15.15s %s\n", colc, activec, user -> current_name, user_title( user ) );
               user_count++;
          }
          mover = mover -> next;
     }
     string_append_title( u, buf, LEFT, "%d user%s", user_count, ( user_count > 1 ) ? "s" : "" );

     command_output( buf -> data );

     string_free( buf );
     
}

void look( user_t *u, int argc, char *argv[] )
{
     ulist_t *mover = universe -> connected;
     user_t *user;
     int user_count = 0;
     string_t *buf;

     buf = string_alloc( "" );

     while( mover )
     {
          user = mover -> u;
     
          if( user -> sys_flags & USF_LOGGED_IN && inactive_time( user ) < INACTIVITY_TIMEOUT )
          {                                  
               if( user_count == 0 )
                    string_append( buf, "%s", user -> current_name );
               else
                    string_append( buf, ", %s", user -> current_name );

               user_count++;
          }
          mover = mover -> next;
     }

     if( user_count > 1 )
          command_output( "\nThere are %d users online:\n%s\n", user_count, buf -> data );
     else
          command_output( "\nYou are the only user connected\n" );

     string_free( buf );
     
}

void activity( user_t *u, int argc, char *argv[] )
{                                
     ulist_t *mover;
     user_t *user;
     int user_count = 0;
     string_t *buf;
     int inactivity;
     int i;

     if( argc > 1 )
     {
          user = user_find_on_by_name( u, argv[ 1 ] );
          
          if( user != NULL )
               command_output( "\n  %s is %s inactive\n", 
               user -> current_name, string_time( inactive_time( user ) ) );
     }
     else
     {

          buf = string_alloc( "" );

          string_append_title( u, buf, LEFT, "User Activity" );

          mover = universe -> connected;

          while( mover )
          {              
               user = mover -> u;
           
               inactivity = inactive_time( user );

               if( user -> sys_flags & USF_LOGGED_IN && inactivity < INACTIVITY_TIMEOUT )
               {
                    string_append( buf, " %20.20s ^C|^R", user -> current_name );

                    for( i = 0; i < 45; i++ )
                    {
                         if( inactivity < INACTIVITY_TIMEOUT - ( i * 60 ) ) 
                              string_append( buf, "%c", '=' );
                         else
                              string_append( buf, "%c", ' ' );

                         if( i == 14 )
                              string_append( buf, "^C|^Y" );
                         else if( i == 29 )
                              string_append( buf, "^C|^G" );
                         
                    }
/*
                    for( i = 0; i < 15; i++ )
                         if( inactivity < INACTIVITY_TIMEOUT - ( i * 60 ) ) 
                              string_append( buf, "%c", '=' );
                         else
                              string_append( buf, "%c", ' ' );

                    string_append( buf, "^C|^Y" );

                    for( i = 0; i < 15; i++ )
                         if( inactivity < ( ( ( INACTIVITY_TIMEOUT / 3 ) * 2 ) - ( i * 60 ) ) )
                              string_append( buf, "%c", '=' );
                         else
                              string_append( buf, "%c", ' ' );
               
                    string_append( buf, "^C|^G" );

                    for( i = 0; i < 15; i++ )
                         if( inactivity < ( ( INACTIVITY_TIMEOUT / 3 ) - ( i * 60 ) ) )
                              string_append( buf, "%c", '=' );
                         else
                              string_append( buf, "%c", ' ' );
*/
                    string_append( buf, "^C|" );

                    string_append( buf, " ^c%6.6s^n\n", string_time_short( inactivity ) );
                    user_count++;
               }
               mover = mover -> next;
          }
          string_append_title( u, buf, LEFT, "%d user%s", user_count, ( user_count > 1 ) ? "s" : "" );

          command_output( buf -> data );

          string_free( buf );
     }
}

void connected( user_t *u, int argc, char *argv[] )
{                                
     ulist_t *mover = universe -> connected;
     user_t *user;
     int user_count = 0;
     string_t *buf;
     char colc;
     char activec;
     int inactivity;

     buf = string_alloc( "" );

     string_append_title( u, buf, LEFT, "Connections" );

     while( mover )
     {
          user = mover -> u;

     
          if( user -> sys_flags & USF_LOGGED_IN )
          {
               inactivity = inactive_time( user );
               if( inactivity < ( INACTIVITY_TIMEOUT / 3 ) )
               {
                    colc = 'g'; activec = '+'; 
               }
               else if( inactivity < ( ( INACTIVITY_TIMEOUT / 3 ) * 2 ) )
               {
                    colc = 'y'; activec = '?';
               }
               else if( inactivity < INACTIVITY_TIMEOUT )
               {
                    colc = 'r'; activec = '!';
               }
               else
               {
                    colc = 'W'; activec = '@';
               }

               string_append( buf, " ^%c%c^n  %-20.20s %-15.15s  %s\n",
                                   colc, activec, 
                                   user -> current_name, 
                                   user -> socket -> ip, 
                                   string_time_short( inactive_time( user ) ) 
                            );
               user_count++;
          }
          mover = mover -> next;
     }
     string_append_title( u, buf, LEFT, "%d user%s", user_count, ( user_count > 1 ) ? "s" : "" );

     command_output( buf -> data );

     string_free( buf );

}

void plugin_init( plugin_s *p_handle )
{          
       add_command( "activity",  activity,  p_handle, CMD_INFO,     0 );
       add_command( "active",    activity,  p_handle, CMD_INVIS, 0 );
       add_command( "idle",      activity,  p_handle, CMD_INVIS, 0 );
       add_command( "idol",      activity,  p_handle, CMD_INVIS, 0 );

       add_command( "connected", connected, p_handle, CMD_INFO,     0 );
       add_command( "lsi",       connected, p_handle, CMD_INVIS, 0 );

       add_command( "whot",      whot,      p_handle, CMD_INFO,     0 );

       add_command( "who",       who,       p_handle, CMD_INFO | CMD_BASIC,     0 );
       add_command( "w",         who,       p_handle, CMD_INVIS, 0 );

       add_command( "look",      look,      p_handle, CMD_INFO,     0 );
       add_command( "l",         look,      p_handle, CMD_INVIS, 0 );
   
       return;
}           

void plugin_fini( plugin_s *p_handle )
{
}

