
#include "plugin_headers.h"

#include <crypt.h>

PLUGIN_NAME(    "Save User" )
PLUGIN_VERSION( "0.1"  )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "galak" )

void got_password2( user_t *, char * );
void got_password1( user_t *, char * );
void got_current_password( user_t *, char * );

void got_password2( user_t *u, char *str )
{
     *( str + 8 ) = '\0';

     if( !strcmp( str, u->password ) )
     {
          free( u->password );
          u->password = strdup( crypt( str, "el" ) );
          command_output( "\nPassword Changed\n" );
          password_mode( u, 0 );
          do_prompt( u );
          u->func = 0;
          save_user( u );
     }
     else
     {
          command_output( "\nPasswords don\'t match!\nPlease try again : \n" );
          prompt_user( u, "New Password > " );
          u->func = got_password1;
     }
}

void got_password1( user_t *u, char *str )
{
     if( strlen( str ) < 3 )
     {
          command_output( "\nNew password must be at least 3 characters long!\n" );
          prompt_user( u, "New Password > " );
          return;
     }

     if( strlen( str ) > 8 )
     {
          u->password = ( char * ) malloc( 9 );
          strncpy( u->password, str, 8 );
          *( u->password + 8 ) = '\0';
     }
     else
          u->password = strdup( str );

     command_output( "\nPlease enter your new password one more time\n" );
     prompt_user( u, "New Password > " );
     u->func = got_password2;
}

void got_current_password( user_t *u, char *str )
{
     if( !strcmp( crypt( str, "el" ), u->password ) )
     {
          command_output( "\nPlease enter a new password\n" );
          prompt_user( u, "New Password > " );
          u->func = got_password1;
     }
     else
     {
          command_output( "\nIncorrect password!\n" );
          password_mode( u, 0 );
          do_prompt( u );
          u->func = 0;
     }
}

void password( user_t *u, int argc, char *argv[] )
{
     password_mode( u, 1 );

     if( u -> password )
     {
          command_output( "Please enter your current password\n" );
          password_mode( u, 1 );
          prompt_user( u, "Old Password > " );
          u->func = got_current_password;
     }
     else
     {
          command_output( "Please enter a new password\n" );
          password_mode( u, 1 );
          prompt_user( u, "New Password > " );
          u->func = got_password1;
     }
}


void save( user_t *u, int argc, char *argv[] )
{                       
     if( !( u -> sys_flags & USF_SAVED ) )
     {                              
          if( login_time( u ) < 600 )
          {
               command_output( "Sorry, 10 minutes login time is required\n" );
               return;
          }
          else
          {
               if( !( u -> password ) )
               {
                    command_output( "Please enter a new password\n" );
                    password_mode( u, 1 );
                    prompt_user( u, "New Password > " );
                    u->func = got_password1;
               }
               else      
               {
                    save_user( u );
               }
          }
     }
     else
     {               
          save_user( u );
     }
}

void list_saved( user_t *u, int argc, char *argv[] )
{
     userid_t *mover;
     int a;
     int start;
     string_t *buf;

     buf = string_alloc( "" );

     string_append_title( u, buf, LEFT, "List of Saved Users" );
               
     for( a = 0; a < 26; a++ )
     {
          mover = universe -> userids[ a ];
                                        
          start = 0;
          while( mover )
          {    
               if( !start )
                    string_append( buf, "%c: ^g%s^n(%d)", 'a' + ( char ) a, mover -> name, mover -> userid );
               else
                    string_append( buf, ", ^g%s^n(%d)", mover -> name, mover -> userid );

               start = 1;
               mover = mover -> next;
          }

          if( start ) string_append( buf, "\n" );
     }
     string_append_title( u, buf, LEFT, "%d user%s have been saved", universe -> res_count, ( universe -> res_count > 1 ) ? "s" : "" );
     
     command_output( buf -> data );

     string_free( buf );
}              

void hsave( user_t *u, int argc, char *argv[] )
{                       
     if( !( u -> sys_flags & USF_SAVED ) )
     {                              
               if( !( u -> password ) )
               {
                    command_output( "Please enter a new password\n" );
                    password_mode( u, 1 );
                    prompt_user( u, "New Password > " );
                    u->func = got_password1;
               }
               else      
               {
                    save_user( u );
               }
     }
     else
     {               
          save_user( u );
     }
}

void plugin_init( plugin_s *p_handle )
{          
       add_command( "save",     save,       p_handle, CMD_SYSTEM ,  0 );
       add_command( "hsave",    hsave,      p_handle, CMD_INVIS ,   0 );
       add_command( "password", password,   p_handle, CMD_SYSTEM ,  0 );
       add_command( "passwd",   password,   p_handle, CMD_INVIS,    0 );

       add_command( "saved",    list_saved, p_handle, CMD_SYSTEM,   0 );


       return;
}           

void plugin_fini( plugin_s *p_handle )
{

}

