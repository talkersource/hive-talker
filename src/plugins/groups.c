#include <signal.h>
#include <ctype.h>

#include "plugin_headers.h"
#include "groups.h"

PLUGIN_NAME( "Groups" )
PLUGIN_VERSION( "0.2.5" )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "galak, sub" )

plugin_s *groups_h;     /* pointer to the plugin struct */

group_t *public_head;
                 

/* LOW LEVEL GROUP HANDLING FUNCTIONS */

group_t *group_alloc( char *name, user_t *creator )
{
     group_t *new;

     new               = ALLOC( 1, group_t );
     new -> name       = strdup( name ); 

     if( creator )
          new -> creator_id = creator -> userid;
     else
          new -> creator_id = 0;

     new -> colour     = 'n';

     return new;
}

void group_free( group_t *c )
{
     if( c -> name )
          free( c -> name );

     if( c -> tag )
          free( c -> tag );

     if( c -> desc )
          free( c -> desc );

     FREE( c );
}              

void group_add( group_t *new, group_t **head )
{
     new -> next = *head;
     *head = new;
}

void group_remove( group_t *g, group_t **head )
{
     group_t *mover, *prev;

     mover = *head;
     prev  = NULL;

     while( mover && mover != g )
     {
          prev = mover;
          mover = mover -> next;
     }
                
     if( mover )
     {
          if( prev == NULL )
          {
               *head = mover -> next;
          }
          else
          {
               prev -> next = mover -> next;
          }
     }

     group_free( g );
}

group_t *group_find_by_name( char *name, group_t *head )
{
     group_t *mover;

     mover = head;

     while( mover )
     {
          if( !strcasecmp( name, mover -> name ) )
               return mover;

          mover = mover -> next;
     }

     return NULL;
}

group_t *group_find_by_letter( char letter, group_t *head )
{
     group_t *mover;

     mover = head;

     while( mover )
     {
          if( letter == mover -> letter )
               return mover;

          mover = mover -> next;
     }

     return NULL;
}
                      
/* returns 1 if the name is valid otherwise 0 */
int group_valid_name( char *name, group_t *head )
{
     group_t *mover;
     int len;

     len = strlen( name );

     if( len < 2 )
     {
          command_output( "Sorry, the group name must be at least 2 characters long\n" );
          return 0;                    
     }             

     if( len > MAX_GROUP_NAME )
     {
          command_output( "Sorry, the group name must be at least %d characters shorter\n", len - MAX_GROUP_NAME );
          return 0;
     }

     if( !( *name >= 'a' && *name <= 'z' ) && !( *name >= 'A' && *name <= 'Z' ) )
     {
          command_output( "Sorry, the first letter of the group must be alphabetic\n" );
          return 0;
     }

     mover = head;

     while( mover )
     {
          if( !strcasecmp( name, mover -> name ) )
          {
               command_output( "Sorry, a group with that name already exists\n" );
               return 0;
          }

          mover = mover -> next;
     }

     /* TODO - more checks */

             
     return 1;     
}

/*** TEMPORARY COMMANDS ***/

void say( user_t *u, int argc, char *argv[] )
{                                  
     char format[10];
     char *str;                  

     if( argc < 2 || cstrlen( argv[ 1 ] ) == 0 )
     {
          command_output( "Format : %s <message>\n", argv[ 0 ] );
          return;
     }         

     str = argv[ 1 ];

     if ( strchr( str, '?' ) )
          strcpy( format, "ask" );
     else if( strchr( str, '!' ) )
          strcpy( format, "exclaim" );
     else
          strcpy( format, "say" );

     group_output_ex( CPUBLIC, u, universe -> connected, " %s %ss \'%s^n\'\n", u -> current_name, format, str );
     command_output( "You %s \'%s^n\'\n", format, str );
}

void emote( user_t *u, int argc, char *argv[] )
{
     if( argc < 2 || cstrlen( argv[ 1 ] ) ==  0 )
     {
          command_output( "Format : emote <action>\n" );
          return;
     }

     group_output( CPUBLIC, universe -> connected, " %s %s^n\n", u -> current_name, argv[ 1 ] );
}

void think( user_t *u, int argc, char *argv[] )
{
     char format[10];
     char *str;     
          
     if( argc < 2 || cstrlen( argv[ 1 ] ) == 0 )
     {
          command_output( "Format : %s <thought>\n", argv[ 0 ] );
          return;
     }         
     str = argv[ 1 ];
     
     if ( strchr( str, '?' ) )
          strcpy( format, "wonder" );
     else
          strcpy( format, "think" );

     group_output_ex( CPUBLIC, u, universe -> connected, " %s %ss . o O ( %s ^n)\n", u -> current_name, format, str );
     command_output( "You %s . o O ( %s ^n)\n", format, str );
}

void sing( user_t *u, int argc, char *argv[] )
{
     if( argc < 2 || cstrlen( argv[ 1 ] ) ==  0 )
     {
          command_output( "Format : %s <song>\n", argv[ 0 ] );
          return;
     }

     group_output_ex( CPUBLIC, u, universe -> connected, " %s sings o/~ %s ^no/~\n", u -> current_name, argv[ 1 ] );
     command_output( "You sing o/~ %s ^no/~\n", argv[ 1 ] );
}                                                       

void echo( user_t *u, int argc, char *argv[] )
{
     if( argc < 2 || cstrlen( argv[ 1 ] ) ==  0 )
     {
          command_output( "Format : echo <message>\n" );
          return;
     }

     group_output( CPUBLIC, universe -> connected, " [%s] %s^n\n", u -> current_name, argv[ 1 ] );
}

/* directed global communication */
void dsay( user_t *u, int argc, char *argv[] )
{                                  
     char format[10];
     char format2[10];
     char *str;                  
     user_t *target;

     if( argc < 3 || cstrlen( argv[ 2 ] ) == 0 )
     {
          command_output( "Format : %s <user> <message>\n", argv[ 0 ] );
          return;
     }         

     target = user_find_on_by_name( u, argv[ 1 ] );
     if( target == NULL )
          return;

     str = argv[ 2 ];

     if ( strchr( str, '?' ) ) 
     {
          strcpy( format, "ask" );
          strcpy( format2, "of "  );
     }
     else if( strchr( str, '!' ) )
     {
          strcpy( format, "exclaim" );
          strcpy( format2, "to " );
     }
     else
     {
          strcpy( format, "say" );
          strcpy( format2, "to " );
     }

     group_output_ex( CPUBLIC, u, universe -> connected, " %s %ss \'%s^n\' %s%s\n", u -> current_name, format, str, format2, target -> current_name );
     command_output( "You %s \'%s^n\' %s%s\n", format, str, format2, target -> current_name );
}

void demote( user_t *u, int argc, char *argv[] )
{
     user_t *target;

     if( argc < 3 || cstrlen( argv[ 2 ] ) ==  0 )
     {
          command_output( "Format : %s <user> <action>\n", argv[ 0 ] );
          return;
     }

     target = user_find_on_by_name( u, argv[ 1 ] );
     if( target == NULL )
          return;

     group_output( CPUBLIC, universe -> connected, " %s %s (to %s)^n\n", u -> current_name, argv[ 2 ], target -> current_name );
} 

/* GROUP COMMANDS */

/* list available groups */
void gr_list( user_t *u, int argc, char *argv[] )
{
     group_t *mover;          
     string_t *buf;
     int count;
     
     if( public_head == NULL )
     {
          command_output( "\nThere are no public groups\n\n" );
          return;
     }

     buf = string_alloc( "" );
     string_append_title( u, buf, LEFT, "Public Groups" );

     mover = public_head;
     count = 0;               

     while( mover )
     {
          string_append( buf, "%20.20s - %-50.50s^n\n", mover -> name, mover -> desc );
          count++;               

          mover = mover -> next;
     }

     string_append_title( u, buf, LEFT, "%d Groups Available", count );

     command_output( buf -> data );
     string_free( buf );

}

void gr_create_format( char *cname )
{
     command_output( "\nUse the following sub-commands to setup the group:\n"
                     "  ^W%1$s tag <tag string>^n            (see 'help group_tag')\n"
                     "  ^W%1$s desc <group description>^n    (a short description of the group)\n"
                     "  ^W%1$s colour <colour wand letter>^n (default colour for text output)\n"
                     "  ^W%1$s letter <single letter>^n      (see 'help group_usage')\n"
                     "  ^W%1$s done^n                        (finish and add the group)^n\n",
                     cname );
}

void gr_create( user_t *u, int argc, char *argv[] )
{
     udgroups_t *udata;

     if( u -> userid < 0 )
     {
          command_output( "Sorry, only saved users can create groups!\n" );
          return;     
     }

     udata = ( udgroups_t * ) udata_get( u, groups_h, sizeof( udgroups_t ) );
     if( udata == NULL )
          return;

     if( argc < 2 )
     {
          if( udata -> new )
               gr_create_format( argv[ 0 ] );
          else   
               command_output( "Format: %s <group name> [public|private]\n", argv[ 0 ] );

          return;     
     }
     else if( argc < 3 || !strcasecmp( argv[ 2 ], "public" ) )
     {

          if( !group_valid_name( argv[ 1 ], public_head ) )
               return;
          
          if( udata -> new == NULL )
          {
               udata -> new = group_alloc( argv[ 1 ], u );

               if( udata -> new == NULL )
               {
                    command_output( "Not enough memory to allocate group!\n" );
                    return;
               }
               
          }
          else
          {    
               if( udata -> new -> name )
                    free( udata -> new -> name );

               udata -> new -> name = strdup( argv[ 1 ] );
          }

          gr_create_format( argv[ 0 ] );

     }
     else if( !strcasecmp( argv[ 2 ], "private" ) )
     {
          command_output( "Private groups are not yet implemented\n" );     

     }
     else
     {
          command_output( "Format: %s <group name> [public|private]\n", argv[ 0 ] );
     }
     
}

void gr_create_tag( user_t *u, int argc, char *argv[] )
{
     udgroups_t *udata;                              
     int count;
     char tag[ MAX_GROUP_TAG + 1 ];
     char *c;     

     udata = ( udgroups_t * ) udata_get( u, groups_h, sizeof( udgroups_t ) );
     if( udata == NULL )
          return;

     if( udata -> new == NULL )
     {
          command_output( "You are not currently creating a group!\n" );
          return;
     }

     if( argc < 2 )
     {           
          command_output( "Format: gr_create %s <tag string>\n", argv[ 0 ] );
          return;
     }       

     if( strchr( argv[ 1 ], '^' ) )
     {
          command_output( "Sorry, the tag may not contain colour wands\n" );
          return;
     }

     count = 0;
     c = argv[ 1 ];

     while( *c && count < MAX_GROUP_TAG )
     {        
          if( isalnum( *c ) )
               tag[ count++ ] = *c;

          c++;
     }                  
     tag[ count ] = '\0';   

     if( udata -> new -> tag )
          free( udata -> new -> tag );

     udata -> new -> tag = strdup( tag ); 

     command_output( "The group tag is set to =%s=\n", udata -> new -> tag );
     
}
             
void gr_create_desc( user_t *u, int argc, char *argv[] )
{
     udgroups_t *udata;                              
     int len;

     udata = ( udgroups_t * ) udata_get( u, groups_h, sizeof( udgroups_t ) );
     if( udata == NULL )
          return;

     if( udata -> new == NULL )
     {
          command_output( "You are not currently creating a group!\n" );
          return;
     }

     if( argc < 2 )
     {           
          command_output( "Format: gr_create %s <group description>\n", argv[ 0 ] );
          return;
     }       

     len = cstrlen( argv[ 1 ] );

     if( len > MAX_GROUP_DESC )
     {
          command_output( "Sorry, the description must be at least %d characters shorter (excluding colour wands)\n", len - MAX_GROUP_DESC );
          return;
     }

     if( udata -> new -> desc )
          free( udata -> new -> desc );

     udata -> new -> desc = strdup( argv[ 1 ] ); 

     command_output( "The group description is set to \'%s^n\'\n", udata -> new -> desc );

}

char groupwandlist [] = "yYrRcCpPgGwWnN";

void gr_create_colour( user_t *u, int argc, char *argv[] )
{
     udgroups_t *udata;                              

     udata = ( udgroups_t * ) udata_get( u, groups_h, sizeof( udgroups_t ) );
     if( udata == NULL )
          return;

     if( udata -> new == NULL )
     {
          command_output( "You are not currently creating a group!\n" );
          return;
     }

     if( argc < 2 )
     {           
          command_output( "Format: gr_create %s <default group colour>\n", argv[ 0 ] );
          return;
     }       

     if( !strchr( groupwandlist, *argv[ 1 ] ) )
     {
          command_output( "Invalid colour wand letter (%s)\n", groupwandlist );
          return;
     }
                    
     udata -> new -> colour = *argv[ 1 ];
     command_output( "^%cGroup colour set to \'%c\'^n\n", udata -> new -> colour, udata -> new -> colour );
}

void gr_create_letter( user_t *u, int argc, char *argv[] )                              
{
     udgroups_t *udata;                              
     char c;     
     group_t *found;

     udata = ( udgroups_t * ) udata_get( u, groups_h, sizeof( udgroups_t ) );
     if( udata == NULL )
          return;

     if( udata -> new == NULL )
     {
          command_output( "You are not currently creating a group!\n" );
          return;
     }

     if( argc < 2 )
     {           
          command_output( "Format: gr_create %s <default group letter>\n", argv[ 0 ] );
          return;
     }       
     
     c = tolower( *argv[ 1 ] );

     if( !( c >= 'a' && c <= 'z' ) )
     {
          command_output( "The group letter must be alphabetic\n" );
          return;
     }           
        
     found = group_find_by_letter( c, public_head );

     if( found )
     {
          command_output( "Letter %c is in use by group %s\nPlease choose another letter (This will be fixed in a future version)\n", 
                          c, found -> name );
          return;
     }     

     udata -> new -> letter = c;

     command_output( "Group letter set to \'%c\'\n", udata -> new -> letter );
     
}                      

void gr_create_done( user_t *u, int argc, char *argv[] )
{
     udgroups_t *udata;                              

     udata = ( udgroups_t * ) udata_get( u, groups_h, sizeof( udgroups_t ) );
     if( udata == NULL )
          return;

     if( udata -> new == NULL )
     {
          command_output( "You are not currently creating a group\n" );
          return;
     }          

     if( ! udata -> new -> desc )
     {
          command_output( "You must give the group a description before it can be added\n" );
          return;
     }

     if( ! udata -> new -> tag )
     {
          command_output( "You must give the group a tag before it can be added\n" );
          return;
     }

     if( ! udata -> new -> letter )
     {
          command_output( "You must give the group a letter before it can be added\n" );
          return;
     }
          
     group_add( udata -> new, &public_head );

     group_output( CGROUP, universe -> connected, "^C-=> A new group '%s' has been created by %s^n\n", udata -> new -> name, u -> current_name );
             
     udata -> new -> users = ulist_add( udata -> new -> users, u );
     command_output( "^g-^G>^n %s joins the ^%c%s^n group ^G<^g-^n\n", u -> current_name, udata -> new -> colour, udata -> new -> name );

     udata -> new = 0;

}

void gr_create_abort( user_t *u, int argc, char *argv[] )
{
     udgroups_t *udata;                              

     udata = ( udgroups_t * ) udata_get( u, groups_h, sizeof( udgroups_t ) );
     if( udata == NULL )
          return;

     if( udata -> new == NULL )
     {
          command_output( "You were not creating a group\n" );
          return;
     }                                                              

     group_free( udata -> new );
     command_output( "Group aborted\n" );
}

void gr_destroy( user_t *u, int argc, char *argv[] )
{    
     group_t *g;

     if( argc < 2 )
     {
          command_output( "Format: %s <group name>\n", argv[ 0 ] );
          return;
     }            

     g = group_find_by_name( argv[ 1 ], public_head );

     if( g ==  NULL )
     {
          command_output( "Unknown group \'%s\'\n", argv[ 1 ] );
          return;
     }

     if( u -> userid != g -> creator_id && !( u -> sys_flags & USF_ADMIN ) )
     {
          command_output( "Sorry, only the groups creator can remove it\n" );
          return;
     }

     group_output( CPUBLIC, g -> users, "Group \'%s\' destroyed by %s (user id %d)\n", 
                   g -> name, u -> current_name, u -> userid );

     group_remove( g, &public_head );

     command_output( "Done\n" );
}

void gr_info( user_t *u, int argc, char *argv[] )
{    
     group_t *g;
     string_t *buf;          

     if( argc < 2 )
     {
          command_output( "Format: %s <group name>\n", argv[ 0 ] );
          return;
     }            

     g = group_find_by_name( argv[ 1 ], public_head );

     if( g ==  NULL )
     {
          command_output( "Unknown group \'%s\'\n", argv[ 1 ] );
          return;
     }

     buf = string_alloc( "" );
     
     string_append_title( u, buf, LEFT, "Group %s", g -> name );

     string_append( buf, "%s^n\n", g -> desc );

     string_append_title( u, buf, LEFT, "Defaults" );

     string_append( buf, "    Tag : %s^n\n", g -> tag );
     string_append( buf, " Letter : %c^n\n", g -> letter );
     string_append( buf, " Colour : %c^n\n", g -> colour );

     string_append_line( u, buf );

     command_output( buf -> data );

     string_free( buf );

}
                       
void gr_join( user_t *u, int argc, char *argv[] )
{
     group_t *g;
     ulist_t *found;
     
     if( argc < 2 )
     {
          command_output( "Format: %s <group name>\n", argv[ 1 ] );
          return;
     }            

     g = group_find_by_name( argv[ 1 ], public_head );

     if( g ==  NULL )
     {
          command_output( "Unknown group \'%s\'\n", argv[ 1 ] );
          return;
     }

     found = ulist_find_by_id( g -> users, u -> userid );

     if( found == NULL )
     {                              
          g -> users = ulist_add( g -> users, u );
          group_output( CPUBLIC, g -> users, "^g-^G>^n %s joins the ^%c%s^n group ^G<^g-^n\n", u -> current_name, g -> colour, g -> name );
     }
     else
     {
          command_output( "You are already in the \'%s\' group\n", g -> name );
     }
}

void gr_leave( user_t *u, int argc, char *argv[] )
{
     group_t *g;
     ulist_t *found;
     
     if( argc < 2 )
     {
          command_output( "Format: %s <group name>\n", argv[ 1 ] );
          return;
     }            

     g = group_find_by_name( argv[ 1 ], public_head );

     if( g ==  NULL )
     {
          command_output( "Unknown group \'%s\'\n", argv[ 1 ] );
          return;
     }

     found = ulist_find_by_id( g -> users, u -> userid );

     if( found == NULL )
     {                              
          command_output( "You are not in the \'%s\' group\n", g -> name );

     }
     else
     {
          group_output( CPUBLIC, g -> users, "^R<^r-^n %s leaves the ^%c%s^n group ^r-^R>^n\n", u -> current_name, g -> colour, g -> name );
          g -> users = ulist_remove( g -> users, u );
     }

}

char *get_tag( group_t *g )
{
     static char tag[ 30 ];

     if( g -> tag )
          sprintf( tag, "^W=^%c%s^W=", g -> colour, g -> tag );
     else
          tag[ 0 ] = 0; 

     return tag;   
}

void gr_say( group_t *group, user_t *u, int argc, char *argv[] )
{                                  
     char format[10];
     char *str;                  

     if( argc < 2 || cstrlen( argv[ 1 ] ) == 0 )
     {
          command_output( "Format : %s <message>\n", argv[ 0 ] );
          return;
     }         

     str = argv[ 1 ];
     
     if ( strchr( str, '?' ) )
          strcpy( format, "ask" );
     else if( strchr( str, '!' ) )
          strcpy( format, "exclaim" );
     else
          strcpy( format, "say" );

     group_output_ex( CPUBLIC, u, group -> users, "%s ^%c%s %ss \'%s^%2$c\'^n\n", 
                      get_tag( group ), group -> colour, u -> current_name, format, str );

     command_output( "%s ^%cYou %s \'%s^%2$c\'^n\n", get_tag( group ), group -> colour, format, str );
}

void gr_emote( group_t *group, user_t *u, int argc, char *argv[] )
{
     if( argc < 2 || cstrlen( argv[ 1 ] ) ==  0 )
     {
          command_output( "Format : %s <action>\n", argv[ 0 ] );
          return;
     }

     group_output( CPUBLIC, group -> users, "%s ^%c%s %s^n\n", 
                      get_tag( group ), 
                      group -> colour, 
                      u -> current_name, 
                      argv[ 1 ] 
                 );
}

void gr_think( group_t *group, user_t *u, int argc, char *argv[] )
{
     char format[10];
     char *str;     
          
     if( argc < 2 || cstrlen( argv[ 1 ] ) == 0 )
     {
          command_output( "Format : %s <thought>\n", argv[ 0 ] );
          return;
     }         
     str = argv[ 1 ];
     
     if ( strchr( str, '?' ) )
          strcpy( format, "wonder" );
     else
          strcpy( format, "think" );

     group_output_ex( CPUBLIC, u, group -> users, "%s ^%c%s %ss . o O ( %s ^%2$c)^n\n", 
                      get_tag( group ), group -> colour, u -> current_name, format, str );
     command_output( "%s ^%cYou %s . o O ( %s ^%2$c)^n\n", 
                     get_tag( group ), group -> colour, format, str );
}

void gr_sing( group_t *group, user_t *u, int argc, char *argv[] )
{
     if( argc < 2 || cstrlen( argv[ 1 ] ) ==  0 )
     {
          command_output( "Format : %s <song>\n", argv[ 0 ] );
          return;
     }

     group_output_ex( CPUBLIC, u, group -> users, "%s ^%c%s sings o/~ %s ^%2$co/~^n\n", 
                      get_tag( group ), group -> colour, u -> current_name, argv[ 1 ] );
     command_output( "%s ^%cYou sing o/~ %s ^%2$co/~^n\n", 
                     get_tag( group ), group -> colour, argv[ 1 ] );
}                                                       

void gr_dsay( group_t *group, user_t *u, int argc, char *argv[] )
{                                  
     char format[10];
     char format2[10];
     char *str;                  
     user_t *target;
     char *target_name;

     if( argc < 2 || cstrlen( argv[ 1 ] ) == 0 )
     {
          command_output( "Format : %s <user> <message>\n", argv[ 0 ] );
          return;
     }         

     target_name = argv[ 1 ];
     argv[ 1 ] = next_token( argv[ 1 ]  );

     if( *target_name == '\0' || *argv[ 1 ] == '\0' )
     {
          command_output( "Format : %s <user> <message>\n", argv[ 0 ] );
          return;
     }

     target = user_find_by_name( group -> users, u, target_name );
     if( target == NULL )
          return;

     str = argv[ 1 ];
     
     if ( strchr( str, '?' ) ) 
     {
          strcpy( format, "ask" );
          strcpy( format2, "of "  );
     }
     else if( strchr( str, '!' ) )
     {
          strcpy( format, "exclaim" );
          strcpy( format2, "to " );
     }
     else
     {
          strcpy( format, "say" );
          strcpy( format2, "to " );
     }

     group_output_ex( CPUBLIC, u, group -> users, "%s ^%c%s %ss \'%s^%2$c\' %s%s^n\n", 
                      get_tag( group ), 
                      group -> colour, 
                      u -> current_name, 
                      format, 
                      str,
                      format2,
                      target -> current_name 
                    );

     command_output( "%s ^%cYou %s \'%s^%2$c\' %s%s^n\n", 
                      get_tag( group ), 
                      group -> colour, 
                      format, 
                      str,
                      format2,
                      target -> current_name 
                   );
}

void gr_demote( group_t *group, user_t *u, int argc, char *argv[] )
{
     user_t *target;
     char *target_name;

     if( argc < 2 || cstrlen( argv[ 1 ] ) == 0 )
     {
          command_output( "Format : %s <user> <message>\n", argv[ 0 ] );
          return;
     }         

     target_name = argv[ 1 ];
     argv[ 1 ] = next_token( argv[ 1 ]  );

     if( *target_name == '\0' || *argv[ 1 ] == '\0' )
     {
          command_output( "Format : %s <user> <message>\n", argv[ 0 ] );
          return;
     }

     target = user_find_by_name( group -> users, u, target_name );
     if( target == NULL )
          return;

     group_output( CPUBLIC, group -> users, "%s ^%c%s %s (to %s)^n\n", 
                      get_tag( group ), 
                      group -> colour, 
                      u -> current_name, 
                      argv[ 1 ],
                      target -> current_name 
                 );
}

void gr_echo( group_t *group, user_t *u, int argc, char *argv[] )
{
     if( argc < 2 || cstrlen( argv[ 1 ] ) ==  0 )
     {
          command_output( "Format : echo <message>\n" );
          return;
     }

     group_output( CPUBLIC, group -> users, "%s ^%c[%s] %s^n\n", 
                      get_tag( group ), group -> colour, u -> current_name, argv[ 1 ] );
}

void gr_look( group_t *group, user_t *u, int argc, char *argv[] )
{                                             
     int count;
     string_t *names;

     count = ulist_count_online( group -> users );

     if( count == 1 )
     {
          command_output( "You are the only user in the group\n" );
     }
     else
     {               
          names = ulist_names( group -> users );

          command_output( "There are %d users in the group: %s\n", count, names -> data );

          string_free( names );
     }
}

/*

xu say   
x'
x"

xe emote
x;
x:

xs sing
x(
x)  

xt think
x~
          
x+ echo

xh history

xw who

xi examine

xl look

x> command redirect to group

*/



char dyn_list [] = "u\'\"e;:s()t~+hwxl>[]";

int PLUGIN_DYNAMIC_COMMAND( user_t *u, int argc, char *argv[] )
{   
     group_t *gmover;
     ulist_t *found;
                      
     if( strlen( argv[ 0 ] ) != 2 )
          return 0;

     if( strchr( dyn_list, *( argv[ 0 ] + 1 ) ) == NULL )
          return 0;

     /* temp - find the group */

     gmover = public_head;

     while( gmover && gmover -> letter != *argv[ 0 ] )
          gmover = gmover -> next;

     if( gmover )
     {             
          found = ulist_find_by_id( gmover -> users, u -> userid );

          if( found == NULL )
          {
               command_output( "You are not in the \'%s\' group\n", gmover -> name );
          }
          else
          {
               switch( *( argv[ 0 ] + 1 ) )
               {
                    case 'u'  : 
                    case '\'' :
                    case '\"' : gr_say( gmover, u, argc, argv );
                                break;
                    case 'e'  :
                    case ';'  :
                    case ':'  : gr_emote( gmover, u, argc, argv );
                                break;
                    case 's'  :
                    case '('  : 
                    case ')'  : gr_sing( gmover, u, argc, argv );
                                break;
                    case 't'  :
                    case '~'  : gr_think( gmover, u, argc, argv );
                                break;
                    case '+'  : gr_echo( gmover, u, argc, argv );
                                break;
                    case 'w'  :
                    case 'l'  : gr_look( gmover, u, argc, argv );
                                break;
                    case '['  : gr_dsay( gmover, u, argc, argv );
                                break;
                    case ']'  : gr_demote( gmover, u, argc, argv );
                                break;
               }
          }
     }
     else
     {
          command_output( "There is no group which uses the letter \'%c\'\n", *argv[ 0 ] );
     }            

     return 1;
}

int PLUGIN_LOAD_UDATA( user_t *u, void *data )
{
     udgroups_t *udata;
     
     udata = ( udgroups_t * ) data;
     if( udata == NULL )
          return 1;
          

     return 0;
}

void PLUGIN_SAVE_UDATA( user_t *u )
{

}

void PLUGIN_USER_LOGIN( user_t *u )
{                       
     group_t *mover;
     ulist_t *ulm;          

     mover = public_head;

     while( mover )
     {           
          ulm = mover -> users;

          while( ulm )
          {     
               if( ulm -> userid == u -> userid )
                    ulm -> u = u;

               ulm = ulm -> next;
          }
          
          mover = mover -> next;
     }
}

void PLUGIN_USER_LOGOUT( user_t *u )
{
     group_t *mover;
     ulist_t *ulm;          

     mover = public_head;

     while( mover )
     {           
          ulm = mover -> users;

          while( ulm )
          {     
               if( ulm -> userid == u -> userid )
                    ulm -> u = 0;

               ulm = ulm -> next;
          }
          
          mover = mover -> next;
     }
}
 
void PLUGIN_USER_RECONNECT( user_t*old, user_t *new )
{
     group_t *mover;
     ulist_t *ulm;          

     mover = public_head;

     while( mover )
     {           
          ulm = mover -> users;

          while( ulm )
          {     
               if( ulm -> userid == new -> userid )
                    ulm -> u = new;

               ulm = ulm -> next;
          }
          
          mover = mover -> next;
     }
}

void plugin_init( plugin_s *p_handle )
{    
     groups_h = p_handle;

     /* add default group */

     public_head = group_alloc( "General", 0 );
     if( !public_head )
          return;

     public_head -> desc = strdup( "General Banal Conversation" );
     public_head -> letter = 'c';

     /* load groups */

     /* TO DO */

     /* group manipulation commands */

     add_command( "group_list",   gr_list,   p_handle, CMD_GROUP, 0 );
     add_command( "grl",          gr_list,   p_handle, CMD_INVIS, 0 );

     add_command( "group_create", gr_create, p_handle, CMD_GROUP, 1 );
     add_command( "grc",          gr_create, p_handle, CMD_INVIS, 1 );

     add_subcommand( "group_create", "tag",    gr_create_tag,    p_handle, CMD_GROUP, 0 );
     add_subcommand( "group_create", "desc",   gr_create_desc,   p_handle, CMD_GROUP, 0 );
     add_subcommand( "group_create", "colour", gr_create_colour, p_handle, CMD_GROUP, 0 );
     add_subcommand( "group_create", "letter", gr_create_letter, p_handle, CMD_GROUP, 0 );
     add_subcommand( "group_create", "done",   gr_create_done,   p_handle, CMD_GROUP, 0 );
     add_subcommand( "group_create", "abort",  gr_create_abort,  p_handle, CMD_GROUP, 0 );

     add_subcommand( "grc",          "tag",    gr_create_tag,    p_handle, CMD_INVIS, 0 );
     add_subcommand( "grc",          "desc",   gr_create_desc,   p_handle, CMD_INVIS, 0 );
     add_subcommand( "grc",          "colour", gr_create_colour, p_handle, CMD_INVIS, 0 );
     add_subcommand( "grc",          "letter", gr_create_letter, p_handle, CMD_INVIS, 0 );
     add_subcommand( "grc",          "done",   gr_create_done,   p_handle, CMD_INVIS, 0 );
     add_subcommand( "grc",          "abort",  gr_create_abort,  p_handle, CMD_INVIS, 0 );

     add_command( "group_destroy", gr_destroy, p_handle, CMD_GROUP,    1 );
     add_command( "grd",           gr_destroy, p_handle, CMD_INVIS, 1 );

     add_command( "group_info",    gr_info, p_handle, CMD_GROUP,    0 );
     add_command( "gri",           gr_info, p_handle, CMD_INVIS, 0 );

     add_command( "group_join",    gr_join, p_handle, CMD_GROUP,    0 );
     add_command( "grj",           gr_join, p_handle, CMD_INVIS, 0 );

     add_command( "group_leave",   gr_leave, p_handle, CMD_GROUP,    0 );
     add_command( "grq",           gr_leave, p_handle, CMD_INVIS, 0 );

     /* commands for your default group */

     add_command( "say",       say,     p_handle, CMD_GROUP | CMD_PUBLIC | CMD_BASIC,     0 );
     add_command( "`",         say,     p_handle, CMD_INVIS,  0 );
     add_command( "\'",        say,     p_handle, CMD_INVIS,  0 );
     add_command( "\"",        say,     p_handle, CMD_INVIS,  0 );

     add_command( "emote",     emote,   p_handle, CMD_GROUP | CMD_PUBLIC | CMD_BASIC,     0 );
     add_command( ";",         emote,   p_handle, CMD_INVIS,  0 );
     add_command( ":",         emote,   p_handle, CMD_INVIS,  0 );
     add_command( "emtoe",     emote,   p_handle, CMD_INVIS,  0 );
     add_command( "emoet",     emote,   p_handle, CMD_INVIS,  0 );
     add_command( "emotes",    emote,   p_handle, CMD_INVIS,  0 );

     add_command( "sing",      sing,    p_handle, CMD_GROUP | CMD_PUBLIC,    0 );
     add_command( ")",         sing,    p_handle, CMD_INVIS, 0 );

     add_command( "think",     think,   p_handle, CMD_GROUP | CMD_PUBLIC,    0 );
     add_command( "~",         think,   p_handle, CMD_INVIS, 0 );

     add_command( "echo",      echo,    p_handle, CMD_GROUP | CMD_PUBLIC,    0 );
     add_command( "+",         echo,    p_handle, CMD_INVIS , 0 );
     add_command( "aecho",     echo,    p_handle, CMD_INVIS , 0 );
     add_command( "becho",     echo,    p_handle, CMD_INVIS , 0 );

     add_command( "dsay",      dsay,    p_handle, CMD_GROUP | CMD_PUBLIC,     1 );
     add_command( "[",         dsay,    p_handle, CMD_INVIS,     1 );

     add_command( "demote",    demote,  p_handle, CMD_GROUP | CMD_PUBLIC,     1 );
     add_command( "]",         demote,  p_handle, CMD_INVIS,     1 );

}           

void plugin_fini( plugin_t p_handle )
{
}
