/*
 *   binary load/save
 *   Authors: James Garlick
 */

#include <stdio.h>
#include <string.h>

#include "include/core_headers.h"


void fwrite_int8( uint16 elem, int8 *data, FILE *f )
{
     if( ( *data ) != 0 )
     {
          fwrite( &elem, sizeof( uint16 ), 1, f );
          fwrite( data, sizeof( int8 ), 1, f );
     }
}

void fwrite_uint8( uint16 elem, uint8 *data, FILE *f )
{
     if( ( *data ) != 0 )
     {
          fwrite( &elem, sizeof( uint16 ), 1, f );
          fwrite( data, sizeof( uint8 ), 1, f );
     }
}

void fwrite_int16( uint16 elem, int16 *data, FILE *f )
{
     if( ( *data ) != 0 )
     {
          fwrite( &elem, sizeof( uint16 ), 1, f );
          fwrite( data, sizeof( int16 ), 1, f );
     }
}

void fwrite_uint16( uint16 elem, uint16 *data, FILE *f )
{
     if( ( *data ) != 0 )
     {
          fwrite( &elem, sizeof( uint16 ), 1, f );
          fwrite( data, sizeof( uint16 ), 1, f );
     }
}

void fwrite_int32( uint16 elem, int32 *data, FILE *f )
{
     if( ( *data ) != 0 )
     {
          fwrite( &elem, sizeof( uint16 ), 1, f );
          fwrite( data, sizeof( int32 ), 1, f );
     }
}

void fwrite_uint32( uint16 elem, uint32 *data, FILE *f )
{
     if( ( *data ) != 0 )
     {
          fwrite( &elem, sizeof( uint16 ), 1, f );
          fwrite( data, sizeof( uint32 ), 1, f );
     }
}

void fwrite_char( uint16 elem, char *data, FILE *f )
{
     if( ( *data ) != 0 )
     {
          fwrite( &elem, sizeof( uint16 ), 1, f );
          fwrite( data, sizeof( char ), 1, f );
     }
}

void fwrite_uchar( uint16 elem, unsigned char *data, FILE *f )
{
     if( ( *data ) != 0 )
     {
          fwrite( &elem, sizeof( uint16 ), 1, f );
          fwrite( data, sizeof( unsigned char ), 1, f );
     }
}

void fwrite_string( uint16 elem, char *data, FILE *f )
{
     if( data != 0 )
     {
          fwrite( &elem, sizeof( uint16 ), 1, f );
          fwrite( data, sizeof( char ), strlen( ( char * ) data ) + 1, f );
     }
}

void fwritew( uint16 elem, int type, void *data, FILE *f )
{
     /* write data */
     switch( type )
     {
          case INT8  :
               fwrite_int8( elem, ( int8 * ) data, f );
               break;
          case UINT8 :
               fwrite_uint8( elem, ( uint8 * ) data, f );
               break;
          case INT16  :
               fwrite_int16( elem, ( int16 * ) data, f );
               break;
          case UINT16 :
               fwrite_uint16( elem, ( uint16 * ) data, f );
               break;
          case INT32  :
               fwrite_int32( elem, ( int32 * ) data, f );
               break;
          case UINT32 :
               fwrite_uint32( elem, ( uint32 * ) data, f );
               break;
          case CHAR  :
               fwrite_char( elem, ( char * ) data, f );
               break;
          case UCHAR :
               fwrite_uchar( elem, ( unsigned char * ) data, f );
               break;
          case STRING :
               fwrite_string( elem, ( char * ) data, f );
               break;
     }
}

int8 fread_int8( FILE *f )
{
     int8 elem;

     fread( &elem, sizeof( int8 ), 1, f );

     return elem;
}

uint8 fread_uint8( FILE *f )
{
     uint8 elem;

     fread( &elem, sizeof( uint8 ), 1, f );

     return elem;
}

int16 fread_int16( FILE *f )
{
     int16 elem;

     fread( &elem, sizeof( int16 ), 1, f );

     return elem;
}

uint16 fread_uint16( FILE *f )
{
     uint16 elem;

     fread( &elem, sizeof( uint16 ), 1, f );

     return elem;
}

int32 fread_int32( FILE *f )
{
     int32 elem;

     fread( &elem, sizeof( int32 ), 1, f );

     return elem;
}

uint32 fread_uint32( FILE *f )
{
     uint32 elem;

     fread( &elem, sizeof( uint32 ), 1, f );

     return elem;
}

char fread_char( FILE *f )
{
     char elem;

     fread( &elem, sizeof( char ), 1, f );

     return elem;
}

unsigned char fread_uchar( FILE *f )
{
     unsigned char elem;

     fread( &elem, sizeof( unsigned char ), 1, f );

     return elem;
}

char *fread_string( FILE *f )
{
     char c;
     string_t *buf;
     char *ret;

     buf = string_alloc( "" );

     while( fread( &c, sizeof( char ), 1, f ),  c != 0 )
          string_append( buf, "%c", c );

     ret = strdup( buf -> data );
     string_free( buf );

     return ret;
}
              
/****************************************************************/

void save_userids( )
{
     FILE *f;
     userid_t *mover;
     int a;

     if( !( f = fopen( BASE_DIR "/data/userids", "wb" ) ) )
     {
          debug( 1, "save_userids: failed to open userid file!!" );
          return;
     }

     for( a = 0; a < 26; a++ )
     {
          mover = universe -> userids[ a ];

          while( mover )
          {    
               printf( "saving userid: %s -> %d\n", mover -> name, mover -> userid );
               fwrite( &( mover -> userid ), sizeof( int ), 1, f );
               fwrite( mover -> name, sizeof( char ), strlen( mover -> name ) + 1, f );

               mover = mover -> next;
          }
     }

     fclose( f );
}
         


/* 1 success, 0 failure */
int load_userids( )
{
     FILE *f;
     char *data;      
     int32 userid;
     int32 max;

     printf( "Loading user ids....\n" );

     if( !( f = fopen( BASE_DIR "/data/userids", "rb" ) ) )
     {
          debug( 1, "load_userids: failed to open userid file!!" );
          return 0;
     }

     max = -1;
     while( !feof( f ) )
     {
          if( fread( &userid, sizeof( int32 ), 1, f ) )
          {
               data = fread_string( f );

               if( userid > max ) max = userid;

               user_add_userid( data, userid );
          }
     }
     universe -> res_count = max + 1;

     printf( "Done\n" );

     return 1;
}

#define PASSWORD           1
#define FLAGS              2
#define DEFAULT_NAME       3
#define GENDER             4
#define DEBUG              5
#define FIRST_SAVE_STAMP   6
#define TOTAL_TIME         7
#define TOTAL_CONNECTIONS  8
#define TOTAL_BYTES_IN     9
#define TOTAL_BYTES_OUT    10
#define PROMPT             11
#define TITLE              12

void save_user( user_t *u )
{         
     char fname[ 255 ];
     FILE *f;     
     plugin_s *pmover, *pnext;
     void ( *fptr ) ( );
     env_t *envs;
          
     if( !( u -> sys_flags & USF_LOGGED_IN ) )
          return;

     if( !( u -> sys_flags & USF_SAVED ) )
     {
//          u -> userid = universe -> res_count + 1;
          u -> userid = universe -> res_count;
          universe -> res_count++;

          user_add_userid( u -> current_name, u -> userid );
          save_userids( );

          u -> first_save_stamp = ( uint32 ) time( 0 );
          
          system_output( u, "You have been assigned user id %d\n", u -> userid );
          sys_announce( "%s becomes a saved user", u -> current_name );
     }

     sprintf( fname, "%s/data/users/%d.base", BASE_DIR, u -> userid );

     if( !( f = fopen( fname, "wb" ) ) )
     {
          system_output( u, "An error occured while saving your user data!\n" );
          return;
     }

     fwritew( PASSWORD,          STRING, u -> password,   f );
     fwritew( FLAGS,             UINT32,  &( u -> flags ), f );

     fwritew( GENDER,            UINT8, &( u -> gender ), f );
     fwritew( DEBUG,             UINT8,  &( u -> debug ), f );
     fwritew( FIRST_SAVE_STAMP,  UINT32, &( u -> first_save_stamp ), f );
     fwritew( TOTAL_TIME,        UINT32, &( u -> total_time ), f );
     fwritew( TOTAL_CONNECTIONS, UINT32, &( u -> total_connections ), f );
     fwritew( TOTAL_BYTES_IN,    UINT32, &( u -> total_bytes_in ), f );
     fwritew( TOTAL_BYTES_OUT,   UINT32, &( u -> total_bytes_out ), f );

     if( u -> prompt )
          fwritew( PROMPT,            STRING, u -> prompt,   f );

     if( u -> title )
          fwritew( TITLE,             STRING, u -> title,    f );

     fclose( f );

     /* call plugins save functions */
     pmover = plugin_head;

     while( pmover )
     {                          
          pnext = pmover -> next;

          PLUGIN_SYM( pmover -> handle, "PLUGIN_SAVE_UDATA", fptr );
          if( fptr != NULL )
          {
               if( env_push( pmover, "%s save udata hook", pmover -> name ) )
               {
                    debug( 5, "Failed to push env in save udata plugin hooks execution" );
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


     u -> sys_flags |= USF_SAVED;
     printf( "user saved %s\n", u -> current_name );
     system_output( u, "User saved\n" );
}
   

/* 0 failure, >0 success */
int load_user( user_t *u )
{
     char fname[ 255 ];
     FILE *f;
     uint16 elem;

     sprintf( fname, "%s/data/users/%d.base", BASE_DIR, u -> userid );

     f = fopen( fname, "rb" );
     if( f == NULL )
          return 0;

     while( fread( &elem, sizeof( uint16 ), 1, f ) )
     {
          switch( elem )
          {
               case PASSWORD :
                    u -> password = fread_string( f );
                    break;
               case FLAGS :
                    u -> flags = fread_uint32( f );
                    break;
               case GENDER :
                    u -> gender = fread_uint8( f );
                    break;
               case DEBUG :
                    u -> debug = fread_uint8( f );
                    break;
               case FIRST_SAVE_STAMP :
                    u -> first_save_stamp = fread_uint32( f );
                    break;
               case TOTAL_TIME :
                    u -> total_time = fread_uint32( f );
                    break;
               case TOTAL_CONNECTIONS :
                    u -> total_connections = fread_uint32( f );
                    break;
               case TOTAL_BYTES_IN :
                    u -> total_bytes_in = fread_uint32( f );
                    break;
               case TOTAL_BYTES_OUT :
                    u -> total_bytes_out = fread_uint32( f );
                    break;
               case PROMPT :
                    u -> prompt = fread_string( f );
                    break;
               case TITLE :
                    u -> title = fread_string( f );
          }
     }

     fclose( f );

     return 1;     
}

