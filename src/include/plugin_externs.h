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
 */

#ifndef __PLUGIN_EXTERNS_H
        #define __PLUGIN_EXTERNS_H

/* core functions which can be called by plugins */

/* command */

extern char *next_token( char * );
extern command_t *find_command( char *, command_t ** );
extern void add_command( char *, void ( *f ) ( ), plugin_s *, int32, int );
extern void add_subcommand( char *, char *, void ( *f ) ( ), plugin_s *, int32, int );
extern void remove_command( char * );

/* basecomms */
extern void user_write( user_t *, char * );
extern void user_output( int, user_t *, user_t *, char *, ... );
extern void command_output( char *, ... );
extern void group_output( int, ulist_t *, char *, ... );
extern void group_output_ex( int, user_t *, ulist_t*, char *, ... );
extern void sys_announce( char *, ... );
extern void prompt_user( user_t *, char *, ... );
extern void do_prompt( user_t * );
extern void system_output( user_t *, char *str, ... );

/* tools */
extern char *next_space( char * );
extern void strlwr( char * );       
extern void str_trunc( char *, int );
extern char *string_time( int32 );
extern char *string_time_short( int32 );
extern int cstrlen( char * );
extern string_t *process_output( user_t *, char * );
extern int vasprintf ( char **, const char *, va_list );
extern void string_append_title( user_t *, string_t *, int, char *, ...  );
extern void string_append_line( user_t *, string_t * );
extern int get_index( char );
extern void add_log( char *, char *, ... );
extern void debug( uint8, char *, ... );
extern char *user_title( user_t * );

/* user */
extern ulist_t *ulist_find_by_id( ulist_t *, int );
extern user_t *user_find_by_name( ulist_t *, user_t *, char * );
extern user_t *user_find_on_by_id( user_t *, int );
extern user_t *user_find_on_by_name( user_t *, char * );
extern int user_get_userid( char * );
extern int login_time( user_t * );
extern int inactive_time( user_t * );
extern udata_t *udata_find( user_t *, plugin_s * );
extern udata_t *udata_add( user_t *, plugin_s *, int );
extern void udata_remove( user_t *, plugin_s * );
extern void *udata_get( user_t *, plugin_s *, int );
extern ulist_t *ulist_add( ulist_t *, user_t * );
extern ulist_t *ulist_remove( ulist_t *, user_t * );
extern string_t *ulist_names( ulist_t * );
extern int ulist_count( ulist_t * );
extern int ulist_count_online( ulist_t * );

/* login */
void password_mode( user_t *, int );

/* textfile */
char *get_textmessage( char * );

#endif

