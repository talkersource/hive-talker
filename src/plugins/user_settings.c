#include <string.h>

#include "plugin_headers.h"

#include "user_settings.h"

PLUGIN_NAME( "User Settings" )
PLUGIN_VERSION( "0.4"  )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "galak" )

#define FILE_EXTENSION "user_settings"
     
plugin_s *user_settings_h;

void colour( user_t *u, int argc, char *argv[] )
{
     if( argc > 1 )
     {
          if( !strcasecmp( argv[ 1 ], "on" ) ||
              !strcasecmp( argv[ 1 ], "yes" ) )
               u -> flags |= UF_COLOUR_ON;
          else if( !strcasecmp( argv[ 1 ], "off" ) ||
                  !strcasecmp( argv[ 1 ], "no" ) ||
                  !strcasecmp( argv[ 1 ], "none" ) )
               u -> flags &= ~UF_COLOUR_ON;
          else
               u -> flags ^= UF_COLOUR_ON;
           
     }
     else
          u -> flags ^= UF_COLOUR_ON;
     
     if( u -> flags & UF_COLOUR_ON )
          command_output( "^YANSI colour is on^N\n" );
     else
          command_output( "ANSI colour is off\n" );
}

void recap( user_t *u, int argc, char *argv[] )
{                     
     if( argc < 2 || strcasecmp( u -> current_name, argv[ 1 ] ) )
     {
          command_output( "Format : %s <your name>\n", argv[ 0 ] );
          return;
     }

     strcpy( u -> current_name, argv[ 1 ] );

     command_output( "You recap your name to: %s\n", u -> current_name );
}

void inactivemsg( user_t *u, int argc, char *argv[] )
{                     
     if( argc < 2 || cstrlen( argv[ 1 ] ) == 0 )
     {
          command_output( "Format : %s <msg>\n", argv[ 0 ] );
          return;
     }

     u -> inactivemsg = strdup( argv[ 1 ] );

     command_output( "You set your inactive message to:\n^W %s is inactive> %s^n\n", u -> current_name, u -> inactivemsg );
}

void prompt( user_t *u, int argc, char *argv[ ] )
{
     if( u -> prompt )
          free( u -> prompt );
          	
     u -> prompt = NULL;

     if( argc < 2 )
     {
          command_output( "You will now receive the default prompt\n" );
          return;
     }
     
     u -> prompt = strdup( argv[ 1 ] );
     
     if( !strcasecmp( u -> prompt, "off" ) )
          command_output( "You will no longer receive a prompt\n" );
     else
          command_output( "Prompt changed\n" );
}

void title( user_t *u, int argc, char *argv[] )
{         
     if( u -> title )
          free( u -> title );

     if( argc < 2 )
     {
          u -> title = strdup( "" );
          command_output( "You now have no title\n" );
          return;
     }
                                  
     str_trunc( argv[ 1 ], 60 );

     u -> title = strdup( argv[ 1 ]  );

     command_output( "Your title is now:\n%s %s^n\n", u -> current_name, u -> title );
          
}

/*** SET COMMAND ***/

void set( user_t *u, int argc, char *argv[ ] )
{
     command_output( "You can set optional user data with the following sub-commands:\n"
                     "  %1$s ^Wemail^N <your email address>\n"
                     "  %1$s ^Wlocation^N <your physical location>\n"
//                     "  %1$s ^Wdob^N <your date of birth in the form DD/MM/YYYY>\n"
                     "  %1$s ^Wirl_name^N <your name in real life>\n"
                     "  %1$s ^Woccupation^N <your occupation>\n"
                     "  %1$s ^Whomepage^N <the URL address of your homepage>\n"
                     "  %1$s ^Wphoto^N <the URL address of your photo>\n"
                     "  %1$s ^Winterests^N <a list of your interests>\n",
                     argv[ 0 ] 
                   );
}

void set_email( user_t *u, int argc, char *argv[ ] )
{
     udpersonal_t *udata;

     if( argc < 2 )
     {           
          command_output( "Format: set %s <your email address>\n", argv[ 0 ] );
          return;
     }

     udata = ( udpersonal_t * ) udata_get( u, user_settings_h, sizeof( udpersonal_t ) );
     if( udata == NULL )
          return;
                                
     if( strchr( argv[ 1 ], '@' ) == NULL )
     {
          command_output( "An email address must contain the symbol '@'\n" );
          return;
     }

     str_trunc( argv[ 1 ], 65 );

     udata -> email = strdup( argv[ 1 ] );

     command_output( "Your email address is now: %s\n", udata -> email );
}

void set_location( user_t *u, int argc, char *argv[ ] )
{
     udpersonal_t *udata;

     if( argc < 2 )
     {           
          command_output( "Format: set %s <your physical location>\n", argv[ 0 ] );
          return;
     }

     udata = ( udpersonal_t * ) udata_get( u, user_settings_h, sizeof( udpersonal_t ) );
     if( udata == NULL )
          return;
                 
     str_trunc( argv[ 1 ], 65 );

     udata -> location = strdup( argv[ 1 ] );

     command_output( "Your physical location is now: %s\n", udata -> location );
}

void set_dob( user_t *u, int argc, char *argv[ ] )
{
     udpersonal_t *udata;

     if( argc < 2 )
     {           
          command_output( "Format: set %s <your date of birth in the form DD/MM/YYYY>\n", argv[ 0 ] );
          return;
     }

     udata = ( udpersonal_t * ) udata_get( u, user_settings_h, sizeof( udpersonal_t ) );
     if( udata == NULL )
          return;

     command_output( "Sorry dob is not implemented\n" );     
}

void set_irl_name( user_t *u, int argc, char *argv[ ] )
{
     udpersonal_t *udata;

     if( argc < 2 )
     {           
          command_output( "Format: set %s <your name in real life>\n", argv[ 0 ] );
          return;
     }

     udata = ( udpersonal_t * ) udata_get( u, user_settings_h, sizeof( udpersonal_t ) );
     if( udata == NULL )
          return;

     str_trunc( argv[ 1 ], 65 );

     udata -> irl_name = strdup( argv[ 1 ] ); 

     command_output( "Your name in real life is set to: %s\n", udata -> irl_name );
}

void set_occupation( user_t *u, int argc, char *argv[ ] )
{
     udpersonal_t *udata;

     if( argc < 2 )
     {           
          command_output( "Format: set %s <your occupation>\n", argv[ 0 ] );
          return;
     }

     udata = ( udpersonal_t * ) udata_get( u, user_settings_h, sizeof( udpersonal_t ) );
     if( udata == NULL )
          return;

     str_trunc( argv[ 1 ], 65 );

     udata -> occupation = strdup( argv[ 1 ] );

     command_output( "Your occupation is now set as: %s\n", udata -> occupation );
}

void set_homepage( user_t *u, int argc, char *argv[ ] )
{
     udpersonal_t *udata;

     if( argc < 2 )
     {           
          command_output( "Format: set %s <the URL address of your homepage>\n", argv[ 0 ] );
          return;
     }

     udata = ( udpersonal_t * ) udata_get( u, user_settings_h, sizeof( udpersonal_t ) );
     if( udata == NULL )
          return;

     str_trunc( argv[ 1 ], 65 );

     udata -> homepage = strdup( argv[ 1 ] );

     command_output( "Your homepage is now set as: %s\n", udata -> homepage );
}

void set_photo( user_t *u, int argc, char *argv[ ] )
{
     udpersonal_t *udata;

     if( argc < 2 )
     {           
          command_output( "Format: set %s <the URL address of your photo>\n", argv[ 0 ] );
          return;
     }

     udata = ( udpersonal_t * ) udata_get( u, user_settings_h, sizeof( udpersonal_t ) );
     if( udata == NULL )
          return;

     str_trunc( argv[ 1 ], 65 );

     udata -> photo = strdup( argv[ 1 ] );

     command_output( "The location of your photo is now set as: %s\n", udata -> photo );
}

void set_interests( user_t *u, int argc, char *argv[ ] )
{
     udpersonal_t *udata;

     if( argc < 2 )
     {           
          command_output( "Format: set %s <a list of your interests>\n", argv[ 0 ] );
          return;
     }

     udata = ( udpersonal_t * ) udata_get( u, user_settings_h, sizeof( udpersonal_t ) );
     if( udata == NULL )
          return;
                                
     str_trunc( argv[ 1 ], 65 );

     udata -> interests = strdup( argv[ 1 ] );

     command_output( "Your interests are now listed as: %s\n", udata -> interests );
}

#define US_EMAIL      1
#define US_LOCATION   2
#define US_IRL_NAME   3
#define US_OCCUPATION 4
#define US_HOMEPAGE   5
#define US_PHOTO      6
#define US_INTERESTS  7

void PLUGIN_SAVE_UDATA( user_t *u )
{
     char fname[ 255 ];
     FILE *f;     
     udata_t *udata;
     udpersonal_t *up;

     udata = udata_find( u, user_settings_h );
     if( udata == NULL || udata -> data == NULL )
     {
          return;
     }

     up = ( udpersonal_t * ) udata -> data;

     sprintf( fname, "%s/data/users/%d.%s", BASE_DIR, u -> userid, FILE_EXTENSION );

     if( !( f = fopen( fname, "wb" ) ) )
     {
          debug( 3, "Error: couldnt save \'%s\' in plugin \'%s\'\n", u -> current_name, user_settings_h -> name );
          return;
     }

     if( up -> email )
          fwritew( US_EMAIL,      STRING, up -> email,      f );
     if( up -> location )
          fwritew( US_LOCATION,   STRING, up -> location,   f );
     if( up -> irl_name )
          fwritew( US_IRL_NAME,   STRING, up -> irl_name,   f );
     if( up -> occupation )
          fwritew( US_OCCUPATION, STRING, up -> occupation, f );
     if( up -> homepage )
          fwritew( US_HOMEPAGE,   STRING, up -> homepage,   f );
     if( up -> photo )
          fwritew( US_PHOTO,      STRING, up -> photo,      f );
     if( up -> interests )
          fwritew( US_INTERESTS,  STRING, up -> interests,  f );

     fclose( f );
}

/* returns 0 on success */
int PLUGIN_LOAD_UDATA( user_t *u, void *data )
{
     char fname[ 255 ];
     FILE *f;     
     uint16 elem;
     udpersonal_t *up;

     up = ( udpersonal_t * ) data;
     if( up == NULL )
          return 1;

     sprintf( fname, "%s/data/users/%d.%s", BASE_DIR, u -> userid, FILE_EXTENSION );

     f = fopen( fname, "rb" );
     if( f == NULL )
          return 1;

     while( fread( &elem, sizeof( uint16 ), 1, f ) )
     {
          switch( elem )
          {
               case US_EMAIL :
                    up -> email = fread_string( f );
                    break;
               case US_LOCATION :
                    up -> location = fread_string( f );
                    break;
               case US_IRL_NAME :
                    up -> irl_name = fread_string( f );
                    break;
               case US_OCCUPATION :
                    up -> occupation = fread_string( f );
                    break;
               case US_HOMEPAGE :
                    up -> homepage = fread_string( f );
                    break;
               case US_PHOTO :
                    up -> photo = fread_string( f );
                    break;
               case US_INTERESTS :
                    up -> interests = fread_string( f );
                    break;
          }
     }
     
     fclose( f );

     return 0;
}

void plugin_init( plugin_s *p_handle )
{                                                         
     user_settings_h = p_handle;

     add_command( "colour",      colour,      p_handle, CMD_SETTINGS,  0 );
     add_command( "color",       colour,      p_handle, CMD_INVIS,  0 );

     add_command( "recap",       recap,       p_handle, CMD_SETTINGS,  0 );
     add_command( "inactivemsg", inactivemsg, p_handle, CMD_SETTINGS,  0 );
     add_command( "idlemsg",     inactivemsg, p_handle, CMD_INVIS , 0 );
     add_command( "imsg",        inactivemsg, p_handle, CMD_INVIS , 0 );

     add_command( "prompt",      prompt,      p_handle, CMD_SETTINGS,  0 );
                                   
     add_command( "title",       title,       p_handle, CMD_SETTINGS,  0 );

     add_command( "set",         set,         p_handle, CMD_INFO,     1 );

     add_subcommand( "set", "email",      set_email,      p_handle, CMD_INFO, 0 );
     add_subcommand( "set", "location",   set_location,   p_handle, CMD_INFO, 0 );
     add_subcommand( "set", "dob",        set_dob,        p_handle, CMD_INFO, 0 );
     add_subcommand( "set", "irl_name",   set_irl_name,   p_handle, CMD_INFO, 0 );
     add_subcommand( "set", "occupation", set_occupation, p_handle, CMD_INFO, 0 );
     add_subcommand( "set", "homepage",   set_homepage,   p_handle, CMD_INFO, 0 );
     add_subcommand( "set", "photo",      set_photo,      p_handle, CMD_INFO, 0 );
     add_subcommand( "set", "interests",  set_interests,  p_handle, CMD_INFO, 0 );

}           

void plugin_fini( plugin_s *p_handle )
{                               

}


