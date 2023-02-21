/* 
 * example.c 
 * An example plugin
 *
 * Author: James Garlick
 */

#include <signal.h>             

#include "plugin_headers.h"
  
/* required */
/* a short name for the plugin */
PLUGIN_NAME(    "Plugin Example" )

/* required */
/* the plugin version */
PLUGIN_VERSION( "1.0"  )

/* required */
/* a comma seperated list of plugins required before  */
/* this plugin will load (plugin filenames)           */
PLUGIN_DEPENDENCIES( "" )

/* required */
/* a comma seperated list of the plugin authors, used */
/* to authorize the loading/unloading of the plugin   */
PLUGIN_AUTHORS_LIST( "galak" )

/* a unique file extension for this plugins user data */
#define FILE_EXTENSION "example"

/* a pointer to the plugin structure - needed for udata handling */
plugin_s *example_h;

/* a test user data structure */
typedef struct test_s
{
     uint32 int1;
     uint32 int2;
     uint32 int3;
     uint32 int4;
     uint32 int5;

     char  *string1; 
     char  *string2; 
     char  *string3; 
     char  *string4; 
     char  *string5; 
     
} test_t;

/* optional */
/* called every time a user logs in */
void PLUGIN_USER_LOGIN( user_t *u )
{                       
     debug( 5, "example: user login: userid %d, %s", u -> userid, u -> current_name );
}
                
/* optional */
/* called every time a user logs out */
void PLUGIN_USER_LOGOUT( user_t *u )
{
     debug( 5, "example: user logout: userid %d, %s", u -> userid, u -> current_name );
}

/* optional */
/* called every time a user reconnects */
void PLUGIN_USER_RECONNECT( user_t*old, user_t *new )
{
     debug( 5, "example: user reconnect: userid %d, %s", new -> userid, new -> current_name );
}

/* optional */
/* called every time a user is saved */
void PLUGIN_SAVE_UDATA( user_t *u )
{
     char fname[ 255 ];
     FILE *f;     
     udata_t *udata;
     test_t *test;

     udata = udata_find( u, example_h );
     if( udata == NULL || udata -> data == NULL )
     {
          debug( 5, "no need to save as udata not loaded" );
          return;
     }

     test = ( test_t * ) udata -> data;
     debug( 5, "UDATA_FIND: FOUND %d", test );

     sprintf( fname, "%s/data/users/%d.%s", BASE_DIR, u -> userid, FILE_EXTENSION );

     if( !( f = fopen( fname, "wb" ) ) )
     {
          debug( 3, "Error: couldnt save \'%s\' in plugin \'%s\'\n", u -> current_name, example_h -> name );
          return;
     }

     command_output( "%d\n", test -> int1 );
     command_output( "%d\n", test -> int2 );
     command_output( "%d\n", test -> int3 );
     command_output( "%d\n", test -> int4 );
     command_output( "%d\n", test -> int5 );

     command_output( "%s\n", test -> string1 );
     command_output( "%s\n", test -> string2 );
     command_output( "%s\n", test -> string3 );
     command_output( "%s\n", test -> string4 );
     command_output( "%s\n", test -> string5 );

     fwritew( 1,   INT32,  &( test -> int1 ), f );
     fwritew( 2,   INT32,  &( test -> int2 ), f );
     fwritew( 3,   INT32,  &( test -> int3 ), f );
     fwritew( 4,   INT32,  &( test -> int4 ), f );
     fwritew( 5,   INT32,  &( test -> int5 ), f );
     fwritew( 6,   STRING, test -> string1,   f );
     fwritew( 7,   STRING, test -> string2,   f );
     fwritew( 8,   STRING, test -> string3,   f );
     fwritew( 9,   STRING, test -> string4,   f );
     fwritew( 10,  STRING, test -> string5,   f );

     fclose( f );

     debug( 5, "example udata saved for %s", u -> current_name );
}
            
/* only required if user data is being loaded/saved */
/* returns 0 on success */
int PLUGIN_LOAD_UDATA( user_t *u, void *data )
{
     char fname[ 255 ];
     FILE *f;     
     uint16 elem;
     test_t *test;

     test = ( test_t * ) data;
     if( test == NULL )
          return 1;

     sprintf( fname, "%s/data/users/%d.%s", BASE_DIR, u -> userid, FILE_EXTENSION );

     f = fopen( fname, "rb" );
     if( f == NULL )
          return 1;

     while( fread( &elem, sizeof( uint16 ), 1, f ) )
     {
          switch( elem )
          {
               case 1 :
                    test -> int1 = fread_int32( f );
                    break;
               case 2 :
                    test -> int2 = fread_int32( f );
                    break;
               case 3 :
                    test -> int3 = fread_int32( f );
                    break;
               case 4 :
                    test -> int4 = fread_int32( f );
                    break;     
               case 5 :
                    test -> int5 = fread_int32( f );
                    break;
               case 6 :
                    test -> string1 = fread_string( f );
                    break;
               case 7 :
                    test -> string2 = fread_string( f );
                    break;
               case 8 :
                    test -> string3 = fread_string( f );
                    break;
               case 9 :
                    test -> string4 = fread_string( f );
                    break;
               case 10 :
                    test -> string5 = fread_string( f );
                    break;
          }
     }

     fclose( f );

     return 0;
}  

void PLUGIN_SET_UDATA_DEFAULTS( void *data )
{
     test_t *test;
                 
     debug( 5, "1" );

     test = ( test_t * ) data;
     if( test == NULL )
          return;

     debug( 5, "2" );
/*                 
     test -> int1 = 1;
     test -> int2 = 2;
     test -> int3 = 3;
     test -> int4 = 4;
     test -> int5 = 5;

     test -> string1 = strdup( "string1" ); 
     test -> string2 = strdup( "string2" ); 
     test -> string3 = strdup( "string3" ); 
     test -> string4 = strdup( "string4" ); 
     test -> string5 = strdup( "string5" ); 
*/
}

/* an example of retrieving user data */
void testlist( user_t *u, int argc, char *argv[] )
{
     test_t *test;

     test = ( test_t * ) udata_get( u, example_h, sizeof( test_t ) );
     if( test == NULL )
          return;

     command_output( "%d\n", test -> int1 );
     command_output( "%d\n", test -> int2 );
     command_output( "%d\n", test -> int3 );
     command_output( "%d\n", test -> int4 );
     command_output( "%d\n", test -> int5 );

     command_output( "%s\n", test -> string1 );
     command_output( "%s\n", test -> string2 );
     command_output( "%s\n", test -> string3 );
     command_output( "%s\n", test -> string4 );
     command_output( "%s\n", test -> string5 );

}

void setudata( user_t *u, int argc, char *argv[] )
{
     test_t *test;

     test = ( test_t * ) udata_get( u, example_h, sizeof( test_t ) );
     if( test == NULL )
          return;

     test -> int1 = 5;     
     test -> int2 = 4;     
     test -> int3 = 3;     
     test -> int4 = 2;     
     test -> int5 = 1;     

     test -> string1 = strdup( "string5" );
     test -> string2 = strdup( "string4" );
     test -> string3 = strdup( "string3" );
     test -> string4 = strdup( "string2" );
     test -> string5 = strdup( "string1" );
}

/* prints the tokens sent to the command */
void test( user_t *u, int argc, char *argv[] )
{                                
     int a;

     command_output( "\nargc = %d\n", argc );

     for( a = 0; a < argc; a++ )
          command_output( "argv %d = \'%s\'\n", a, argv[ a ] );

}

/* a command to test crash recovery */
void setupbomb( user_t *u, int argc, char *argv[] )
{
     raise( SIGSEGV );     
}
     
/* an example of sub commands */
void sub1( user_t *u, int argc, char *argv[] )
{
     command_output( "sub command 1: %s, %s\n", argv[ 0 ], argv[ 1 ] );
}

void sub2( user_t *u, int argc, char *argv[] )
{
     command_output( "sub command 2: %s, %s\n", argv[ 0 ], argv[ 1 ] );
}

void sub3( user_t *u, int argc, char *argv[] )
{
     command_output( "sub command 3: %s, %s\n", argv[ 0 ], argv[ 1 ] );
}

void subtest( user_t *u, int argc, char *argv[] )
{
     command_output( "no sub command, command executed instead\n" );
}

void hello( user_t *u, int argc, char *argv[] )
{
     user_output( CGAMES, NULL, u, "hello world\n" );     
}

/* required */
/* called as the plugin is loaded */
void plugin_init( plugin_s *p_handle )
{            
     example_h = p_handle;

     add_command( "HELLO", hello, p_handle, CMD_SETTINGS, 0 );
     
     add_command( "setupbomb", setupbomb, p_handle, CMD_INVIS ,0 );

     add_command( "test0", test, p_handle, CMD_INVIS ,0 );
     add_command( "test1", test, p_handle, CMD_INVIS ,1 );
     add_command( "test2", test, p_handle, CMD_INVIS ,2 );
     add_command( "test3", test, p_handle, CMD_INVIS ,3 );
     add_command( "test4", test, p_handle, CMD_INVIS ,4 );
     add_command( "test5", test, p_handle, CMD_INVIS ,5 );

     add_command( "tl", testlist, p_handle, CMD_INVIS ,0 );
     add_command( "set", setudata, p_handle, CMD_INVIS ,0 );
                                
     /* a command with sub commands, you must ask for at least 1 token */
     add_command( "subtest",subtest, p_handle, CMD_INVIS ,1 );

     add_subcommand( "subtest", "sub1", sub1, p_handle, CMD_INVIS ,0 );
     add_subcommand( "subtest", "sub2", sub2, p_handle, CMD_INVIS ,0 );
     add_subcommand( "subtest", "sub3", sub3, p_handle, CMD_INVIS ,0 );

     return;
}           

/* optional */
/* called as the plugin is unloaded */
void plugin_fini( plugin_s *p_handle )
{
     /* commands and udata are removed automatically */
}


