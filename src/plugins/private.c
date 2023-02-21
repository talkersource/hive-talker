#include <signal.h>
#include <string.h>       

#include "plugin_headers.h"

#include "history.h"

PLUGIN_NAME( "Private Communication" )
PLUGIN_VERSION( "0.3"  )
PLUGIN_DEPENDENCIES( "history.so" )
PLUGIN_AUTHORS_LIST( "galak" )

plugin_s *private_h;  /* pointer to the plugin struct */

/* data structure to store users private communications data */
typedef struct private_s
{
     history_t *history;
} private_t;

void put_history( user_t *u, char *format, ... )
{
     private_t *udata;
     char *str;
      
     va_list argp;    

     udata = ( private_t * ) udata_get( u, private_h, sizeof( private_t ) );
     if( udata == NULL )
     {
          debug( 3, "Warning: udata in private.so is NULL for %s", u -> current_name );
          return;
     }
     
     if( udata -> history == NULL )
     {
          debug( 3, "Warning: udata->history in private.so is NULL for %s", u -> current_name );
          return;
     }

     va_start( argp, format );
     vasprintf( &str, format, argp  );
     va_end( argp );

     history_set( udata -> history, str );     

     /* do not free str here as it is used in the history buffer */
}

void history( user_t *u, int argc, char *argv[] )
{
     private_t *udata;
     int i;
     string_t *buf;
     char *str;
     
     udata = ( private_t * ) udata_get( u, private_h, sizeof( private_t ) );
     if( udata == NULL )
     {
          debug( 3, "Warning: udata in private.so is NULL for %s", u -> current_name );
          return;
     }
     
     if( udata -> history == NULL )
     {
          debug( 3, "Warning: udata->history in private.so is NULL for %s", u -> current_name );
          return;
     }

     str = history_get( udata -> history, 1 );
     
     if( !str )
     {
          command_output( "You have not received any private communication\n" );
          return;
     }
     
     buf = string_alloc( "" );
     
     string_append_title( u, buf, LEFT, "Private Communication History" );
     
     for( i = 10; i > 0; i-- )
     {
          str = history_get( udata -> history, i );
          
          if( str )
               string_append( buf, "%2d) %s\n", i, str );
     }

     string_append_line( u, buf );

     command_output( buf -> data );
     
     string_free( buf );
}
                 
void group_tell( user_t *u, int argc, char *argv[] )
{           
     char     format[10];       
     char     format2[10];
     char     *ptr;
     ulist_t  *users;
     user_t   *target, *temp;
     string_t *names;

     users = NULL;

     ptr = strtok( argv[ 1 ], "," );

     while( ptr )
     {    
          target = user_find_on_by_name( u, ptr );

          if( target == NULL )
               return;

          temp = user_find_by_id( users, target, target -> userid );
          if( !temp )
          {

               if( target -> inactivemsg )
                    command_output( " ^W%s is inactive> %s^n\n", target -> current_name, target -> inactivemsg );

               users = ulist_add( users, target );
          }

          ptr = strtok( NULL, "," );
     }

     if ( strchr( argv[ 2 ], '?' ) )
     {
          strcpy( format,  "ask" );
          strcpy( format2, "of "  );
     }
     else if( strchr( argv[ 2 ], '!' ) )
     {
          strcpy( format,  "exclaim" );
          strcpy( format2, "to " );
     }
     else                        
     {
          strcpy( format, "tell" );
          format2[0] = 0;
     }

     names = ulist_names( users );

     group_output( CPRIVATE, users, "^W> %s %ss %s%s \'%s^W\'^n\n", u -> current_name, format, format2, names -> data, argv[ 2 ] );
     command_output( "^WYou %s %s%s \'%s^W\'^n\n", format, format2, names -> data, argv[ 2 ] );

     string_free( names );

}

void tell( user_t *u, int argc, char *argv[] )
{
     char format[10];       
     char format2[10];
     user_t *target;

     if( argc < 3 || cstrlen( argv[ 2 ] ) == 0 )
     {
          command_output( " Format : %s <user(s)> <message>\n", argv[ 0 ] );
          return;
     }

     if( strchr( argv[ 1 ], ',' ) )
     {
          group_tell( u, argc, argv );
          return;
     }

     target = user_find_on_by_name( u, argv[ 1 ] );

     if( target == NULL )
          return;

     if ( strchr( argv[ 2 ], '?' ) )
     {
          strcpy( format,  "ask" );
          strcpy( format2, "of "  );
     }
     else if( strchr( argv[ 2 ], '!' ) )
     {
          strcpy( format,  "exclaim" );
          strcpy( format2, "to " );
     }
     else                        
     {
          strcpy( format, "tell" );
          format2[0] = 0;
     }

     user_output( CPRIVATE, u, target, "^W> %s %ss %syou \'%s^W\'^n\n", u -> current_name, format, format2, argv[ 2 ] );
     command_output( "^WYou %s %s%s \'%s^W\'^n\n", format, format2, target -> current_name, argv[ 2 ] );

     if( !strcmp( format, "tell" ) )
          strcpy( format, "told" );
     else
          sprintf( format, "%sed", format );

     put_history( target, "%s %s %syou \'%s\'", u -> current_name, format, format2, argv[ 2 ] ); 

     if( target -> inactivemsg )
          command_output( " ^W%s is inactive> %s^n\n", target -> current_name, target -> inactivemsg );
}

void group_remote( user_t *u, int argc, char *argv[] )
{           
     char     *ptr;
     ulist_t  *users;
     user_t   *target, *temp;
     string_t *names;

     users = NULL;

     ptr = strtok( argv[ 1 ], "," );

     while( ptr )
     {    
          target = user_find_on_by_name( u, ptr );

          if( target == NULL )
               return;

          temp = user_find_by_id( users, target, target -> userid );
          if( !temp )
          {
               if( target -> inactivemsg )
                    command_output( " ^W%s is inactive> %s^n\n", target -> current_name, target -> inactivemsg );
          
               users = ulist_add( users, target );
          }

          ptr = strtok( NULL, "," );
     }

     names = ulist_names( users );

     group_output( CPRIVATE, users, "^W> %s %s ^W(to %s)^n\n", 
                   u -> current_name, argv[ 2 ], names -> data );

     command_output( "^WYou emote '%s %s^W' to %s^n\n", 
                     u -> current_name, argv[ 2 ], names -> data );

     string_free( names );

}

void remote( user_t *u, int argc, char *argv[] )
{
     user_t *target;

     if( argc < 3 || cstrlen( argv[ 2 ] ) == 0 )
     {
          command_output( " Format : %s <user(s)> <message>\n", argv[ 0 ] );
          return;
     }

     if( strchr( argv[ 1 ], ',' ) )
     {
          group_remote( u, argc, argv );
          return;
     }

     target = user_find_on_by_name( u, argv[ 1 ] );

     if( target == NULL )
          return;

     user_output( CPRIVATE, u, target, "^W> %s %s^n\n", u -> current_name, argv[ 2 ] );
     command_output( "^WYou emote '%s %s^W' to %s^n\n", u -> current_name, argv[ 2 ], target -> current_name );

     put_history( target, "%s %s^n", u -> current_name, argv[ 2 ] );

     if( target -> inactivemsg )
          command_output( " ^W%s is inactive> %s^n\n", target -> current_name, target -> inactivemsg );
}

void rsing( user_t *u, int argc, char *argv[] )
{
     user_t *target;

     if( argc < 3 || cstrlen( argv[ 2 ] ) == 0 )
     {
          command_output( " Format : %s <user(s)> <message>\n", argv[ 0 ] );
          return;
     }

     target = user_find_on_by_name( u, argv[ 1 ] );

     if( target == NULL )
          return;

     user_output( CPRIVATE, u, target, "^W> %s sings o/~ %s ^Wo/~ to you^n\n", u -> current_name, argv[ 2 ] );
     command_output( "^WYou sing o/~ %s ^Wo/~ to %s^n\n", argv[ 2 ], target -> current_name );

     put_history( target, "%s sung o/~ %s ^no/~ to you", u -> current_name, argv[ 2 ] );

     if( target -> inactivemsg )
          command_output( " ^W%s is inactive> %s^n\n", target -> current_name, target -> inactivemsg );
}

void rthink( user_t *u, int argc, char *argv[] )
{
     user_t *target;

     if( argc < 3 || cstrlen( argv[ 2 ] ) == 0 )
     {
          command_output( " Format : %s <user(s)> <message>\n", argv[ 0 ] );
          return;
     }

     target = user_find_on_by_name( u, argv[ 1 ] );

     if( target == NULL )
          return;

     user_output( CPRIVATE, u, target, "^W> %s thinks .oO( %s ^W) to you^n\n", u -> current_name, argv[ 2 ] );
     command_output( "^WYou think .oO( %s ^W) to %s^n\n", argv[ 2 ], target -> current_name );

     put_history( target, "%s thought .oO( %s ^n) to you", u -> current_name, argv[ 2 ] );

     if( target -> inactivemsg )
          command_output( " ^W%s is inactive> %s^n\n", target -> current_name, target -> inactivemsg );
}



int PLUGIN_LOAD_UDATA( user_t *u, void *data )
{
     private_t *udata;
     
     udata = ( private_t * ) data;
     if( udata == NULL )
          return 1;
          
     udata -> history = history_new( 10 );

     return 0;
}

void PLUGIN_SAVE_UDATA( user_t *u )
{

}

void plugin_init( plugin_s *p_handle )
{          
     private_h = p_handle;

     add_command( "tell",      tell,    p_handle, CMD_PRIVATE | CMD_BASIC,    1 );
     add_command( ".",         tell,    p_handle, CMD_INVIS, 1 );
     add_command( ">",         tell,    p_handle, CMD_INVIS, 1 );

     add_command( "remote",    remote,  p_handle, CMD_PRIVATE | CMD_BASIC,    1 );
     add_command( ",",         remote,  p_handle, CMD_INVIS, 1 );
     add_command( "<",         remote,  p_handle, CMD_INVIS, 1 );

     add_command( "rsing",     rsing,   p_handle, CMD_PRIVATE,    1 );
     add_command( "rs",        rsing,   p_handle, CMD_INVIS, 1 );
     add_command( "(",         rsing,   p_handle, CMD_INVIS, 1 );


     add_command( "rthink",    rthink,  p_handle, CMD_PRIVATE,    1 );
     add_command( "rt",        rthink,  p_handle, CMD_INVIS, 1 );
     add_command( "*",         rthink,  p_handle, CMD_INVIS, 1 );

     add_command( "history",   history, p_handle, CMD_INFO,    0 );
     add_command( "review",    history, p_handle, CMD_INVIS, 0 );
}           

void plugin_fini( plugin_s *p_handle )
{
}


