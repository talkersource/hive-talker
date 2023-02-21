#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef LINUX
#include <linux/types.h>
#endif

#include "plugin_headers.h"
#include "elite.h"

PLUGIN_NAME( "Elite" )
PLUGIN_VERSION( "1.0" )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "galak" )

#define NUM_PLANETS  256

/* x value needs to be maximum plansys.x value + max name size + 2 */
/* ( to simplify rendering )   255 + 8 + 2 = 265 */
#define MAPSIZEX     265
/* y value only needs to be max (plansys.y / 2) + 1 = (255 / 2) + 1 = 128 */
#define MAPSIZEY     129

/* I am using 66k for this map, but it makes rendering
   alot easier and quicker
*/
int16 map[MAPSIZEX][MAPSIZEY];


char pairs[] = "..LEXEGEZACEBISO"
               "USESARMAINDIREA."
               "ERATENBERALAVETI"
               "EDORQUANTEISRION"; /* Dots should be nullprint characters */

char *govnames[] = {
     "Anarchy",
     "Feudal",
     "Multi-government",
     "Dictatorship",
     "Communist",
     "Confederacy",
     "Democracy",
     "Corporate State"
};

char *econnames[] = {
     "Rich Industrial",
     "Average Industdrial",
     "Poor Industrial",
     "Mainly Industrial",
     "Mainly Agricultural",
     "Rich Agricultural",
     "Average Agricultural",
     "Poor Agricultural"
};

char *planet[] = {
     "    ........    ",
     "  .....      .  ",
     "......         .",
     "......         .",
     "......         .",
     "......         .",
     "  .....      .  ",
     "    ........    "
};                    

const uint16 base0 = 0x5A4A;
const uint16 base1 = 0x0248;
const uint16 base2 = 0xB753;  /* Base seed for galaxy 1 */

plansys galaxy[256];
seedtype seed; 
           
void tweakseed( seedtype *s )
{
     uint16 tmp;

     tmp = s->w0 + s->w1 + s->w2;
     s->w0 = s->w1; 
     s->w1 = s->w2;
     s->w2 = tmp;
}

void stripout( char *s, const char c )
{
     char *ptr = s;
 
     while( *s )
     {
          if( *s != c )
               *ptr++ = *s;
          s++;
     }
     *ptr = '\0';
}

plansys makesystem( seedtype *s )
{
     plansys ps;
     uint   p[4], a;
     uint16 longnameflag = s->w0 & ( 1 << 6 );

     ps.x = s->w1 >> 8;
     ps.y = s->w0 >> 8;

     ps.govtype = ( s->w1 >> 3 ) & 7;
     ps.economy = ( s->w0 >> 8 ) & 7;

     if( ps.govtype < 2 )
          ps.economy |= 2;

     ps.techlev = ( ( s->w1 >> 8 ) & 3 ) + ( ps.economy ^ 7 );
     ps.techlev += ps.govtype >> 1;

     if( ( ps.govtype & 1 ) == 1 )
          ps.techlev += 1;

     ps.population = ( 4 * ps.techlev + ps.economy ) 
                     + ps.govtype + 1;

     ps.productivity = ( ( ps.economy ^ 7 ) + 3 )
                       * ( ps.govtype + 4 );
     ps.productivity *= ps.population * 8;

     ps.radius = 256 * ( ( ( s->w2 >> 8 ) & 15 ) + 11 ) + ps.x;

     for( a = 0; a < 4; a++ )
     {
          p[a] = 2 * ( ( s->w2 >> 8 ) & 31 );
          tweakseed( s );
     }
      
     ps.name[0] = pairs[ p[0]     ];
     ps.name[1] = pairs[ p[0] + 1 ];
     ps.name[2] = pairs[ p[1]     ];
     ps.name[3] = pairs[ p[1] + 1 ];
     ps.name[4] = pairs[ p[2]     ];
     ps.name[5] = pairs[ p[2] + 1 ];

     if( longnameflag )
     {
          ps.name[6] = pairs[ p[3]     ];
          ps.name[7] = pairs[ p[3] + 1 ];
          ps.name[8] = '\0';
     }
     else
          ps.name[6] = '\0';

     stripout( ps.name, '.' );

     return ps;
}

void buildgalaxy( )
{            
     int syscount;

     seed.w0 = base0;
     seed.w1 = base1;
     seed.w2 = base2;

     for( syscount = 0; syscount < NUM_PLANETS; ++syscount )
          galaxy[syscount] = makesystem( &seed );             
}

void buildmap( )
{
     int a, b;

     for( a = 0; a < MAPSIZEY; a++ )
          for( b = 0; b < MAPSIZEX; b++ )
               map[b][a] = -1;

     for( a = 0; a < NUM_PLANETS; a++ )
          map[galaxy[a].x][( int ) floor( galaxy[a].y / 2 )] = a;
}

plansys *find_planet( char *str )
{
     int a; 

     for( a = 0; a < NUM_PLANETS; a++ )
          if( !strcasecmp( galaxy[a].name, str ) )
               return &( galaxy[a] );

     command_output( "Unknown planet '\%s\'\n", str );
     return NULL;
}   

/* star chart */
void display_map( user_t *u, int argc, char *argv[] )
{
     int cx, cy, x, y, startx, starty;
     int a, remaining, len, m, dx, dy;
     plansys *current = &( galaxy[7] ), *p;
     string_t *buf;
     int ok;
     char *name;

     if( argc > 1 )
     {
          p = find_planet( argv[ 1 ] );

          if( p == NULL )
               return;
     }
     else
          p = current;

     dx = ( ( u->socket->x - 1 ) < MAPSIZEX ) ? u->socket->x - 1 : MAPSIZEX;
     dy = ( ( u->socket->y - 4 ) < MAPSIZEY ) ? u->socket->y - 4 : MAPSIZEY;

     cx = p->x;
     cy = ( int ) floor( p->y / 2 );

     startx = cx - ( dx / 2 );
     starty = cy - ( dy / 2 );

     if( startx < 0 )
          startx = 0;
     else if( startx > ( MAPSIZEX - dx ) )
          startx = MAPSIZEX - dx;

     if( starty < 0 )
          starty = 0;
     else if( starty > ( MAPSIZEY - dy ) )
          starty = MAPSIZEY - dy;

     buf = string_alloc( "" );
     string_append_title( u, buf, LEFT, "Star Chart local to %s", p->name );

     remaining = 0;
     for( y = starty; y < starty + dy ; y++ )
     {
          for( x = startx; x < startx + dx; x++ )
          {
               if( !remaining )
               {
                    m = map[x][y];
                    if( m != -1 && m < 10000 )
                    {
                         name = galaxy[m].name;
                         len = strlen( name );

                         ok = 1;

                         if( x + len + 1 >= startx + dx )
                              ok = 0;
                         else
                              // look right
                              for( a = 0; a < len + 2; a++ )
                                   if( map[x+a+1][y] != -1 ) ok = 0;

                         if( ok )
                         {
                              string_append( buf, "^GO^b ^p%s", name );
                              remaining = len + 1;
                         }
                         else
                         {
                              ok = 1;

                              if( x + len + 2 >= startx + dx )
                                   ok = 0;
                              else
                                   // look right and down a line
                                   for( a = 0; a < len + 3; a++ )
                                        if( map[x+a][y+1] != -1 ) ok = 0;

                              if( ok )
                                   // put a marker on the line below
                                   map[x][y+1] = m + 10000;
                                  // otherwise give up trying to display the name

                              string_append( buf, "^GO" );
                         }
                    }
                    else if( m != -1 )
                    {
                         // fill in a name marker left previously
                         name = galaxy[m - 10000].name;
                         len = strlen( name );

                         string_append( buf, " ^p`%s", name );
                         remaining = len + 1;
                    }
                    else
                    {
                         a = ( int ) floor ( ( 4 * sqrt( ( current->x - x ) * ( current->x - x ) + ( current->y - ( y * 2 ) ) * ( current->y - ( y * 2 ) ) / 4 ) ) + 0.5 );
                         if( a > 67 && a < 73 )
                              string_append( buf, "^c-" );
                         else
                              string_append( buf, " " );
                    }
               }
               else
                    remaining--;
          }
          string_append( buf, "\n" );
     }

     string_append_line( u, buf );

     command_output( buf -> data );

     string_free( buf );
}

/* get data on a planet */
void data( user_t *u, int argc, char *argv[] )
{
     plansys *p;
     string_t *buf;

     if( argc < 2 )
     {
          command_output( "Format : %s <planet name>\n", argv[ 0 ] );
          return;
     }

     p = find_planet( argv[ 1 ] );

     if( p != NULL )
     {
          buf = string_alloc( "" );

/*

                --- LAVE ---                               ........
                                                         .....      .
          Economy : Rich Agricultural                  ......         .
       Government : Dictatorship                       ......         .
       Tech Level : 5                                  ......         .
     Productivity : 7000 M Cr                          ......         .
       Population : 2.5 Billion                          .....      .
                                                           ........   
      This planet is a tedious place                        
                                                       |<- 4116 km -->|

*/                               
          string_append( buf,
                   "             --- %s ---^G%*s^N\n"
                   "^G%64s^N\n"
                   "        Economy : %-30s^G%s^N\n"
                   "     Government : %-30s^G%s^N\n"
                   "     Tech Level : %-30d^G%s^N\n"
                   "   Productivity : %d M Cr^G%*s^N\n"
                   "     Population : %.1f Billion^G%*s^N\n"
                   "^G%64s^N\n"
                   "   %s\n"
                   "   %-45s|<- %d km -->|\n",

                   p->name,  43 - strlen( p->name ), planet[0],
                   planet[1], 
                   econnames[p->economy], planet[2],
                   govnames[p->govtype], planet[3],
                   p->techlev + 1, planet[4],
                   p->productivity, ( p->productivity > 9999 ) ? 36 : 37, planet[5],
                   ( ( double ) p->population ) / 10, 35, planet[6],
                   planet[7],
                   "This planet is a tedious place", 
                   "", p->radius );

          command_output( buf->data );

          string_free( buf );
     }
}

void plugin_init( plugin_s *p_handle )
{   
     srand( 12345 );    /* Ensure repeatability */

     buildgalaxy( );

     buildmap( );

     add_command( "data",    data,           p_handle, CMD_GAMES, 0 );
     add_command( "map",     display_map,    p_handle, CMD_GAMES, 0 );
}
