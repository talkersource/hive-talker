#ifndef __PLUGIN_H
        #define __PLUGIN_H

#include <time.h>

#ifdef HPUX
#define HAVE_SHL
#endif

#if defined ( LINUX ) || defined ( SOLARIS ) || defined ( DIGITAL )
#define HAVE_DL
#endif

#ifdef HAVE_SHL
#include <dl.h>

typedef shl_t plugin_t;
#define PLUGIN_OPEN( x ) shl_load( x, BIND_DEFERRED , 0L )
#define PLUGIN_SYM( x, y, z ) !( shl_findsym( &x, y, TYPE_PROCEDURE, &( z ) ) )
#define PLUGIN_CLOSE( x ) shl_unload( x )
#define PLUGIN_ERROR NULL

#elif defined ( HAVE_DL )
#include <dlfcn.h>

typedef void * plugin_t;
#define PLUGIN_OPEN( x ) dlopen( x, RTLD_LAZY | RTLD_GLOBAL )
#define PLUGIN_SYM( x, y, z ) ( z = dlsym( x, y ) )
#define PLUGIN_CLOSE( x ) dlclose( x )
#define PLUGIN_ERROR dlerror( )

#else
#error "Can't find dynamic library support on this platform!"
#endif

#define PLUGIN_NAME( x ) char *_name( ) { return x; }
#define PLUGIN_VERSION( x ) char *_version( ) { return x; }
#define PLUGIN_DEPENDENCIES( x ) char *_dependencies( ) { return x; }
#define PLUGIN_AUTHORS_LIST( x ) char *_authors_list( ) { return x; }
#define PLUGIN_FILE_EXTENSION( x ) char *_file_extension( ) { return x; }

struct plugin
{
        plugin_t handle;      /* shared library handle            */
        char *name;           /* library name                     */
        char *version;        /* plugin version                   */
        char *path;           /* plugin filename                  */
        char **dependencies;  /* zero terminated dependency list  */
        char **authors_list;  /* zero terminated author list      */

        time_t loaded;        /* the time the plugin was loaded   */
        time_t compiled;      /* the time the plugin was compiled */

        struct plugin *next;
};
typedef struct plugin plugin_s;

extern plugin_s *plugin_head;

extern char *plugin_get_name( plugin_t );
extern plugin_s *plugin_find( char * );
extern int  plugin_register( char * );
extern int  plugin_remove( plugin_s * );
extern void plugin_remove_all( void );                          
extern int  plugin_reload( plugin_s * );

#endif






