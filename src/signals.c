/*
 *   purpose : signal handling
 *             and timer tick
 *
 *   authors : James Garlick
 *
 */

#include <stdio.h> 
#include <stdarg.h>
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>            

#include "core_headers.h"

void sig_init( )
{
     struct sigaction s;                   

     s.sa_flags = 0;

     s.sa_handler = sig_pipe_handler;
     sigaction( SIGPIPE, &s, NULL );

     s.sa_handler = sig_handler;

     sigaction( SIGFPE,  &s, NULL );
     sigaction( SIGILL,  &s, NULL );
     sigaction( SIGQUIT, &s, NULL );
     sigaction( SIGSEGV, &s, NULL );
     sigaction( SIGBUS,  &s, NULL );
     sigaction( SIGTERM, &s, NULL );

     s.sa_handler = handle_interrupt;
     sigaction( SIGINT,  &s, NULL );
}

void sig_pipe_handler( int dummy )
{
     if( universe->current_user )
     {
          if( universe->current_user->sys_flags & USF_LOGGED_IN )
               universe->current_user->sys_flags |= USF_LOGOUT_ANNOUNCE;
          else
               universe->current_user->sys_flags |= USF_LOGOUT_QUIET;
     }
}

void sig_handler( int signal )
{
       switch( signal )      
       {
               case SIGABRT :
               sig_die( "Abort received" );
               break;
               case SIGFPE :
               printf( "Floating point exception (SIGFPE) received\n" );
               handle_error( signal );
               break;
               case SIGILL :
               printf( "Illegal Instruction exception (SIGILL) received\n" );
               handle_error( signal );
               break;
               case SIGQUIT :
               sys_announce( "Quit signal received\n" ); 
               universe -> sys_flags |= SHUTDOWN;
               break;
               case SIGSEGV :       
               printf( "Segmentation violation (SIGSEV) received\n" );
               handle_error( signal );
               break;
               case SIGBUS : 
               printf( "Bus error (SIGBUS) received\n" );
               handle_error( signal );
               break;
               case SIGTERM :
               sys_announce( "Software termination signal received" );
               universe -> sys_flags |= SHUTDOWN;
               break;
       }
}

void sig_die( char *str, ... )
{
     char *out;
         
     va_list argp;

     va_start( argp, str );
     vasprintf( &out, str, argp );
     va_end( argp );         

     sys_announce( out );
     sys_announce( "Immediate Shutdown... I thought you said this shit was stable?" );

     free( out );

     exit( 1 );
}                   

