/*
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *      socket.c
 *            
 *      Authors : James Garlick
 *      Purpose : handling sockets and some of the telnet protocol
 *
 */    


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/telnet.h>
#include <ctype.h>
#include <stdlib.h>
#ifdef HPUX
#include <strings.h>
#endif
#ifdef LINUX
//#include <linux/time.h>
#include <sys/ioctl.h>
#endif

#include "core_headers.h"

/*
#include "adns.h"

static adns_state ads;
*/
int write_sock( int fd, char *buf, int num_bytes )
{
        int remaining, written;

        remaining = num_bytes;

        while( remaining > 0 )
        {
                written = write( fd, buf, remaining );

                if( written < 0 )
                        return( written );

                remaining -= written;
                buf       += written;
        }

        return( num_bytes );
}

int read_char( int fd, unsigned char *c )
{
        if( read( fd, c, 1 ) < 0 )
        {
                switch( errno )
                {
                case EINTR:
                case EAGAIN:
                        return read_char( fd, c );
                        break;

                default:
                        return -1;
                }
        }

        return 1;
}

void erase_char( user_t *u )
{
        if( u -> socket -> ibuf_len > 0 )
        {
                u -> socket -> ibuf_len--;
                u -> socket -> ibuf[ u -> socket -> ibuf_len ] = '\0';
        }
}

void erase_line( user_t *u )
{
        memset( u -> socket -> ibuf, 0, BUFFER_SIZE );
        u -> socket -> ibuf_len = 0;
}

void handle_user_input( user_t *u )
{
        int remaining = 0, r = 1, trunc = 0;
        unsigned char in;

        if( ioctl( u -> socket -> fd, FIONREAD, &remaining ) < 0 )
        {
                u -> sys_flags |= USF_LOGOUT_ANNOUNCE;
                add_log( "socket", "handle_user_input: ioctl" );
                return;
        }

        if( remaining == 0 )
        {
                u -> sys_flags |= USF_LOGOUT_ANNOUNCE;
                return;
        }
      
        u->socket->sent_bytes += remaining;

        while( remaining-- && ( r > 0 ) )
        {
                if( ( r = read_char( u -> socket -> fd, &in ) ) > 0 )
                {
                        switch( in )
                        {
                        case IAC : 

                                if( ( r = read_char( u -> socket -> fd, &in ) ) > 0 )
                                {
                                        remaining--;
                                        switch( in )
                                        {
                                        case IP :
                                                u -> sys_flags |= USF_LOGOUT_ANNOUNCE;
                                                break;
                                        case AYT :
                                                user_write( u, "\n[yes]\n" );
                                                break;
                                        case EC :
                                                erase_char( u );
                                                break;
                                        case EL :
                                                erase_line( u );
                                                break;
                                        case WILL :
                                                if( ( r = read_char( u -> socket -> fd, &in ) ) > 0 )
                                                {
                                                        remaining--;
                                                        switch( in )
                                                        {
                                                        case TELOPT_NAWS :
                                                                if( !( u -> socket -> flags & SOCK_NAWS ) )
                                                                {
                                                                        u -> socket -> flags |= SOCK_NAWS;
                                                                        system_output( u, "%c%c%c", IAC, DO, TELOPT_NAWS );
                                                                }
                                                                break;
                                                        }
                                                }
                                                break;
                                        case WONT :
                                                if( ( r = read_char( u -> socket -> fd, &in ) ) > 0 )
                                                {
                                                        remaining--;
                                                        switch( in )
                                                        {
                                                        case TELOPT_NAWS :
                                                                u -> socket -> flags &= ~SOCK_NAWS;
                                                                break;
                                                        }
                                                }
                                                break;
                                        case DO :
                                                break;
                                        case DONT :
                                                break;
                                        case SB :
                                                if( ( r = read_char( u -> socket -> fd, &in ) ) > 0 )
                                                {
                                                        remaining--;
                                                        switch( in )
                                                        {
                                                        case TELOPT_NAWS :
                                                                if( u->socket->flags & SOCK_NAWS )
                                                                {
                                                                        r = read_char( u -> socket -> fd, &in );
                                                                        if( r == -1 ) break;
                                                                        remaining--;
                                                                        u -> socket -> x = ( uint16 ) ( in << 8 );
                                                                        read_char( u -> socket -> fd, &in );
                                                                        if( r == -1 ) break;
                                                                        remaining--;
                                                                        u -> socket -> x |= ( uint16 ) in;
                                                                        read_char( u -> socket -> fd, &in );
                                                                        if( r == -1 ) break;
                                                                        remaining--;
                                                                        u -> socket -> y = ( uint16 ) ( in << 8 );
                                                                        read_char( u -> socket -> fd, &in );
                                                                        if( r == -1 ) break;
                                                                        remaining--;
                                                                        u -> socket -> y |= ( uint16 ) in;
                                                                        read_char( u -> socket -> fd, &in );
                                                                        if( r == -1 ) break;
                                                                        remaining--;
                                                                        read_char( u -> socket -> fd, &in );
                                                                        if( r == -1 ) break;
                                                                        remaining--;
                                                                        printf( "[New term size : ( %d x %d y )]\n", ( int ) u -> socket -> x, ( int ) u -> socket -> y );
                                                               }
                                                               break;
                                                        }
                                                }
                                                break;
                                        default :
                                                add_log( "socket", "IAC not handled - %d", (int) in );
                                                break;
                                        }
                                }
                                break;  

                        case ( char ) '\n' :
                                if( u -> func )
                                {
                                        ( u -> func ) ( u, u -> socket -> ibuf );
                                        memset( u -> socket -> ibuf, 0, BUFFER_SIZE );
                                        u -> socket -> ibuf_len = 0;
                                }
                                else
                                { 
                                        parse_user_input( u );
                                        memset( u -> socket -> ibuf, 0, BUFFER_SIZE );
                                        u -> socket -> ibuf_len = 0;

                                        if( !( u -> sys_flags & USF_LOGOUT ) && !u -> func )
                                                do_prompt( u );
                                }
                                break;        
                         
                        case 8:       
                        case 127:
                        case -9:
                                erase_char( u );
                                break;
                        default :
                                if( isgraph( ( char ) in ) || in == ' ' )
                                {
                                        if( u -> socket -> ibuf_len < BUFFER_SIZE )
                                        {
                                                u -> socket -> ibuf[ u -> socket -> ibuf_len ] = ( char ) in;
                                                u -> socket -> ibuf_len++;
                                        }
                                        else
                                                trunc = 1;
                                }
                        
                                break;
                        }
                }
        }

        if( r != 1 )
        {
                u -> sys_flags |= USF_LOGOUT_ANNOUNCE;
        }

        if( trunc )
                system_output( u, "Sorry, input truncated to %d characters\n", BUFFER_SIZE );

}

void bind_server( int port )
{
        int sfd;
        static struct sockaddr_in sa;
        int on = 1;

        bzero( ( char * ) &sa, sizeof( sa ) );
        sa.sin_family = AF_INET;
        sa.sin_port = htons( port );

        if( ( sfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
        {
                perror( "bind_server: socket" );
                exit( 1 );
        } 

        ( void ) setsockopt( sfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( on ) );

        if( ioctl( sfd, FIONBIO, &on ) < 0 )
        {
                perror( "bind_server: ioctl" );
                exit( 1 );
        } 

        if( bind( sfd, ( struct sockaddr * ) &sa, sizeof( sa ) ) < 0 )
        {
                perror( "bind_server: bind" );
                exit( 1 );
        } 

        if( listen( sfd, 1 ) < 0 )
        {
                perror( "bind_server: listen" );
                exit( 1 );
        } 

        printf( "Server bound successfully to port %d\n", port );

        universe -> socket.connection_count    = 0;
        universe -> socket.current_connections = 0;
        universe -> socket.fd                  = sfd;
/*
        if( adns_init( &ads, ( adns_if_debug | adns_if_noautosys ), 0 ) )
        {
                perror( "bind_server: adns_init" );
                exit( 1 );
        }
*/
}

void handle_new_connection( int port )
{
        int user_fd;
        user_t *u;
        struct sockaddr_in sa;
        socklen_t addrlen;

        addrlen = ( socklen_t ) sizeof( sa );

        user_fd = accept( port, ( struct sockaddr * ) &sa, &addrlen );
        if( user_fd == -1 )
             return;

        u = login_start( user_fd, strdup( inet_ntoa( sa.sin_addr ) ) );

        /* suggest NAWS to client */
//        system_output( u, "%c%c%c", IAC, DO, TELOPT_NAWS );
}

void serve_connections( )
{
        fd_set fdvar;
        struct timeval tv;
        ulist_t *mover, *next = NULL;
        time_t now;
        int i;

        tv.tv_sec  = 5;
        tv.tv_usec = 0;

        FD_ZERO( &fdvar );

        FD_SET( universe -> socket.fd, &fdvar );
                                              
        for( mover = universe -> connected; mover; mover = mover -> next )
             FD_SET( mover -> u -> socket -> fd, &fdvar );

        if( ( select ( FD_SETSIZE, &fdvar, NULL, NULL, &tv ) ) < 0 )
        {
                if( errno != EINTR )
                {           
                        add_log( "crash", "serve_connections - select error" );
                        universe -> sys_flags |= SHUTDOWN;
                }
                return;
        }

        if( FD_ISSET( universe -> socket.fd, &fdvar ) )
                handle_new_connection( universe -> socket.fd );

        for( mover = universe -> connected; mover; mover = next )
        {
                if( mover -> u )
                        next = mover -> next;

                if( FD_ISSET( mover -> u -> socket -> fd, &fdvar ) )
                {
                        universe -> current_user = mover -> u;
                        handle_user_input( mover -> u );
                }
                universe -> current_user = NULL;

                if( mover -> u -> sys_flags & USF_LOGOUT )
                        logout( mover -> u );
        }   
                  
        now = time( 0 );
              
        if( ( int ) difftime( now, universe -> last_tick ) >= 5 )
        {
/*             printf( "TICK %d\n", ( int ) now );*/
             if( universe -> shutdown > -1 )
             {
                  i = ( int ) difftime( universe -> shutdown, now );
                  i -= ( i % 5 );
                  switch( i )
                  {
                  case 600 :
                  case 300 :
                  case 240 : 
                  case 180 : 
                  case 120 : 
                  case 60  : 
                  case 30  : 
                  case 10  :
      
                          sys_announce( "Shutting down in %s!", string_time( i ) );
                          break;
                  case 5 : 
                          sys_announce( "Good bye" );
                          break;
     
                  case 0 :
                          group_output( CSYSTEM, universe -> connected, "\n\n                 ^y>>>>>>>> ^YServer shutting down now! ^y<<<<<<<<^n\n\n" );
      
                          if( universe -> shutdown_message )
                                  group_output( CSYSTEM, universe -> connected, "^Y-=> %s^n\n", universe->shutdown_message );
     
                          universe -> sys_flags |= SHUTDOWN;
                          break;
                  }
             }

             for( mover = universe -> connected; mover; mover = mover -> next )
             {

                  if( mover -> u -> sys_flags & USF_LOGGED_IN   )
                  {         
                       if( !( mover -> u -> sys_flags & USF_SAVED ) )
                       {
                            i = ( int ) difftime( now, mover -> u -> login_time );
                            i -= ( i % 5 );

                            if( i == 600 )
                                 system_output( mover -> u, "\n ^Y*  ^rIf you do not already have a saved character, you are welcome to ^Y*^n\n"
                                                            " ^Y*  ^rsave your user data if you want to, using the '^Rsave^r' command     ^Y*^n\n"
                                                            " ^Y*  ^rPlease be considerate and only save one character, there is no   ^Y*^n\n"
                                                            " ^Y*  ^rneed for more than one anyway since you can have multiple names  ^Y*^n\n" );
                       }

                       i = inactive_time( mover -> u );
                       i -=( i % 5 );

                       if( i == INACTIVITY_TIMEOUT )
                       {
                            group_output_ex( CINFORM, mover -> u, universe -> connected, "^C<^c< ^N%s suddenly disappears ^c>^C>^n\n", mover -> u -> current_name );
                            system_output( mover -> u, "^C<^c< ^NYou are no longer visible ^c>^C>^n\n" );
                       }
                  }
             }

             universe -> last_tick = now;
        }
}
