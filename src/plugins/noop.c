#include <signal.h>

#include "plugin_headers.h"
PLUGIN_NAME(    "Pointless" )
PLUGIN_VERSION( "0.0.1a"  )

PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "suB" )
plugin_s *noop_h;


void noop( user_t *u )
{
	command_output( "250 NOOP command successful.\n" );
}
void plugin_init( plugin_s *p_handle )
{            
     noop_h = p_handle;

	add_command ("noop", noop, p_handle, CMD_INVIS ,0);
	
     return;
}

void plugin_fini( plugin_s *p_handle )
{
}
     
