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
 *
 *      Prototypes should only be placed in here if the function should
 *      only be accessible to the core, and not the plugins, otherwise it
 *      should be placed in plugin_externs.h
 *
 */

#ifndef __CORE_EXTERNS_H
        #define __CORE_EXTERNS_H

#include <stdarg.h>       

#include "plugin_externs.h"

/* socket */               
extern int write_sock( int, char *, int );
extern void bind_server( int );
extern void serve_connections( void );

/* parse */
extern void parse_user_input( user_t * );
                        
/* login */
extern user_t *login_start( int, char * );
extern void logout( user_t * );
extern void password_ques( user_t * );

/* command */
extern int do_command( user_t *, char *, char *, command_t ** );
extern void remove_plugin_commands( plugin_s * );

/* ccommands */
extern void add_core_commands( void );

/* user */
extern user_t *user_alloc( int, char * );
extern void user_free( user_t * );
void user_add_userid( char *, int );

/* recovery */
extern int handling_error;

#endif

