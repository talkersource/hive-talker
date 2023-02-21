/*
 *   The Hive - recovery.c
 *
 *   purpose : error recovery
 *
 *   authors : James Garlick
 *   
 */ 
       
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core_headers.h"

env_t *env_stack;              
int handling_error;
   
/* returns 0 on success */
int env_push( plugin_s *phandle, char *format, ... )
{    
     va_list argp;
     env_t *new;

     new = ALLOC( 1, env_t );

/*     printf( "PUSH ENV\n" );*/
     
     if( new == NULL )
     {
          debug( 1, "env_push: out of memory!" );
          return 1;
     }

     new -> plugin = phandle;
     
     va_start( argp, format ); 
     vsnprintf( new -> action, 254, format, argp );
     va_end( argp );
     new -> action[ 254 ] = 0;
                            
     new -> next = env_stack;

     env_stack = new;   

     return 0;
}

env_t *env_top( )
{
     return env_stack;
}

void env_pop( )
{    
     env_t *next;

/*     printf( "POP ENV\n" );*/

     if( env_stack == NULL )
     {
          debug( 5, "env_pop: env_stack == NULL!" );
          return;
     }

     next = env_stack -> next;

     FREE( env_stack );

     env_stack = next;
}

void handle_error( int signal )
{                      
     char s[ 100 ];
     int r;

     if( handling_error )
          exit( 1 );

     handling_error = 1;

     switch( signal )
     {
          case SIGFPE :
          strcpy( s, "Floating point exception" );
          break;
          case SIGILL :
          strcpy( s, "Illegal instruction exception" );
          break;
          case SIGSEGV :       
          strcpy( s, "Segmentation violation" );
          break;
          case SIGBUS : 
          strcpy( s, "Bus error" );
          break;
          default :
          sprintf( s, "Unhandled signal (%d)", signal );
     }

     if( env_stack != NULL )
     {
          r = rand( ) % 4;

          switch( r )
          {
               case 0 : debug( 5, "We get signal" ); break;
               case 1 : debug( 5, "What in the blazes is going on!?" ); break;
               case 2 : debug( 5, "Once again that oaf has ruined my day!" ); break;
               case 3 : debug( 5, "Buffoon!!" ); break;
          }   

          if( env_stack -> plugin != NULL )
          {
               add_log( "crash", "%s received from \'%s\' in plugin \'%s\'",
                        s, env_stack -> action, env_stack -> plugin -> name );

               if( !plugin_reload( env_stack -> plugin ) )
               {
                    debug( 3, "Plugin reloaded, continuing" );
                    handling_error = 0;
                    siglongjmp( env_stack -> env, 1 );
               }
               else
               {
                    debug( 3, "Failed to reload plugin during crash recovery" );
                    debug( 3, "Trying to continue anyway..." );
                    handling_error = 0;
                    siglongjmp( env_stack -> env, 1 );
               }
          }
          else
          {
               add_log( "crash", "%s received from \'%s\' in core",
                        s, env_stack -> action );

               handling_error = 0;
               siglongjmp( env_stack -> env, 1 );
          }
     }
     else
     {
          sig_die( "%s received in core program !!", s);
     }
     handling_error = 0;
}
 
/*  this is intended as a temporary way of  */
/*  recovering from infinite loops          */

void handle_interrupt( int dummy )
{
     if( env_stack == NULL )
          return;

     if( env_stack -> plugin != NULL )
     {
          add_log( "crash", "SIGINT used to abort \'%s\' in plugin \'%s\'", 
               env_stack -> action, env_stack -> plugin -> name );
     }
     else
     {
          add_log( "crash", "SIGINT used to abort \'%s\' in core",
               env_stack -> action );
     }
     siglongjmp( env_stack -> env, 1 );

}
