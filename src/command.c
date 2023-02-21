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
 *      command.c
 *            
 *      Purpose : add / remove / find / execute commands
 *
 *      Authors : James Garlick
 */    
                  
#include "core_headers.h"     

#include <stdio.h>
#include <ctype.h> 
#include <string.h>        
#include <stdlib.h>

command_t *find_command( char *name, command_t **command_list )
{                  
       command_t *mover;
       int       a;


       a = get_index( *name );

       mover = command_list[ a ];
               
       while( mover )
       {      
                   
               if( a == 26 )
               {                               
                       if( *name == *( mover -> name ) )
                               return mover;
               }          
               else
               {
                       if( !strcasecmp( name, mover -> name ) )
                               return mover;
               }

               mover = mover -> next;       
       }                                      

       return NULL;
}                   

command_t *find_subcommand( command_t *command, char *name )
{                  
     command_t *mover;

     mover = command -> sub_commands;

     while( mover )
     {
          if( !strcasecmp( name, mover -> name ) )
               return mover;

          mover = mover -> next;
     }

     return NULL;
}

char **tokenize( char *name, char *body, int rtokens, int *argc )
{
     char **argv;
     int tokens;
     char *ptr;

     argv  = ALLOC( rtokens + 3, char * );
     *argv = name;
                       
     tokens = 0;
     ptr = body;                       
               
     while( ( tokens < rtokens ) && *ptr )
     {
          while( *ptr == ' ' )
               ptr++;

          if( *ptr )
          {
               *( argv + tokens + 1 ) = ptr;
               tokens++;

               while( *ptr && *ptr != ' ' )
                    ptr++;

               if( *ptr )
                    *ptr++ = 0;
          }
     }
     *( argv + tokens + 1 ) = ptr;

     if( !*ptr )
     {
          *argc = tokens + 1;
     }
     else
     {
          *argc = tokens + 2;

          /* strip spaces from end of string */
          while( *ptr ) ptr ++;
          while( *( ptr - 1 ) == ' ' ) ptr --;
          *ptr = 0;
     }          

     return argv;
}

int do_command( user_t *u, char *name, char *body, command_t **command_list )
{
        command_t *c, *sub;
        char **argv;
        int    argc;
        env_t *envs;
        plugin_s *pmover, *pnext;
        int ( *fptr ) ( );
        int used;
                                   
        c = find_command( name, command_list );

        if( c ) /* command found */
        {
               if( ( c->flags & CMD_ADMIN ) && !( u->sys_flags & USF_ADMIN ) )
                    return 0;

               argv = tokenize( name, body, c -> tokens, &argc );
       
               sub = NULL;
               if( argc > 1 )
                    sub = find_subcommand( c, *( argv + 1 ) );


               if( env_push( c -> plugin, "%s command", c -> name ) )
               {
                      FREE( argv );
                      debug( 5, "Failed to push env during command execution" );
                      system_output( u, "Command failed to run correctly, sorry\n" );
                      return -1;
               }
               envs = env_top( );

               if( sigsetjmp( envs -> env, 1 ) )
               {     
                      env_pop( );
                      FREE( argv );
                      system_output( u, "Command failed to run correctly, sorry\n" );
                      return -1;
               }

               if( sub )
                    ( sub -> func ) ( u, argc - 1, argv + 1 );
               else
                    ( c -> func ) ( u, argc, argv );

               env_pop( );

               FREE( argv );

               return 1;
        }
	else  /* command not found */
	{      
               /* search for a dynamic command */
               pmover = plugin_head;

               while( pmover )
               {
                    pnext = pmover -> next;

                    PLUGIN_SYM( pmover -> handle, "PLUGIN_DYNAMIC_COMMAND", fptr );

                    if( fptr != NULL )
                    {
                         if( env_push( pmover, "%s dynamic command hook", pmover -> name ) )
                         {
                              debug( 5, "Failed to push env in dynamic command plugin hooks execution" );
                         }
                         else
                         {
                              envs = env_top( );

                              if( sigsetjmp( envs -> env, 1 ) )
                              {
                                   env_pop( );
                              }
                              else
                              {
                                   argv = tokenize( name, body, 0, &argc );

                                   used = 0;
                                   used = fptr( u, argc, argv );
                                   env_pop( );
                                   FREE( argv ); 
          
                                   if( used )
                                        return 1;
                              }
                         }
                    }

                    pmover = pnext;
               }
	}

        return 0;
}

void add_command( char *name, void ( *func ) ( ), plugin_s *plugin, int32 flags, int tokens )
{
       command_t *new;
       int a;
             
       new = find_command( name, universe -> command_ptrs );

       if( new )
       {
               new -> func   = func; 
               new -> plugin = plugin;
               new -> flags  = flags;
               new -> tokens = tokens;

               return;
       }

       new           = ALLOC( 1, command_t );
       new -> name   = strdup( name );
       new -> func   = func; 
       new -> plugin = plugin;
       new -> flags  = flags;
       new -> tokens = tokens;
                   
       a = get_index( *name );
       
       new -> next = universe -> command_ptrs[ a ];
       universe -> command_ptrs[ a ] = new;
}

void add_subcommand( char *command_name, char *name, void ( *func ) ( ), plugin_s *plugin, int32 flags, int tokens )
{
       command_t *command;
       command_t *new;
       int a;
                          
       command = find_command( command_name, universe -> command_ptrs );

       if( command == NULL )
       {
            debug( 4, "Trying to add sub-command \'%s\' to \'%s\' when command doesn\'t exist!", 
                      name, command_name );
            return;
       }

       new = find_subcommand( command, name );

       if( new )
       {
               new -> func   = func; 
               new -> plugin = plugin;
               new -> flags  = flags;
               new -> tokens = tokens;

               return;
       }

       new           = ALLOC( 1, command_t );
       new -> name   = strdup( name );
       new -> func   = func; 
       new -> plugin = plugin;
       new -> flags  = flags;
       new -> tokens = tokens;
                   
       a = get_index( *name );

       new -> next = command -> sub_commands;
       command -> sub_commands = new;
}

void remove_command( char *name )
{      
       command_t *mover, *prev;
       int a;
                        
       a = get_index( *name );

       mover = universe -> command_ptrs[ a ];
       prev  = NULL;
               
       while( mover )
       {      
               if( !strcasecmp( name, mover -> name ) )
               {
                       if( prev == NULL )
                               universe -> command_ptrs[ a ] = mover -> next;
                       else
                               prev -> next = mover -> next;
                       
                       free( mover -> name );
                       FREE( mover );        

                       return;
               }             

               prev  = mover;
               mover = mover -> next;
       }                                      

       debug( 4, "Tried to remove unknown command: %s\n", name );
}

void remove_plugin_commands( plugin_s *plugin )
{      
       command_t *mover, *prev, *next;
       command_t *subs, *prev2;
       int a;
                        
       for( a = 0; a < 27; a++ )
       {
            mover = universe -> command_ptrs[ a ];
            prev  = NULL;
               
            while( mover )
            {          
                    subs = mover -> sub_commands;
                    prev2 = NULL;

                    /* check sub commands */
                    while( subs )
                    {
                         next = subs -> next;
                                     
                         if( subs -> plugin == plugin )
                         {
                              if( prev2 == NULL )
                                   mover -> sub_commands = subs -> next;
                              else
                                   prev2 -> next = subs -> next;

                              free( subs -> name );
                              FREE( subs );
                         }
                         else
                              prev2 = subs;

                         subs = next;
                    }
                    
                    next = mover -> next;

                    /* check command */
                    if( mover -> plugin == plugin )
                    {
                            if( prev == NULL )
                                    universe -> command_ptrs[ a ] = mover -> next;
                            else
                                    prev -> next = mover -> next;

                            free( mover -> name );
                            FREE( mover );
                    }
                    else
                            prev  = mover;

                    mover = next;
            }
       }
}

