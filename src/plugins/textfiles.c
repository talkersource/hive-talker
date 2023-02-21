/*
 *  Plugin for textfiles
 *  
 *  author: James Garlick
 *
 */

#include <dirent.h>
#include <string.h>      
#include <stdio.h>
#include <stdlib.h>   

#include "plugin_headers.h"

PLUGIN_NAME( "Textfiles" )
PLUGIN_VERSION( "1.0"  )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "galak" )

typedef struct textfile_s
{
     char     *filename;
     string_t *string;

     struct textfile_s *next;
} textfile_t;

textfile_t      *textfiles_head;      /* list of loaded textfiles */

int entry_select( const struct dirent *entry )
{
     return ( entry -> d_name[ 0 ] == '.' ||
               entry -> d_name[ strlen( entry -> d_name )-1 ] == '~' ) 
               ? 0 : 1;
}

/* loads all the files in the given directory */
void textfiles_load( char *dir )
{
     struct dirent **namelist;
     int n;
     FILE *f;
     char str[ 255 ];
     textfile_t *new;
     char filepath[ 512 ];

     n = scandir( dir, &namelist, entry_select, alphasort );
     if( n < 0 )
          perror( "load_textfiles: scandir" );
     else
          while( n-- )
          {
               new = ALLOC( 1, textfile_t );
               new -> filename = strdup( namelist[n]->d_name );
               new -> string = string_alloc( "" );

               sprintf( filepath, "%s%s", dir, new->filename );

               f = fopen( filepath, "r" );
               if( !f )
                    perror( "load_textfiles: fopen" );

               while( fgets( str, 255, f ) )
                    string_append( new -> string, str );

               fclose( f );

               new -> next = textfiles_head;
               textfiles_head = new;

               free( namelist[n] );
          }

     debug( 5, "Textfiles Loaded" );
}              

void textfiles_unload( )
{
     textfile_t *mover, *next;

     mover = textfiles_head;

     while( mover )
     {
          next = mover -> next;
                       
          free( mover -> filename );
          string_free( mover -> string );
          FREE( mover );

          mover = next;
     }

     debug( 5, "Textfiles Unloaded" );
}

textfile_t *get_textfile( char *filename )
{
     textfile_t *mover;

     mover = textfiles_head;

     while( mover != NULL )
     {
          if( !strcmp( mover -> filename, filename ) )
               return mover;

          mover = mover -> next;
     }

     return  NULL;
}

char *_get_textmessage( char *filename )
{
     static char error_str[255];
     textfile_t *tf = get_textfile( filename );

     if( tf )
          return tf -> string -> data;
     
     sprintf( error_str, "MISSING TEXTFILE: \'%s\'!\n", filename );

     return error_str;
}

void plugin_init( plugin_s *p_handle )
{          
     textfiles_load( BASE_DIR "/textfiles/" );
}

void plugin_fini( plugin_s *p_handle )
{
     textfiles_unload( );
}

