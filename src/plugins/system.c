#include "plugin_headers.h"

PLUGIN_NAME( "System Commands" )
PLUGIN_VERSION( "0.2"  )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "galak" )
     
/* recursive sub-command count */                                 
int count_sub_commands( command_t *c )
{
     command_t *mover;
     int count;

     if( !c )
          return 0;
       
     mover = c -> sub_commands;
          
     count = 0;

     while( mover )
     {
          count += 1 + count_sub_commands( mover -> sub_commands );     
          mover = mover -> next;
     }

     return count;
}

void commands( user_t *u, int argc, char *argv[] )
{
     command_t *mover;
     int a, count, subcount;
     char count_str[ 80 ];
     string_t *buf;        

     count    = 0;
     subcount = 0;

     mover = NULL;

     if( argc > 1 )
     {
          mover = find_command( argv[ 1 ], universe -> command_ptrs );

          if( !mover )
          {
               command_output( "Unknown command or command-group \'%s\'\n", argv[ 1 ] );
               return;
          }   

          if( !mover -> sub_commands )
          {
               command_output( "This command has no sub-commands to list\n" );
               return;
          }
     }

     buf = string_alloc( "" );

     if( mover )
     {
          string_append_title( u, buf, LEFT, "Sub-Commands of %s", mover -> name );
                  
          mover = mover -> sub_commands;

          while( mover )
          {
                  if( count == 0 )
                       string_append( buf, mover -> name );
                  else
                       string_append( buf, ", %s", mover -> name );
                  
                  count++;
                  
                  mover = mover -> next;
          }
          
          
     }
     else if( u -> sys_flags & USF_ADMIN )
     {               
          string_append_title( u, buf, LEFT, "Administration Commands" );

          for( a = 0; a < 27; a++ )
          {                       
                  mover = universe -> command_ptrs[ a ];

                  while( mover )
                  {
                          if( mover -> flags & CMD_ADMIN )
                          {
                                  if( count == 0 )
                                       string_append( buf, mover -> name );
                                  else
                                       string_append( buf, ", %s", mover -> name );
                              
                                  count++;
                          }
                          mover = mover -> next;
                  }
          }
     }
     else
     {
          string_append_title( u, buf, LEFT, "Available Commands" );

          for( a = 0; a < 27; a++ )
          {
                  mover = universe -> command_ptrs[ a ];

                  while( mover )
                  {
                          if( !( mover -> flags & ( CMD_INVIS | CMD_ADMIN ) ) )
                          {
                                  if( count == 0 )
                                       string_append( buf, mover -> name );
                                  else
                                       string_append( buf, ", %s", mover -> name );

                                  /* add a '+' if the command has sub-commands */
                                  if( mover -> sub_commands )
                                  {
                                       string_append( buf, "+" );
                                       subcount += count_sub_commands( mover );
                                  }                              

                                  count++;
                          }
                          mover = mover -> next;
                  }
          }
     }                          

     string_append( buf, "\n" );
                 
     if( count > 1 )
          sprintf( count_str, "There are %d Commands", count );
     else
          sprintf( count_str, "1 Command" ); 

     if( subcount )
          sprintf( count_str, "%s and %d Sub-Commands", count_str, subcount );
     
     string_append_title( u, buf, LEFT, "%s Listed", count_str );

     command_output( buf -> data );

     string_free( buf );

}

void changes( user_t *u, int argc, char *argv[] )
{
     command_output( get_textmessage( "changes" ) );
}                            

void version( user_t *u, int argc, char *argv[] )
{
     command_output( "\n%s v%s   Compiled %s\n\nhttp://www.cse.dmu.ac.uk/~se98jg/hive\n", 
                     SERVER_NAME, VERSION, ctime( &( universe -> compile_time ) ) );
}

void idea( user_t *u, int argc, char *argv[] )
{
     if( argc < 2 || cstrlen( argv[ 1 ] ) == 0 )
     {
          command_output( "Format: %s <your idea>\n", argv[ 0 ] );
          return;
     }

     add_log( "idea", "%s - %s", u -> current_name, argv[ 1 ] );
     command_output( "Idea logged, thank you\n" );
}

void bug( user_t *u, int argc, char *argv[] )
{
     if( argc < 2 || cstrlen( argv[ 1 ] ) == 0 )
     {
          command_output( "Format: %s <bug description>\n", argv[ 0 ] );
          return;
     }

     add_log( "bug", "%s - %s", u -> current_name, argv[ 1 ] );
     command_output( "Bug logged, thank you\n" );
}

char *debug_levels[] =
{ "None", 
  "Very Important", 
  "Important", 
  "Standard", 
  "Extensive", 
  "Development" 
};

void debugc( user_t *u, int argc, char *argv[] )
{     
     uint8 level;
     int i;      
     string_t *buf;

     if( argc > 1 )
     {
          level = ( uint8 ) atoi( argv[ 1 ] );
     }

     if( argc < 2 || level > 5 )
     {
          buf = string_alloc( "" );

          string_append( buf, "DEBUG LEVELS\n\n" );
          for( i = 0; i < 6; i++ )
          {
               string_append( buf, "%d - %s\n", i, debug_levels[ i ] );
          }
          string_append( buf, "\nYour current debug level is %s\n", debug_levels[ u -> debug ] );

          command_output( buf -> data );

          string_free( buf );
     }
     else
     {
          u -> debug = level;
          command_output( "You set your debug level to %s\n", debug_levels[ u -> debug ] );
     }
}

void quit( user_t *u, int argc, char *argv[] )
{
     command_output( "Goodbye %s\n", u->current_name );
     u->sys_flags |= USF_LOGOUT_ANNOUNCE;
}

void toryhunt( user_t *u, int argc, char *argv[] )
{
     string_t *buf;
     
     buf = string_alloc( "" );
     
     string_append_title( u, buf, LEFT, "Tory Hunt" );
     string_append( buf, get_textmessage( "toryhunt" ) );
     string_append_title( u, buf, LEFT, "2 Tories Found" );

     command_output( buf -> data );
     string_free( buf );      
}

void testcard( user_t *u, int argc, char *argv[] )
{
     command_output( get_textmessage( "testcard" ) );
}

void plugin_init( plugin_s *p_handle )
{          
     add_command( "commands",   commands,   p_handle, CMD_SYSTEM,   0 );

     add_command( "changes",    changes,    p_handle, CMD_SYSTEM,   0 );

     add_command( "version",    version,    p_handle, CMD_SYSTEM,   0 );

     add_command( "idea",       idea,       p_handle, CMD_SYSTEM,   0 );
     add_command( "bug",        bug,        p_handle, CMD_SYSTEM,   0 );
     add_command( "debug",      debugc,     p_handle, CMD_SYSTEM,   0 );

     add_command( "quit",       quit,       p_handle, CMD_SYSTEM,   0 );
     add_command( "exit",       quit,       p_handle, CMD_INVIS,    0 );
     add_command( "bye",        quit,       p_handle, CMD_INVIS,    0 );
     add_command( "disconnect", quit,       p_handle, CMD_INVIS,    0 );

     add_command( "toryhunt",   toryhunt,   p_handle, CMD_INVIS,    0 );

     add_command( "testcard",    testcard,   p_handle, CMD_SYSTEM,   0 );
     add_command( "fingerpaint", testcard,   p_handle, CMD_INVIS, 0 );
}           


