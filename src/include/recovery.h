#ifndef __RECOVERY_H
        #define __RECOVERY_H

#include <setjmp.h>

//extern char       *executing_plugin;
//extern char       executing_action [ ];
//extern sigjmp_buf env[];

//extern void    set_executing( plugin_t, int, char *, ... );
//extern int     set_executing( plugin_t, int, char *, ... );
//extern void    done_executing( void );
         
typedef struct env_s
{
     jmp_buf   env;
     plugin_s *plugin;
     char      action[255];

     struct env_s *next;
} env_t;

extern env_t *env_stack;

extern int    env_push( plugin_s *, char *, ... );
extern env_t *env_top( );
extern void   env_pop( );

extern void handle_error( int );
extern void handle_interrupt( int );

#endif
