/*
 *   purpose : plugin handling
 *   
 *   authors : James Garlick
 *
 */

#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>           
                               
#include "core_headers.h"

plugin_s *plugin_head;

int plugin_loaded( char *name )
{                  
       plugin_s *mover = plugin_head;

       while( mover != NULL )
       {
               if( ! strcmp( mover -> name, name ) )
                       return 1;

               mover = mover -> next;
       }

       return 0;
}

char *plugin_get_name( plugin_t handle )
{                  
       plugin_s *mover = plugin_head;

       while( mover != NULL )
       {
               if( mover -> handle == handle )
                       return mover -> name;

               mover = mover -> next;
       }

       return "unknown";
}                  

plugin_s *plugin_find( char *name )
{
     plugin_s *mover;

     mover = plugin_head;

     while( mover )
     {            
          if( !strcasecmp( mover -> name, name ) || !strcasecmp( mover -> path, name ) )
               return mover;

          mover = mover -> next;
     }

     return NULL;
}

/* returns 0 on success */
int plugin_register( char *plugin_name )
{
        plugin_t handle;
        void ( *plugin_init )( );
        plugin_s *new;
        plugin_s *mover = plugin_head;
        char full_path[ 255 ];          
        char * ( *fptr ) ( );
        char *start, *ptr;
        int count, a;
        struct stat sbuf;
        env_t *envs;

        if( plugin_find( plugin_name ) != NULL )
        {
             debug( 4, "Plugin \'%s\' is already loaded", plugin_name );
             return 1;
        }

        if( *plugin_name != '/' )
               sprintf( full_path, "%s/bin/plugins/%s", BASE_DIR, plugin_name );
        else
               strcpy( full_path, plugin_name );

        handle = NULL;

        handle = PLUGIN_OPEN( full_path );
        if( handle == NULL )
        {             
             perror( dlerror( ) );  
             return -1;
        }        

        stat( full_path, &sbuf );

        if( handle == NULL )
        {                                  
#ifdef HPUX
                debug( 4, "Plugin load error: %s: %s", plugin_name, PLUGIN_ERROR );
#else
                debug( 4, "Plugin load error: %s", PLUGIN_ERROR );
#endif
                return 1;
        } 

        new = ALLOC( 1, plugin_s );
        
        new -> handle   = handle;
        new -> path     = strdup( plugin_name );
        new -> loaded   = time( 0 );                
        new -> compiled = sbuf.st_ctime;

        /*** NAME ***/

        PLUGIN_SYM( handle, "_name", fptr );
        if( fptr == NULL )
        {
                debug( 3, "Plugin load error: %s: No plugin name!", plugin_name );
                PLUGIN_CLOSE( handle );
                FREE( new );
                return 1;
        }
        new -> name = fptr( );
                      
        /*** VERSION ***/

        PLUGIN_SYM( handle, "_version", fptr );
        if( fptr == NULL )
        {
                debug( 3, "Plugin load error: %s: No plugin version!", plugin_name );
                PLUGIN_CLOSE( handle );
                FREE( new );
                return 1;
        }
        new -> version = fptr( );
                    
        /*** DEPENDENCIES ***/

        PLUGIN_SYM( handle, "_dependencies", fptr );
        if( fptr == NULL )
        {
                debug( 3, "Plugin load error: %s: No plugin dependencies list!", plugin_name );
                PLUGIN_CLOSE( handle );
                FREE( new );
                return 1;
        }
        start = strdup( fptr( ) );

        /* count number of deps */
        ptr   = start;

        count = ( *ptr ) ? 1 : 0;
        while( *ptr )
        {          
             if( *ptr == ',' )
             {
                  *ptr = 0;
                  count++;
             }
             ptr++;
        }

        /* set up dependencies list */
        new -> dependencies = ALLOC( count + 1, char * );

        ptr   = start;
        for( a = 0; a < count; a++ )
        {
             while( *ptr == ' ' ) ptr++;

             *( new -> dependencies + a ) = ptr;
             
             while( *ptr ) ptr++;
             ptr++;
        }

        /*** AUTHORS ***/

        PLUGIN_SYM( handle, "_authors_list", fptr );
        if( fptr == NULL )
        {
                debug( 3, "Plugin load error: %s: No plugin authors list!", plugin_name );
                PLUGIN_CLOSE( handle );
                FREE( new );
                return 1;
        }
        start = strdup( fptr( ) );

        /* count number of authors */
        ptr   = start;

        count = ( *ptr ) ? 1 : 0;
        while( *ptr )
        {          
             if( *ptr == ',' )
             {
                  *ptr = 0;
                  count++;
             }
             ptr++;
        }

        /* set up authors_list */
        new -> authors_list = ALLOC( count + 1, char * );

        ptr   = start;
        for( a = 0; a < count; a++ )
        {
             while( *ptr == ' ' ) ptr++;

             *( new -> authors_list + a ) = ptr;
             
             while( *ptr ) ptr++;
             ptr++;
        }

        if( PLUGIN_SYM( handle, "plugin_init", plugin_init ) )
        {
               if( env_push( NULL, "Plugin initialisation" ) )
               {
                      PLUGIN_CLOSE( new -> handle );

                      free( new -> path );
                      FREE( new -> dependencies );
                      FREE( new -> authors_list );
                      FREE( new );    
                      return 1;
               }
               envs = env_top( );

               if( sigsetjmp( envs -> env, 1 ) )
               {     
                      env_pop( );
                      debug( 3, "Plugin %s crashed during plugin_init!", new -> path );
                      PLUGIN_CLOSE( new -> handle );

                      free( new -> path );
                      FREE( new -> dependencies );
                      FREE( new -> authors_list );
                      FREE( new );    
                      return 1;
               }

               ( *plugin_init ) ( new );

               env_pop( );
        }
        else
        {
                debug( 4, "Plugin load error: No plugin_init" );
                PLUGIN_CLOSE( new -> handle );

                free( new -> path );
                FREE( new -> dependencies );
                FREE( new -> authors_list );
                FREE( new );    

                return 1;

        }

        // add plugin to linked list
        if( mover == NULL )
                  plugin_head = new;
        else
        {
                while( mover->next != NULL )
                        mover = mover->next;
        
                mover->next = new;
        }

        return 0;
}                        

/* returns 0 on success */
int plugin_remove( plugin_s *target )
{                  
        plugin_s *prev,
                 *mover;
        void ( *plugin_fini )( );
        env_t *envs;
        ulist_t *umover;

        if( target == NULL )
               return 1;


        if( PLUGIN_SYM( target -> handle, "plugin_fini", plugin_fini ) )
        {
               if( env_push( NULL, "Plugin fini" ) )
               {
                      debug( 3, "Could not allocated env space during plugin remove!" );
               }
               else
               {
                      envs = env_top( );

                      if( sigsetjmp( envs -> env, 1 ) )
                      {     
                             env_pop( );
                             debug( 3, "Plugin %s crashed during plugin_fini!", target -> path );
                      }
                      else
                      {
                             ( *plugin_fini ) ( target );

                             env_pop( );
                      }
              }
        }

        /* remove any udata */

        umover = universe -> connected;

        while( umover )
        {
             udata_remove( umover -> u, target );

             umover = umover -> next;
        }
         
        /* remove plugin from list */
        mover = plugin_head;
        prev  = NULL;

        while( mover != target )
        {                          
             prev = mover;
             mover = mover -> next;
        }

        if( prev == NULL )
             plugin_head = mover -> next;
        else
             prev -> next = mover -> next;

        /* remove any commands that this plugin has added */
        remove_plugin_commands( target );

        /* close and free the plugin */
        PLUGIN_CLOSE( target -> handle );

        free( target -> path );
        FREE( target -> dependencies );
        FREE( target -> authors_list );
        FREE( target );    

        return 0;

}                

void plugin_remove_all( )
{
        plugin_s *mover, *next;

        void ( *plugin_fini )( );

        mover = plugin_head;

        while( mover )
        {   
                next = mover -> next;
                             
                if( PLUGIN_SYM( mover -> handle, "plugin_fini", plugin_fini ) )
                       ( *plugin_fini ) ( mover -> handle );

                PLUGIN_CLOSE( mover -> handle );

                free( mover -> name );

                FREE( mover );    

                mover = next;
        }
}

/* returns 0 on success */
int plugin_reload( plugin_s *target )
{    
     int r = 1;
     char *path;
             
     if( !target )
          return 1;

     path = strdup( target -> path );

     if( !plugin_remove( target ) )
          r = plugin_register( path );

     free( path );

     return r;
}

char *get_textmessage( char *filename )
{
     static char error_str[255];
     char * ( *f )( );
     plugin_s *plugin;

     plugin = plugin_find( "textfiles.so" );

     if( plugin && PLUGIN_SYM( plugin -> handle, "_get_textmessage", f ) )
          return ( *f ) ( filename );

     strcpy( error_str, "UNABLE TO DISPLAY TEXTFILE: PLUGIN NOT LOADED!\n" );

     return error_str;
}
        
