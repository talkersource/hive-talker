/* 
 * history.c 
 * Provides history buffers for other plugins
 *
 * Author: James Garlick
 */

#include <signal.h>             

#include "plugin_headers.h"

#include "history.h"
  
PLUGIN_NAME(    "History Buffers" )
PLUGIN_VERSION( "1.0"  )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "galak" )

history_t *history_new( int size )
{
     history_t *new;
     
     new = ALLOC( 1, history_t );
     
     if( !new )
     	return NULL;
     	
     new -> size = size;

     new -> entries = ALLOC( size, char* );

     new -> latest = new -> entries;
     
     return new;
}

void history_free( history_t *h )
{
     if( *( h -> entries ) )
          FREE( h -> entries );
          
     FREE( h );
}


char *history_get( history_t *h, int index )
{
     char **mover;
     
     mover = h -> latest;
     index--;
     
     while( index )
     {
          if( mover != h -> entries )
               mover--; 
          else
               mover = h -> entries + h -> size;
          index--;
     }
     
     return *mover;
}

void history_set( history_t *h, char *str )
{
     h -> latest++;
                                    
     if( h -> latest - h -> entries > h -> size ) h -> latest = h -> entries;
     
     if( *( h -> latest ) ) free ( *( h -> latest ) );
     
     *( h -> latest ) = str;
}

void plugin_init( plugin_s *p_handle )
{            
     return;
}           

