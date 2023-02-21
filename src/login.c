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
#include  "core_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <crypt.h>

void password_mode( user_t *u, int s )
{
     if( s == 1 )
          user_write( u, "\377\373\001" );
     else
          user_write( u, "\377\374\001" );     
}   

void login_who( user_t *u )
{
     ulist_t *mover = universe -> connected;
     int user_count = 0;
     string_t *buf;

     buf = string_alloc( "" );

     while( mover )
     {
          if( mover -> u -> sys_flags & USF_LOGGED_IN && 
              inactive_time( mover -> u ) < INACTIVITY_TIMEOUT && 
              !( mover -> u -> sys_flags & USF_LOGOUT ) )
          {                                  
               if( user_count == 0 )
                    string_append( buf, "%s", mover -> u -> current_name );
               else
                    string_append( buf, ", %s", mover -> u -> current_name );

               user_count++;
          }
          mover = mover -> next;
     }

     if( user_count > 1 )
          system_output( u, "\nThere are %d users online:\n%s\n", user_count, buf -> data );
     else if( user_count == 1 )
     {
          if( ( u -> sys_flags & USF_LOGGED_IN ) )
               system_output( u, "\nYou are the only user connected\n" );
          else
               system_output( u, "%s is the only user connected\n", buf -> data );
     }
     else
          system_output( u, "\nThere are no users connected\n" );

     string_free( buf );

} 

void login_end( user_t *u )
{
     user_t *old = NULL;
     ulist_t *mover;
     plugin_s *pmover, *pnext;
     void ( *fptr ) ( );
     env_t *envs; 
     int userid;

     ( universe -> socket ).connection_count ++;

     if( !( u -> sys_flags & USF_SAVED ) )         
     {           
          /* this is where any non-zero default user values are defined */

          /* assign the user a temporary userid */
          userid = 0;

          /* find the lowest used userid */
          mover = universe -> connected;
          while( mover )
          {
               if( mover -> userid < userid ) userid = mover -> userid;
               mover = mover -> next;
          }
          u -> userid = userid - 1;
          
          /* set the userid in the ulist */
          mover = universe -> connected;

          while( mover && mover -> u != u )
               mover = mover -> next;

          if( mover )
               mover -> userid = u -> userid;


          system_output( u, "\n%s\n", get_textmessage( "welcome_newuser" ) );
     }
     else
     {
          mover = universe -> connected;
          
          while( mover )
          {
               /* check if the user is already logged on */
               if( mover -> u != u && mover -> userid == u -> userid )
               {
                    mover -> u -> sys_flags |= USF_LOGOUT_QUIET;
                    old = mover -> u;
               } 
               /* set the userid in the ulist */ 
               else if( mover -> u == u )
               {
                    mover -> userid = u -> userid;
               }
               
          
               mover = mover -> next;
          }
     
          system_output( u, "%s", get_textmessage( "changes" ) );
     }
     
     if( !old )
     {
          u -> total_connections ++;

          /* inform plugins of login */
          pmover = plugin_head;

          while( pmover )
          {                          
               pnext = pmover -> next;

               PLUGIN_SYM( pmover -> handle, "PLUGIN_USER_LOGIN", fptr );
                         

               if( fptr != NULL )
               {
                    if( env_push( pmover, "%s login hook", pmover -> name ) )
                    {
                         debug( 5, "Failed to push env in login plugin hooks execution" );
                    }
                    else
                    {
                         envs = env_top( );

                         if( sigsetjmp( envs -> env, 1 ) )
                         {
                              env_pop( );
                         }
                         else
                         {
                              fptr( u );
                              env_pop( );
                         }
                    }
               }
     
               pmover = pnext;
          }

          group_output_ex( CINFORM, u, universe -> connected, "^g>^G> ^n%s connects ^G<^g<^n\n", u -> current_name );
     }
     else  /* reconnection */
     {
               /* inform plugins of the reconnection */
               pmover = plugin_head;

               while( pmover )
               {                          
                    pnext = pmover -> next;

                    PLUGIN_SYM( pmover -> handle, "PLUGIN_USER_RECONNECT", fptr );

                    if( fptr != NULL )
                    {
                         if( env_push( pmover, "%s reconnect hook", pmover -> name ) )
                         {
                              debug( 5, "Failed to push env in reconnect plugin hooks execution" );
                         }
                         else
                         {
                              envs = env_top( );
                         
                              if( sigsetjmp( envs -> env, 1 ) )
                              {
                                   env_pop( );
                              }
                              else
                              {
                                   fptr( old, u );
                                   env_pop( );
                              }
                         }
                    }
     
                    pmover = pnext;
               }


               group_output_ex( CINFORM, u, universe -> connected, "^Y>^y< ^n%s reconnects ^y>^Y<^n\n", u -> current_name );
     }

     u -> func = 0;
     u -> login_time = u -> last_activity = time( 0 );
     u -> sys_flags |= USF_LOGGED_IN;

     login_who( u );

     system_output( u, "\n^g>^G> ^nWelcome to %s ^G<^g<^n\n", SERVER_NAME );

     do_prompt( u );
}

/**** saved users ****/

void got_password( user_t *u, char *str )
{
     if( !strcmp( crypt( str, "el" ), u->password ) )
     {
          password_mode( u, 0 );
          login_end( u );
     }
     else
     {
          system_output( u, "\nIncorrect password!\n" );
          password_ques( u );
     }
}

void password_ques( user_t *u )
{
     system_output( u, "\nPlease enter your password\n" );

     password_mode( u, 1 );
     prompt_user( u, "Password: " );
     u->func = got_password;
}

/**** new users ****/

void handle_disclaimer( user_t *u, char *str )
{
     if( !strcasecmp( str, "agree" ) )
          login_end( u );
     else if( !strcasecmp( str, "quit" ) )
          u -> sys_flags |= USF_LOGOUT_QUIET;
     else
     {
          system_output( u, "\n   ^Y*  ^rYou need to type either \'^Ragree^r\' or \'^Rquit^r\'  ^Y*^n\n\n" );
          prompt_user( u, "Please enter \'agree\' or \'quit\': " );
     }
}

void disclaimer( user_t *u )
{
     system_output( u, "\n%s\n", get_textmessage( "disclaimer" ) );
     prompt_user( u, "Please enter \'agree\' or \'quit\': " );
     u -> func = handle_disclaimer;
}

void handle_gender_ques( user_t *u, char *str )
{
     switch( *str )
     {
          case 'f' :
          case 'F' :
               u -> gender = GENDER_FEMALE;
               system_output( u, "   You set your gender to Female.\n" );
               break;
          case 'm' :
          case 'M' :
               u -> gender = GENDER_MALE;
               system_output( u, "   You set your gender to Male.\n" );
               break;
          default :
               u -> gender = GENDER_NONE;
               system_output( u, "   You have no gender.\n" );
               break;
     }
     disclaimer( u );
}

void gender_ques( user_t *u )
{
     system_output( u, "\n   Your gender is needed for the purposes of correct grammar\n"
                       "   Please enter either \'^WFemale^n\', \'^WMale^n\' or \'^WNone^n\'\n\n" );

     prompt_user( u, "Please enter your gender ([F]emale/[M]ale/[N]one): " );
     u -> func = handle_gender_ques;
}

void handle_colour_ques( user_t *u, char *str )
{
     switch( *str )
     {
          case 'n' :
          case 'N' :
               system_output( u, "\n   You turn colour off. If you change your mind you can\n"
                                 "   turn it on later using the command \'colour on\'\n" );
               break;

          default :
               u -> flags |= UF_COLOUR_ON;
               system_output( u, "\n   ^gYou turn colour on. If you find that you don't like it,\n"
                                 "   ^gyou can turn it off using the command \'^Gcolour off^g\'^n\n" );
               break;
     }
     add_log( "newuser", "%-20.20s (%dx%d)  naws %s  colour %s",
          u -> current_name,
          u -> socket -> x,
          u -> socket -> y,
          ( u-> socket -> flags & SOCK_NAWS ) ? "on " : "off",
          ( u -> flags & UF_COLOUR_ON )       ? "on " : "off" );

     gender_ques( u );

}

void colour_ques( user_t *u )
{
     system_output( u, "\n   Colour is available to users with modern terminals\n\n" );

     prompt_user( u, "Do you want to try turning colour on ? ([Y]es/[n]o): " );
     u -> func = handle_colour_ques;
}

void stats_info( user_t *u )
{
     ulist_t *mover = universe -> connected;
     user_t *user;
     int inactivity;
     int user_count = 0;
                            
     /* count users online */
     while( mover )
     {
          user = mover -> u;

          inactivity = inactive_time( user );
          if( user -> sys_flags & USF_LOGGED_IN && inactivity < INACTIVITY_TIMEOUT )
          {
               user_count ++;
          }
          mover = mover -> next;
     }

     system_output( u,                       
                    "url: http://hive.yuss.org\n"              
                    "contact: hive@yuss.org\n"
                    "connect: telnet://hive.yuss.org:5000\n"
                    "totallogins: %d\n"
                    "newbies: yes\n"
                    "residents: yes\n"
                    "users: %d\n",   
                    ( universe -> socket ).connection_count,
                    user_count
                  );                    

     u -> sys_flags |= USF_LOGOUT_QUIET;
}

/**** get username ****/

void handle_name( user_t *u, char *str )
{
     int len = strlen( str );
     int userid;
     ulist_t *mover; 
     char *ptr;
     
     if( !strcasecmp( str, "stats_info" ) )
     {
          stats_info( u );
          return;
     }

     if( !strcasecmp( str, "quit" ) )
     {
          u -> sys_flags |= USF_LOGOUT_QUIET;
          return;
     }

     if( !strcasecmp( str, "who" ) || !strcasecmp( str, "look" ) )
     {
          login_who( u );
          prompt_user( u, "Please enter your nickname : " );
          return;
     }

     if( len < 2 )
     {
          system_output( u, "Sorry, your name must be at least 2 characters long\n" );
          prompt_user( u, "Please enter a longer nickname: " );
          return;
     }
     
     if( len > MAX_NAME )
     {
          system_output( u, "Sorry, your name must be at least %d characters shorter\n", len - MAX_NAME );
          prompt_user( u, "Please enter a shorter nickname: " );
          return;
     }

     if( !( *str >= 'a' && *str <= 'z' ) && !( *str >= 'A' && *str <= 'Z' ) )
     {
          system_output( u, "Sorry, the first letter of your name must be alphabetic\n" );
          prompt_user( u, "Please enter a new nickname: " );
          return;
     }

     ptr = str;
     
     while( *ptr )
     {
          if( !( *ptr >= 'a' && *ptr <= 'z' ) && 
              !( *ptr >= 'A' && *ptr <= 'Z' ) &&
              !( *ptr >= '0' && *ptr <= '9' ) )
          {
               system_output( u, "Sorry, your name can only contain alpha-numeric characters\n" );
               prompt_user( u, "Please enter a new nickname: " );
               return;
          }
          ptr++;
     }

     mover = universe -> connected;
     while( mover )
     {
          if( mover -> u != u && 
              !( mover -> u -> sys_flags & USF_SAVED ) && 
              mover -> u -> current_name &&
              !strcasecmp( mover -> u -> current_name, str ) )
	       {
               system_output( u, "Sorry, there is already a new user connected with that name\n" );
               prompt_user( u, "Please enter a new nickname: " );
               return;
          }
          mover = mover -> next;     
     }

     u -> current_name = strdup( str );

     /* check for res */

     userid = user_get_userid( u -> current_name );

     if( userid != -1 )
     {           
          system_output( u, "User exists\n" );

          u -> userid = userid;
          u -> sys_flags |= USF_SAVED;

          if( load_user( u ) )
          {
               password_ques( u );
               return;
          }

          system_output( u, "WARNING: Couldn't open base user file!\n" );

          login_end( u );
          return;
     }

     /* handle newbie connection */
     universe -> new_count++;

     if( universe -> new_count > NEWUSER_LIMIT )
     {
          system_output( u, "%s", get_textmessage( "max_newuser" ) );
          u -> sys_flags |= USF_LOGOUT_QUIET;
          return;
     }

     colour_ques( u ); /* continue login sequence */
}

user_t *login_start( int user_fd, char *ip )
{
     user_t *u = user_alloc( user_fd, ip );
     universe -> connected = ulist_add( universe -> connected, u );

     universe -> socket.current_connections++;

     if( strstr( ip, "146.227" ) == ip )
          system_output( u, "\n%s\n", get_textmessage( "connect_screen.dmu" ) );
     else
          system_output( u, "\n%s\n", get_textmessage( "connect_screen" ) );

     password_mode( u, 0 );
     prompt_user( u, "Please enter your nickname: " );

     u -> func = handle_name;

     return u;
}

void logout( user_t *u )
{
     plugin_s *pmover, *pnext;
     void ( *fptr ) ( );
     env_t *envs;
                                       

     shutdown( u -> socket -> fd, 2 );
     close( u -> socket -> fd );

     if( u -> sys_flags & USF_SAVED )
     {

          if( u -> sys_flags & USF_LOGOUT_ANNOUNCE )
          {
               /* update any variables before the final save */

               u -> total_time      += login_time( u ); /* temp */

               u -> total_bytes_in  += u -> socket -> sent_bytes;
               u -> total_bytes_out += u -> socket -> received_bytes;

               save_user( u );
          }
     }
     else
     {
          universe -> new_count --;
     }

     if( u -> sys_flags & USF_LOGGED_IN )
     {
          if( u -> sys_flags & USF_LOGOUT_ANNOUNCE )
          {
               /* inform plugins of logout */
               pmover = plugin_head;

               while( pmover )
               {                          
                    pnext = pmover -> next;
                    PLUGIN_SYM( pmover -> handle, "PLUGIN_USER_LOGOUT", fptr );
                    if( fptr != NULL )
                    {
                         if( env_push( pmover, "%s logout hook", pmover -> name ) )
                         {
                              debug( 5, "Failed to push env in logout plugin hooks execution" );
                         }
                         else
                         {
                              envs = env_top( );
                         
                              if( sigsetjmp( envs -> env, 1 ) )
                              {
                                   env_pop( );
                              }
                              else
                              {
                                   fptr( u );
                                   env_pop( );
                              }
                         }
                    }
     
                    pmover = pnext;
               }

               group_output_ex( CINFORM, u, universe -> connected, "^R<^r< ^n%s disconnects ^r>^R>^n\n", u -> current_name );
          }

     }  

     universe -> connected = ulist_remove( universe -> connected, u );
     user_free( u );
        
     universe -> socket.current_connections--;
}
