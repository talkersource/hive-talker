/* help.c - Online Help System */
            
#include <ctype.h>
#include <stdio.h>          

#include "plugin_headers.h"

#include "help.h"

PLUGIN_NAME( "Help System" )
PLUGIN_VERSION( "0.8" )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "galak" )

help_entry_t *help_head;
help_entry_t **help_entries;

help_subject_t *help_index[ 27 ];
       
int find_subject( char *subject )
{                  
       help_subject_t *mover;
       int       a;

       a = get_index( *subject );

       mover = help_index[ a ];
               
       while( mover )
       {      
            if( !strcasecmp( subject, mover -> name ) )
                 return mover -> index;

            mover = mover -> next;       
       }                                      
      
       return -1;
}               

void add_subject( char *name, int index )
{
       help_subject_t *new;
       int a;
             
       new           = ALLOC( 1, help_subject_t );
       new -> name   = strdup( name );
       new -> index  = index;
                   
       a = get_index( *name );
       
       new -> next = help_index[ a ];
       help_index[ a ] = new;
}

void help_init( )
{
     FILE *f;
     char str[255], *ptr, *token;
     int index, num_entries;

     help_entry_t *mover, *new;

     f = fopen( BASE_DIR "/help/help", "r" );
     if( !f )   
     {
          printf( "can\'t open help file!\n" );
          return;
     }
                         
     help_head = NULL;
     mover = NULL;
     index = 0;
     while( fgets( str, 255, f ) )
     {
          ptr = str;

          while( isspace( *ptr ) ) ptr++;

          if( strlen( ptr ) == 0 || *ptr == '#' )
               continue;

          if( strstr( ptr, "%1" ) == ptr )
          {
               ptr += 2;
               while( isspace( *ptr ) ) ptr++;
                          
               new = ALLOC( 1, help_entry_t );
               if( !new )  
               {
                    printf( "out of memory in init_help\n" );
                    return;
               }

               if( mover == NULL )
               {
                    help_head = mover = new;
               }
               else
               {
                    mover -> next = new;
                    mover = new;
               }

               new -> raw_text = string_alloc( "" );

               token = strtok( ptr, "\040\t\n\r" );

               while( token )
               {
                    add_subject( token, index );

                    token = strtok( NULL, "\040\t\n\r" );
               }
               index ++;
          }
          else if( mover )
          {
               string_append( mover -> raw_text, str );
          }
     }

     /* construct pointers to help entries */

     num_entries = index;
                                    
     help_entries = ALLOC( num_entries, help_entry_t * );

     mover = help_head;
                            
     for( index = 0; index < num_entries; index ++ )
     {
          *( help_entries + index ) = mover;
          mover = mover -> next;
     }
}

void help_fini( )
{
     help_subject_t *hs_mover, *hs_next;
     help_entry_t *he_mover, *he_next;
     int i;

     /* free help index */

     for( i = 0; i < 27; i++ )
     {
          hs_mover = help_index[ i ];

          while( hs_mover )
          {
               hs_next = hs_mover -> next;

               free( hs_mover -> name );
               FREE( hs_mover );

               hs_mover = hs_next;
          }
     }

     /* free entries list */
     FREE( help_entries );

     /* free entries */
     he_mover = help_head;

     while( he_mover )
     {
          he_next = he_mover -> next;
                             
          string_free( he_mover -> raw_text );
          FREE( he_mover );

          he_mover = he_next;
     }
}

string_t *render_help( user_t *u, char *subject, char *raw_text )
{
     char *token, *raw;
     string_t *s;      
     int a, len, width;
     char heading[80];     

     int top_linebreak = 0,
         bot_linebreak = 0;

     width = u -> socket -> x - 4;

     raw = strdup( raw_text );
     s = string_alloc( "" );

     /* title bar */

     string_append( s, "\n  ^P.....(^YOnline Help System^P)" );

     len = width - 45;
     for( a = 0; a < len; a++ )
          string_append( s, "." );

     string_append( s, "(^R%s^P)", subject );

     len = 18 - strlen( subject );
     for( a = 0; a < len; a++ )
          string_append( s, "." );

     string_append( s, "\n  ^P:%*.*s:\n", width - 2, width - 2, "" );

     token = strtok( raw, "\r\n" );

     while( token )
     {                
          heading[ 0 ] = 0;

          if( strstr( token, "%2" ) == token )
          {
               strcpy( heading, "Format  :" );
               top_linebreak = 1;
          }
          else if( strstr( token, "%3" ) == token )         
          {
               strcpy( heading, "Example :" );
               top_linebreak = 1;
          }
          else if( strstr( token, "%4" ) == token )         
          {
               strcpy( heading, "Aliased to :" );
               bot_linebreak ++;
          }
          else if( strstr( token, "%5" ) == token )         
          {
               strcpy( heading, "See also   :" );
               bot_linebreak ++;
          }

          if( strstr( token, "\\n" ) == token )
               token += 2;

          len = strlen( heading );
        
          if( len )
          {
               token += 2;
               while( *token == ' ' )
                    token ++;

               if( bot_linebreak == 1 )
                    string_append( s, "  ^P:%*.*s:\n", width - 2, width - 2, "" );

               string_append( s, "  ^P:     ^g%s ^G%-*.*s    ^P:\n", heading, width - ( 12 + len ), width - ( 12 + len ), token );
          }
          else
          {
               if( top_linebreak )
               {
                    string_append( s, "  ^P:%*.*s:\n", width - 2, width - 2, "" );
                    top_linebreak = 0;
               }

               string_append( s, "  ^P:     ^c%-*.*s    ^P:\n", width - 11, width - 11, token );
          }

          token = strtok( NULL, "\r\n" );
     }

     string_append( s, "  ^P:%*.*s:\n  ^P:", width - 2, width - 2, "" );
     for( a = 0; a < width - 2; a++ )
          string_append( s, "." );
     string_append( s, ":^n\n\n" );          

     free( raw );

     return s;     
}

void help( user_t *u, int argc, char *argv[] )
{            
     int index;
     help_entry_t *entry;     
     string_t *formatted;

     if( argc < 2 || cstrlen( argv[ 1 ] ) == 0 )
     {
          command_output( "Format: %s <subject>\n", argv[ 0 ] );
          return;
     }           
               
     index = find_subject( argv[ 1 ] );

     if( index == -1 )
     {
          add_log( "help", "%s - %s", u -> current_name, argv[ 1 ] );
          command_output( "No help for subject \'%s\'\n", argv[ 1 ] );
          return;
     }           

     entry = *( help_entries + index );

     if( entry && entry -> raw_text && entry -> raw_text -> data )
     {                   
          formatted = render_help( u, argv[ 1 ], entry -> raw_text -> data );

          if( formatted )
          {
               command_output( "%s\n", formatted -> data );
               string_free( formatted );
          }
     }
}

void plugin_init( plugin_s *p_handle )
{          
     help_init( );

     add_command( "help", help,  p_handle, CMD_SYSTEM, 0 );

     add_command( "?",    help , p_handle, CMD_INVIS, 0 );
     add_command( "info", help , p_handle, CMD_INVIS, 0 );
     add_command( "man",  help , p_handle, CMD_INVIS, 0 );
                   
     return;
}           

void plugin_fini( plugin_s *p_handle )
{
     help_fini( );
}                       

