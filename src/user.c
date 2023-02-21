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

#include "include/core_headers.h"
               
#include <stdio.h>
#include <stdlib.h>     
#include <string.h>
#include <ctype.h>     
#include <unistd.h>
#include <sys/socket.h>

user_t *user_alloc( int user_fd, char *ip )
{
     user_t *new;

     new                 = ALLOC( 1, user_t );
     new -> socket       = ALLOC( 1, user_socket_t );
     new -> socket -> fd = user_fd;
     new -> socket -> x  = DEFAULT_TERMSIZE_X;
     new -> socket -> y  = DEFAULT_TERMSIZE_Y;
     new -> socket -> ip = ip;

     return new;
}                                            

void user_free( user_t *u )
{
     if( u -> socket )
     {
          if( u -> socket -> ip )
               free( u -> socket -> ip );
          FREE( u -> socket );
     }

     if( u -> current_name ) free( u -> current_name );
     if( u -> password )     free( u -> password );

     FREE( u );
}

/* get userid from name - return -1 if not found */
int user_get_userid( char *name )
{
       userid_t *mover;
       int       a;

       a = get_index( *name );

       mover = universe -> userids[ a ];
               
       while( mover )
       {      
            if( !strcasecmp( name, mover -> name ) )
                 return mover -> userid;

               mover = mover -> next;       
       }                                      

     return -1;
}

void user_add_userid( char *name, int userid )
{
       userid_t *new;
       int a;
             
       new = ALLOC( 1, userid_t );          
       if( !new )
            return;

       new -> name = strdup( name );
       new -> userid  = userid;
                   
       a = get_index( *name );
       
       new -> next = universe -> userids[ a ];
       universe -> userids[ a ] = new;
}

/* add a user to a ulist, returns the new start to the ulist */
ulist_t *ulist_add( ulist_t *ulist, user_t *u )
{
     ulist_t *new;
     ulist_t *mover;
     
     new = ALLOC( 1, ulist_t );
     if( !new )
          return ulist;
     
     new -> u      = u;
     new -> userid = u -> userid;
     new -> next   = NULL;

     /* list is empty, return the new start */
     if( !ulist )
          return new;

     /* list is not empty, add new element to the end */

     mover = ulist;

     while( mover -> next )
          mover = mover -> next;

     mover -> next = new;     

     return ulist;
}

/* remove a user from a ulist, returns the new start to the ulist */
ulist_t *ulist_remove( ulist_t *ulist, user_t *u )
{
     ulist_t *mover, *prev;

     prev = NULL;
     mover = ulist;

     while( mover )
     {
            if( mover -> u == u )
            {
                    if( prev )
                            prev -> next = mover -> next;
                    else
                            ulist = mover -> next;

                    FREE( mover );

                    return ulist;
            }

            prev  = mover;
            mover = mover -> next;
     }
     
     return ulist; 
}

/*  returns a formatted list of the current  *
 *  names of the users in a ulist            */
string_t *ulist_names( ulist_t *ul )
{
     string_t *buf;
     ulist_t *mover;
     int count;

     buf = string_alloc( "" );

     mover = ul;
     count = 0;     

     while( mover )
     {
          if( mover -> u )
          {
               if( !mover -> next && count )
               {
                    string_append( buf, " and " );     
               }
               else if( count )
               {
                    string_append( buf, ", " );
               }                                           

               string_append( buf, mover -> u -> current_name );
               count ++;
          }
     
          mover = mover -> next;
     }                          

     return buf;
}


/* returns the number of users in the given ulist */
int ulist_count( ulist_t *ul )
{
     ulist_t *mover;
     int count;

     mover = ul;
     count = 0;

     while( mover )
     {
          count++;
          mover = mover -> next;
     }                        

     return count;
}

/* returns the number of online users in the given ulist */
int ulist_count_online( ulist_t *ul )
{
     ulist_t *mover;
     int count;

     mover = ul;
     count = 0;

     while( mover )
     {
          if( mover -> u )
               count++;

          mover = mover -> next;
     }                        

     return count;
}
              
/* returns the ulist_t pointer for the user or NULL if the user is not in the list */
/* regardless of whether or not they are connected */
ulist_t *ulist_find_by_id( ulist_t *ul, int userid )
{
     ulist_t *mover;

     mover = ul;

     while( mover )
     {                          
          if( mover -> userid == userid )
               return mover;

          mover = mover -> next;
     }

     return NULL;     
}

user_t *user_find_on_by_id( user_t *u, int userid )
{
     ulist_t *mover;

     mover = universe -> connected;

     while( mover )
     {                          
          if( ( mover -> u -> sys_flags & USF_LOGGED_IN ) &&
              mover -> userid == userid )
               return mover -> u;

          mover = mover -> next;
     }

     return NULL;     
}


user_t *user_find_by_name( ulist_t *ul, user_t *u, char *name )
{                  
       ulist_t    *mover;
       user_t    *store;
       int       count;
       char      temp[ 100 ];
       string_t  *namelist;

       mover = ul;
                      
       /* try an exact match */
       while( mover )
       {      
            if( ( mover -> u -> sys_flags & USF_LOGGED_IN ) &&
                !strcasecmp( name, mover -> u -> current_name ) )
            {
                 if( mover -> u == u )
                 {
                      system_output( u, "Silly\n" );
                      return NULL;
                 }

                 return mover -> u;
            }

            mover = mover -> next;       
       }                                      
                               
       /* try a partial match */

       strlwr( name );

       count = 0;
       mover = ul;
       while( mover )
       {      
            if( mover -> u -> sys_flags & USF_LOGGED_IN )
            {
                 strcpy( temp, mover -> u -> current_name );
                 strlwr( temp );

                 /* try lower case and standard case partial matching */
                 if( ( strstr( temp, name ) == temp ) )
                 {
                      count ++;
                      store = mover -> u;
     
                      if( count == 1 )
                           namelist = string_alloc( store -> current_name );
                      else
                           string_append( namelist, ", %s", store -> current_name );

                 }
            }
            mover = mover -> next;       
       }                                      
       if( count == 1 )
       {
            string_free( namelist );

            if( store == u )
            {
                 system_output( u, "Silly\n" );
                 return NULL;
            }

            return store;
       }                
       else if( count > 1 )
       {
            system_output( u, "Multiple name matches : %s\n", namelist -> data );
            string_free( namelist );
       }
       else
            system_output( u, "Unknown user \'%s\'\n", name ); 

       return NULL;
}                   

user_t *user_find_on_by_name( user_t *u, char *name )
{                  
       ulist_t    *mover;
       user_t    *store;
       int       count;
       char      temp[ 100 ];
       string_t  *namelist;

       mover = universe -> connected;
                      
       /* try an exact match */
       while( mover )
       {      
            if( ( mover -> u -> sys_flags & USF_LOGGED_IN ) &&
                !strcasecmp( name, mover -> u -> current_name ) )
            {
                 if( mover -> u == u )
                 {
                      system_output( u, "Silly\n" );
                      return NULL;
                 }

                 return mover -> u;
            }

            mover = mover -> next;       
       }                                      
                               
       /* try a partial match */

       strlwr( name );

       count = 0;
       mover = universe -> connected;
       while( mover )
       {      
            if( mover -> u -> sys_flags & USF_LOGGED_IN )
            {
                 strcpy( temp, mover -> u -> current_name );
                 strlwr( temp );

                 /* try lower case and standard case partial matching */
                 if( ( strstr( temp, name ) == temp ) )
                 {
                      count ++;
                      store = mover -> u;
     
                      if( count == 1 )
                           namelist = string_alloc( store -> current_name );
                      else
                           string_append( namelist, ", %s", store -> current_name );

                 }
            }
            mover = mover -> next;       
       }                                      
       if( count == 1 )
       {
            string_free( namelist );

            if( store == u )
            {
                 system_output( u, "Silly\n" );
                 return NULL;
            }

            return store;
       }                
       else if( count > 1 )
       {
            system_output( u, "Multiple name matches : %s\n", namelist -> data );
            string_free( namelist );
       }
       else
            system_output( u, "Unknown user \'%s\'\n", name ); 

       return NULL;
}                   

int login_time( user_t *u )
{
     return ( int ) difftime( time( 0 ), u -> login_time );
}

int inactive_time( user_t *u )
{
     return ( int ) difftime( time( 0 ), u -> last_activity );
}

/*** DATA AREAS FOR PLUGINS ***/
     
udata_t *udata_find( user_t *u, plugin_s *plugin )
{
     udata_t *mover;

     mover = u -> udata;

     while( mover )
     {                          
          if( mover -> plugin == plugin )
               return mover;

          mover = mover -> next;
     }                      

     return NULL;
}
                                                
/* add udata to a single user */
udata_t *udata_add( user_t *u, plugin_s *plugin, int size )
{
     udata_t *new;

     if( udata_find( u, plugin ) != NULL )
     {
          debug( 5, "udata for plugin \'%s\' already exists!", plugin -> name );
          return NULL;
     }

     new = ALLOC( 1, udata_t );
     if( new == NULL )
     {
          debug( 5, "couldn't alloc new udata_t in udata_add!" );
          return NULL;
     }

     new -> plugin = plugin;
     new -> data = ALLOCS( 1, size );

     if( !new -> data )
     {      
          FREE( new );
          debug( 5, "couldn't alloc new -> data in udata_add!" );
          return NULL;
     }                

     new -> next = u -> udata;
     u -> udata = new;

     return new;
}

void udata_remove( user_t *u, plugin_s *plugin )
{
     udata_t *prev, *mover;

     mover = u -> udata;
     prev  = NULL;

     while( mover )
     {                          
          if( mover -> plugin == plugin )
          {
//               debug( 5, "UDATA REMOVE: %s: %s", plugin -> name, u -> current_name );
               if( prev == NULL )
                    u -> udata = mover -> next;
               else
                    prev -> next = mover -> next;

               FREE( mover -> data );
               FREE( mover );
               return;
          }
          
          prev  = mover;
          mover = mover -> next;
     }
}

/* returns  0 on success      */
/*         >0 file load error */
/*         <0 plugin error    */
int udata_load( user_t *u, udata_t *udata, plugin_s *plugin )
{
     int ( *fptr ) ( );

     PLUGIN_SYM( plugin -> handle, "PLUGIN_LOAD_UDATA", fptr );
     if( fptr == NULL )
     {
          /* there is no udata loader in the plugin */
          return -1;
     }

     return fptr( u, udata -> data );
}

void *udata_get( user_t *u, plugin_s *plugin, int size )
{
     udata_t *udata;
     void ( *fptr ) ( );
     int result;

     udata = udata_find( u, plugin );

     if( udata == NULL )
     {
//          debug( 5, "UDATA_FIND: NOT FOUND" );
          udata = udata_add( u, plugin, size );
          if( udata == NULL )
          {
               debug( 3, "udata: couldn't add new udata!" );
               return NULL;
          }

          result = udata_load( u, udata, plugin );
          if( result != 0 )
          {
               /* udata load failed, try to set default values */
               PLUGIN_SYM( plugin -> handle, "PLUGIN_SET_UDATA_DEFAULTS", fptr );
               if( fptr )
                    fptr( udata -> data );
          }
     }
/*     else
     {
          debug( 5, "UDATA_FIND: FOUND %d", udata );
     }
*/
     return udata -> data;
}


