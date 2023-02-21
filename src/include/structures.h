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

#ifndef __STRUCTURES_H
        #define __STRUCTURES_H     

#include <time.h>

typedef signed   char  int8;
typedef unsigned char  uint8;
typedef signed   short int16;
typedef unsigned short uint16;
typedef signed   int   int32;
typedef unsigned int   uint32;      

#define SOCK_NAWS             ( 1 << 0 )

/* communication types */
#define CSYSTEM               ( 1 << 0 )
#define CPUBLIC               ( 1 << 1 )
#define CPRIVATE              ( 1 << 2 )
#define CINFORM               ( 1 << 3 )
#define CGAMES                ( 1 << 4 )
#define CSOCIALS              ( 1 << 5 )
#define CGROUP                ( 1 << 6 ) /* group creation */

struct user_socket_s
{
     int fd;
     int sent_bytes;
     int received_bytes;

     int flags;
     int x, y;

     char *ip;

     char ibuf[ BUFFER_SIZE + 1 ];
     int  ibuf_len;
};
typedef struct user_socket_s user_socket_t;
     
#define GENDER_NONE   0
#define GENDER_FEMALE 1
#define GENDER_MALE   2

/* user.flags (saved) */
#define UF_COLOUR_ON            ( 1 << 0 )

/* user.sys_flags (unsaved) */
#define USF_LOGGED_IN           ( 1 << 0 )
#define USF_LOGOUT_ANNOUNCE     ( 1 << 1 )
#define USF_LOGOUT_QUIET        ( 1 << 2 )
#define USF_LOGOUT              ( USF_LOGOUT_ANNOUNCE | USF_LOGOUT_QUIET )
#define USF_ADMIN               ( 1 << 3 )
#define USF_SAVED               ( 1 << 4 )

typedef struct udata_s
{    
     char     *name;           /* the name of the data area           */
     plugin_s *plugin;         /* the plugin that is storing the data */
     void     *data;           /* pointer to the data                 */
     
     struct udata_s *next;
} udata_t;

typedef struct user_s
{
     user_socket_t *socket;

     void ( *func )( );

     /* saved */
     int     userid;
     char    *current_name;
     char    *password; 

     uint8   gender;

     uint32  first_save_stamp;    /* timestamp of first save */
     uint32  total_time;          /* total time spent on the system */
     uint32  total_connections;
     uint32  total_bytes_in;
     uint32  total_bytes_out;

     char    *prompt;

     uint32  flags;

     uint8   debug;               /* debug level */

     udata_t *udata;              /* user data list */

     char    *title;
     uint16  species;

     /* unsaved */
     uint32 sys_flags;

     time_t last_activity;        /* the time of the users last activity */
     time_t login_time;           /* the time that the user logged       */

     char *inactivemsg;

} user_t;

typedef struct ulist_s
{                         
     int userid;
     user_t *u;

     struct ulist_s *next;
} ulist_t;

/* bits for command.flags */
#define CMD_SYSTEM   ( 1 << 0  ) 
#define CMD_PUBLIC   ( 1 << 1  )   /* public communication  */
#define CMD_PRIVATE  ( 1 << 2  )   /* private communication */
#define CMD_GROUP    ( 1 << 3  )   /* group manipulation    */
#define CMD_INFO     ( 1 << 4  )   /* setting and getting user info */
#define CMD_SETTINGS ( 1 << 5  )   /* personal settings */
#define CMD_GAMES    ( 1 << 6  ) 
#define CMD_MISC     ( 1 << 7  ) 

#define CMD_ADMIN    ( 1 << 8  )   /* admin only */
#define CMD_INVIS    ( 1 << 9  )   /* invisible  */
#define CMD_BASIC    ( 1 << 10 )   /* basic command that a new user should know */

struct command_s
{
     char *name;          /* the command name */

     void ( *func ) ( );  /* pointer to the command function */
     plugin_s *plugin;    /* the plugin in which this command is located */

     int32 flags;         /* command flags, see above for definitions */
     int   tokens;        /* number of tokens to automatically create */

     struct command_s *sub_commands; /* list of sub commands */

     struct command_s *next;
};
typedef struct command_s command_t;

struct server_socket_s
{
     int fd;

     int current_connections;   /* count of the current connections */
     int connection_count;      /* running total of connections */
};
typedef struct server_socket_s server_socket_t;

/* universe.flags */
#define SHUTDOWN ( 1 << 0 )
          
struct userid_s
{
     char *name;
     int  userid;

     struct userid_s *next;
};
typedef struct userid_s userid_t;

struct universe_s
{
     char            *server_name;
     server_socket_t socket;
     uint32          sys_flags;
     time_t          compile_time;

     time_t          last_tick;

     user_t          *current_user;

     int             shutdown;             /* number of seconds to shutdown */
     char            *shutdown_message;

     command_t       *command_ptrs[ 27 ];

     ulist_t         *connected;           /* list of users currently connected  */
     userid_t        *userids[ 26 ];       /* a list of all uids by name */

     int             res_count;            /* the number of saved users */
     int             new_count;            /* the number of new users connected */

};                         
typedef struct universe_s universe_t;
                            
#endif


