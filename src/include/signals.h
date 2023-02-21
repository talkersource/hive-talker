#ifndef __SIGNAL_H
        #define __SIGNAL_H

void sig_init( void );
void sig_pipe_handler( int );
void sig_handler( int );
void sig_die( char *str, ... );
void sig_timer_click( int  );


#endif












