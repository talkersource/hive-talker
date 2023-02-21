/* 
 *    ___    ___              ____  ___  _____
 *   /   \  /   \ |\  | |\  | |    /   \   |
 *   |      |   | | \ | | \ | |__  |       |
 *   |      |   | |  \| |  \| |    |       |
 *   \___/  \___/ |   | |   | |___ \___/   |
 * 
 * ______________|_______|_______|______________
 *          ___  |  ___  |  ___  |  ___
 *         /###\ | /###\ | /###\ | /###\
 *         |#F#| | |#O#| | |#U#| | |#R#|
 *         \###/ | \###/ | \###/ | \###/
 * ______________|_______|_______|______________
 * 
 */

#include "plugin_headers.h"

PLUGIN_NAME( "Connect 4 Game" )
PLUGIN_VERSION( "0.9"  )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "Zub" )

typedef struct player_s
{
     int userid;
     user_t *u;

     int flags;
} player_t;

typedef struct game_s
{
     player_t *p1;
     player_t *p2;

     player_t *turn;

     int grid[ 6 ][ 7 ];

     struct game_s *next;
} game_t;

/* player flags */
#define ACCEPTED      ( 1 << 0 )

static char counter_colour[ 2 ] = "RY";

/* grid bits */

#define EMPTY         ( 1 << 0 )
#define COUNTER1      ( 1 << 1 )
#define COUNTER2      ( 1 << 2 )
#define WINNER        ( 1 << 3 )

game_t *c4_games;

game_t *c4_alloc_game( user_t *u1, user_t *u2 )
{
     game_t *new;

     new = ALLOC( 1, game_t );
     if( !new )
          return NULL;

     new -> p1 = ALLOC( 1, player_t );
     if( !new -> p1 )
     {
          FREE( new );
          return NULL;
     }                

     new -> p2 = ALLOC( 1, player_t );
     if( !new -> p2 )
     {             
          FREE( new -> p1 );
          FREE( new );
          return NULL;
     }

     new -> p1 -> u = u1;
     new -> p1 -> userid = u1 -> userid;
     new -> p2 -> u = u2;     
     new -> p2 -> userid = u2 -> userid;

     return new;
}

void c4_free_game( game_t *game )
{
     if( game == NULL )
          return;

     if( game -> p1 )
          FREE( game -> p1 );

     if( game -> p2 )
          FREE( game -> p2 );

     FREE( game );
}

game_t *c4_add_game( user_t *u1, user_t *u2 )
{
     game_t *new, *mover;

     new = c4_alloc_game( u1, u2 );
     if( new == NULL )
          return NULL;

     /* add the new game to the end of the linked list */
     if( c4_games == NULL )
          c4_games = new;
     else
     {
          mover = c4_games;

          while( mover -> next )
               mover = mover -> next;

          mover -> next = new;
     }
     return new;
}

void c4_remove_game( game_t *game )
{
     game_t *prev, *mover;

     prev = NULL;
     mover = c4_games;

     while( mover && mover != game )
     {
          prev = mover;
          mover = mover -> next;
     }

     if( mover == NULL )
          return;

     if( prev )
          prev -> next = mover -> next;
     else
          c4_games = mover -> next;

     c4_free_game( mover );
}

game_t *c4_find_game_by_user( user_t *u )
{
     game_t *mover;

     mover = c4_games;

     while( mover )
     {                          
          if( mover -> p1 -> u == u || mover -> p2 -> u == u )
               return mover;

          mover = mover -> next;
     }

     return NULL;
}

game_t *c4_find_game_by_userid( int userid )
{
     game_t *mover;

     mover = c4_games;

     while( mover )
     {
          if( mover -> p1 -> userid == userid || mover -> p2 -> userid == userid )
               return mover;

          mover = mover -> next;
     }

     return NULL;
}

player_t *c4_get_player( user_t *u, game_t *game )
{
     if( game -> p1 -> u == u )
          return game -> p1;
     else if( game -> p2 -> u == u )
          return game -> p2;

     return NULL;
}

void c4_render_line( string_t **buf_ptr, int *line, int reverse )
{
     char c, col;
     signed i;

     if( reverse )
          i = 6;
     else
          i = 0;
     while( i >= 0 && i < 7 )
     {
          if( *( line + i ) & COUNTER1 )
          {
               c = '@';
               col = counter_colour[0];
          }
          else if( *( line + i ) & COUNTER2 )
          {
               c = '#';
               col = counter_colour[1];
          }
          else if( ( *( line + i ) & EMPTY ) || ! *( line + i) )
          {
               c = 'O';
               col = 'b';
          }
          else
          {
               c = '?';
               col = 'w';
          }

          if( *( line + i ) & WINNER )
               c = 'X';  

          string_append( *buf_ptr, " ^%c%c", col, c);

          if( reverse )
               i--;
          else
               i++;
     }
}

void c4_render_board( game_t *game, user_t *u, int reverse )
{
     string_t *buf;
     int i;

     if( reverse )
          buf = string_alloc( "\n   ^cG F E D C B A\n" );
     else
          buf = string_alloc( "\n   ^cA B C D E F G\n" );

     for( i = 0; i < 6; i++ )
     {        
          string_append( buf, " ^c%d", 6 - i );
          c4_render_line( &buf, &( game -> grid[ i ][ 0 ] ), reverse );
          string_append( buf, "^n\n" );
     }

     user_output( CGAMES, NULL, u, "%s\n", buf -> data );

     string_free( buf );
}

/* sets x to column number */
/* returns 0 on success, 1 on failure */
int c4_evaluate_move( char *str, int *x )
{
     if( !*str )
          return 1;

     if ( *str >= 'a' && *str <= 'g' )
          *x = (int )( *str - 'a' );
     else if ( *str >= 'A' && *str <= 'H' )
          *x = (int )( *str - 'A' );
     else
	  return 1;

     return 0;
}

int c4_gamedrawn( game_t *game )
{
     int x;
     for ( x = 0; x < 7; x++ )
          if( !( game -> grid[ 0 ][ x ] ) )
               return 0;
     return 1;
}

int c4_testplace( game_t *game, signed x, signed y )
{
     int flag;
     if( !( ( x >= 0 ) && ( x < 7 ) && ( y >= 0 ) && ( y < 6 ) ) )
          return 0;

     if( game -> turn == game -> p1 )
          flag = COUNTER1;
     else
          flag = COUNTER2;
     if( game -> grid[ y ][ x ] & flag )
          return 1;

     return 0;
}

int c4_dotestline( game_t *game, int x, int y, signed xinc, signed yinc, int sofar )
{
     int retval;
     if( !( c4_testplace( game, x, y ) ) )
          return sofar;

     sofar++;

     if( ( retval = c4_dotestline( game, x + xinc, y + yinc, xinc, yinc, sofar ) ) >= 4 )
          game -> grid[ y ][ x ] |= WINNER;
     return retval;
}

int c4_testline( game_t *game, int x, int y, signed xinc, signed yinc )
{
     do
     {
          x -= xinc;
          y -= yinc;
     }
     while( ( x >= 0 ) && ( x < 7) && ( y >= 0 ) && ( y < 6 ) && ( c4_testplace( game, x, y ) ) );

     return( c4_dotestline( game, x+xinc, y+yinc, xinc, yinc, 0 ) );
}

int c4_gamewon( game_t *game, int x, int y )
{
     int won = 0;
     // we must call each testline so that the winning counters are marked:
     if( c4_testline( game, x, y, 0, -1 ) >= 4 )
          won = 1;
     if( c4_testline( game, x, y, -1, 0 ) >= 4 )
          won = 1;
     if( c4_testline( game, x, y, 1, -1 ) >= 4 )
          won = 1;
     if( c4_testline( game, x, y, -1, -1 ) >= 4 )
          won = 1;
     return won;
}

void c4( user_t *u, int argc, char *argv[] )
{
     game_t *game;
     user_t *opponent;
     player_t *p, *opp;
     int x, y;

     game = c4_find_game_by_user( u );

     if( !game )
     {
          if( argc < 2 )
          {
               command_output( "Format: %s <opponent>\n", argv[ 0 ] );
               return;
          }

          opponent = user_find_on_by_name( u, argv[ 1 ]);

          if( !opponent )
               return;

          game = c4_find_game_by_user( opponent );
          if( game )
          {
               command_output( "Sorry, that user is already playing a game of connect 4\n" );
               return;
          }

          game = c4_add_game( u, opponent );
          if( !game )
          {
               command_output( "Sorry, an error occured, you may not play connect 4 at this time\n" );
               return;
          }

          user_output( CGAMES, u, opponent, "^W%s has challenged you to a game of connect 4\n^WType 'cfour accept' or 'cfour decline'^n\n", u -> current_name );
          command_output( "You challenge %s to a game of connect 4...\n", opponent -> current_name );
     }
     else
     {
          p   = c4_get_player( u, game );
          opp = ( p == game -> p1 ) ? game -> p2 : game -> p1;

          if( !( game -> p2 -> flags & ACCEPTED ) )
          {
               if( p == game -> p2 )
                    command_output( "Waiting for you to accept the challenge\n" );
               else
		    command_output( "Waiting for your opponent to accept the challenge\n" );
               return;
          }

          if( opp -> u == NULL )
          {
               c4_render_board( game, p -> u, game -> p1 -> u == opp -> u );
               command_output( "Your opponent is logged out at the moment\n" );
               return;
          }

	  if( p != game -> turn )
          {
               c4_render_board( game, p -> u, game -> p1 -> u == opp -> u );
               command_output( "It is %s's turn\n", opp -> u -> current_name );
               return;
          }

          if( c4_evaluate_move( argv[ 1 ], &x ) )
          {
               c4_render_board( game, p -> u, game -> p1 -> u == opp -> u );
               command_output( "You must specify a column from A to G!\n" );
               return;
          }

          for( y = 0; y < 6; y++ )
               if( ( game -> grid[ y ][ x ] ) )
                    break;

          if( y <= 0 )
          {
               c4_render_board( game, p -> u, game -> p1 -> u == opp -> u );
               command_output( "That column is full!\n" );
          }
          else
          {
               y--;

               if( game -> turn == game -> p1 )
                    game -> grid[ y ][ x ] |= COUNTER1;
               else
                    game -> grid[ y ][ x ] |= COUNTER2;

               if( c4_gamedrawn( game ) )
               {
                    c4_render_board( game, p -> u, game -> p1 -> u == opp -> u );
                    c4_render_board( game, opp -> u, game -> p1 -> u == p -> u );
                    group_output( CGAMES, universe -> connected, "^g->^n %s just drew with %s at Connect 4!\n", u -> current_name, opp -> u -> current_name );
               }
               else if ( c4_gamewon( game, x, y ) )
               {
                    c4_render_board( game, p -> u, game -> p1 -> u == opp -> u );
                    c4_render_board( game, opp -> u, game -> p1 -> u == p -> u );
                    group_output( CGAMES, universe -> connected, "^g->^n %s just outwitted %s at Connect 4!\n", u -> current_name, opp -> u -> current_name );
                    c4_remove_game( game );
               }
	       else
               {
                    c4_render_board( game, p -> u, game -> p1 -> u == opp -> u );
                    c4_render_board( game, opp -> u, game -> p1 -> u == p -> u );
                    user_output( CGAMES, u, opp -> u, "%s places their counter in %c. It is now your turn.\n", u -> current_name, ( char ) x + 'A' );
                    command_output( "It is now %s's turn.\n", opp -> u -> current_name );
                    game -> turn = opp;
               }
          }
     }
}

void c4_who( user_t *u, int argc, char *argv[] )
{
     game_t *mover;
     int i;
     string_t *buf;

     mover = c4_games;

     if( mover == NULL )
     {           
          command_output( "There are no Connect 4 games in progress\n" );
          return;
     }

     buf = string_alloc( "" );

     string_append_title( u, buf, LEFT, "Connect 4 Games in Progress" );
           
     i = 1;

     while( mover )
     {
          if( mover -> p1 -> u == NULL )
               string_append( buf, "%2d) %s's opponent is not connected\n", i, mover -> p2 -> u -> current_name );
          else if( mover -> p2 -> u == NULL )
               string_append( buf, "%2d) %s's opponent is not connected\n", i, mover -> p1 -> u -> current_name );
          else if( !( mover -> p2 -> flags & ACCEPTED ) )
               string_append( buf, "%2d) %s has challenged %s\n", i, mover -> p1 -> u -> current_name, mover -> p2 -> u -> current_name );
          else
               string_append( buf, "%2d) %s is playing %s\n", i, mover -> p1 -> u -> current_name, mover -> p2 -> u -> current_name );

          mover = mover -> next;
          i++;
     }

     string_append_line( u, buf );
     command_output( buf -> data );

     string_free( buf );
}

void c4_view( user_t *u, int argc, char *argv[] )
{
     game_t *mover;
     int r;

     if( argc < 2 )
     {    
          command_output( "Format: cfour %s <game number>\n", argv[0] );
          return;
     }

     r = atoi( argv[ 1 ] ) - 1;

     mover = c4_games;

     while( mover && r )
     {
          mover = mover -> next;
          r--;
     }        

     if( mover == NULL )
     {
          command_output( "Unknown game \'%s\'. Type \'cfour who\' for a list of current games\n", argv[ 1 ] );
          return;
     }

     if( mover -> p1 -> u == NULL || mover -> p2 -> u == NULL )
     {
          command_output( "That game is currently suspended\n" );
          return;
     }           

     if( !( mover -> p2 -> flags & ACCEPTED ) )
     {
          command_output( "That game has not yet begun\n" );
          return;
     }

     c4_render_board( mover, u, u == mover -> p2 -> u );
     
     if( u != mover -> p1 -> u && u != mover -> p2 -> u )
     {
          user_output( CGAMES, u, mover -> p1 -> u, "^G->^n %s just viewed your game\n", u -> current_name );
          user_output( CGAMES, u, mover -> p2 -> u, "^G->^n %s just viewed your game\n", u -> current_name );
     }
}

void c4_accept( user_t *u, int argc, char *argv[ ] )
{
     game_t *game;
     player_t *notturn;

     game = c4_find_game_by_user( u );

     if( !game )
     {
          command_output( "No one has challenged you to a game of connect 4!\n" );
          return;
     }           
                 
     if( game -> p2 -> u != u )
     {
          command_output( "You are the challenger!\n" );
          return;
     }

     if( game -> p2 -> flags & ACCEPTED )
     {
          command_output( "The game has already been accepted\n" );
          return;
     }           

     game -> p2 -> flags |= ACCEPTED;

     game -> turn = ( rand( ) % 2 ) ? game -> p1 : game -> p2;
     notturn = ( game -> turn == game -> p1 ) ? game -> p2 : game -> p1;

     c4_render_board( game, game -> p1 -> u, 0 );
     c4_render_board( game, game -> p2 -> u, 1 );

     user_output( CGAMES, NULL, game -> turn -> u, "You get to go first\n" );
     user_output( CGAMES, NULL, notturn -> u, "%s gets to go first\n", game -> turn -> u -> current_name );
}

void c4_decline( user_t *u, int argc, char *argv[ ] )
{
     game_t *game;                            

     game = c4_find_game_by_user( u );

     if( !game )
     {
          command_output( "No one has challenged you to a game of connect 4!\n" );
          return;
     }           

     if( game -> p2 -> u != u )
     {
          command_output( "You are the challenger! (use cfour quit instead)\n" );
          return;
     }

     if( game -> p2 -> flags & ACCEPTED )
     {
          command_output( "The game has already been accepted (use cfour quit instead)\n" );
          return;
     }           

     user_output( CGAMES, u, game -> p1 -> u, "%s declines your connect 4 offer\n", u -> current_name );
     command_output( "You decline the connect 4 offer from %s\n", game -> p1 -> u -> current_name );

     c4_remove_game( game );
}

void c4_quit( user_t *u, int argc, char *argv[ ] )
{
     game_t *game;
     player_t *p;
     player_t *opp;

     game = c4_find_game_by_user( u );

     if( !game )
     {
          command_output( "You are not playing connect 4!\n" );
          return;
     }           

     p = c4_get_player( u, game );
     opp = ( p == game -> p1 ) ? game -> p2 : game -> p1;

     if( opp -> u )
          user_output( CGAMES, u, opp -> u, "%s has just quit your game of connect 4!\n", u -> current_name );

     command_output( "You quit your game of connect 4\n" );

     c4_remove_game( game );
}

void plugin_init( plugin_s *p_handle )
{
     c4_games = NULL; 

     add_command(    "cfour",            c4,         p_handle, CMD_GAMES,    1 );
     add_subcommand( "cfour", "accept",  c4_accept,  p_handle, CMD_GAMES,    0 );
     add_subcommand( "cfour", "decline", c4_decline, p_handle, CMD_GAMES,    0 );
     add_subcommand( "cfour", "quit",    c4_quit,    p_handle, CMD_GAMES,    0 );
     add_subcommand( "cfour", "who",     c4_who,     p_handle, CMD_GAMES,    0 );
     add_subcommand( "cfour", "view",    c4_view,    p_handle, CMD_GAMES,    0 );

     add_command(    "c4",               c4,         p_handle, CMD_INVIS,    1 );
     add_subcommand( "c4",    "accept",  c4_accept,  p_handle, CMD_GAMES,    0 );
     add_subcommand( "c4",    "decline", c4_decline, p_handle, CMD_GAMES,    0 );
     add_subcommand( "c4",    "quit",    c4_quit,    p_handle, CMD_GAMES,    0 );
     add_subcommand( "c4",    "who",     c4_who,     p_handle, CMD_GAMES,    0 );
     add_subcommand( "c4",    "view",    c4_view,    p_handle, CMD_GAMES,    0 );

     add_command(    "cf",               c4,         p_handle, CMD_INVIS,    1 );
     add_subcommand( "cf",    "accept",  c4_accept,  p_handle, CMD_GAMES,    0 );
     add_subcommand( "cf",    "decline", c4_decline, p_handle, CMD_GAMES,    0 );
     add_subcommand( "cf",    "quit",    c4_quit,    p_handle, CMD_GAMES,    0 );
     add_subcommand( "cf",    "who",     c4_who,     p_handle, CMD_GAMES,    0 );
     add_subcommand( "cf",    "view",    c4_view,    p_handle, CMD_GAMES,    0 );
}

void PLUGIN_USER_LOGIN( user_t *u )
{                       
     game_t *mover;
     player_t *p;
                                
     mover = c4_games;

     while( mover )
     {             
          if( mover -> p1 -> userid == u -> userid )
          {
               mover -> p1 -> u = u;
               user_output( CGAMES, NULL, u, "Your game of connect 4 against %s has resumed\n", mover -> p2 -> u -> current_name );

               user_output( CGAMES, NULL, mover -> p2 -> u, "Your connect 4 game against %s has resumed\n", u -> current_name );

               if( mover -> turn != NULL )
               {
                    p = ( mover -> turn == mover -> p1 ) ? mover -> p2 : mover -> p1;

                    user_output( CGAMES, NULL, mover -> turn -> u, "It is your turn\n" );
                    user_output( CGAMES, NULL, p -> u, "It is %s's turn\n", mover -> turn -> u -> current_name );
               }
          }
          else if( mover -> p2 -> userid == u -> userid )
          {
               mover -> p2 -> u = u;
               user_output( CGAMES, NULL, u, "Your game of connect 4 against %s has resumed\n", mover -> p1 -> u -> current_name );

               user_output( CGAMES, NULL, mover -> p1 -> u, "Your connect 4 game against %s has resumed\n", u -> current_name );
               
               if( mover -> turn != NULL )
               {  
                    p = ( mover -> turn == mover -> p1 ) ? mover -> p2 : mover -> p1;

                    user_output( CGAMES, NULL, mover -> turn -> u, "It is your turn\n" );
                    user_output( CGAMES, NULL, p -> u, "It is %s's turn\n", mover -> turn -> u -> current_name );
               }
          }

          mover = mover -> next;
     }
}
                
void PLUGIN_USER_LOGOUT( user_t *u )
{
     game_t *mover, *remove = NULL;
                              
     mover = c4_games;

     while( mover )
     {             
          if( mover -> p1 -> u == u )
          {                
               if( !( mover -> p2 -> flags & ACCEPTED ) )
               {
                    user_output( CGAMES, NULL, mover -> p2 -> u, "%s withdraws their connect 4 game offer\n", u -> current_name );
                    remove = mover;
               }
               else if( mover -> p2 -> u != NULL )
               {
                    mover -> p1 -> u = NULL;
                    user_output( CGAMES, NULL, mover -> p2 -> u, "Your connect 4 game is suspended\n" );
               }                            
               else
                    remove = mover;
          }
          else if( mover -> p2 -> u == u )
          {                
               if( !( mover -> p2 -> flags & ACCEPTED ) )
               {
                    user_output( CGAMES, NULL, mover -> p1 -> u, "%s declines your connect 4 offer\n", u -> current_name );
                    remove = mover;
               }
               else if( mover -> p1 -> u != NULL )
               {
                    mover -> p2 -> u = NULL;
                    user_output( CGAMES, NULL, mover -> p1 -> u, "Your connect 4 game is suspended\n" );
               }
               else
                    remove = mover;
          }

          mover = mover -> next;
     }

     if( remove )
          c4_remove_game( remove );          
}

void PLUGIN_USER_RECONNECT( user_t*old, user_t *new )
{
     game_t *mover;

     mover = c4_games;

     while( mover )
     {             
          if( mover -> p1 -> u == old )
               mover -> p1 -> u = new;
          else if( mover -> p2 -> u == old )
               mover -> p2 -> u = new;

          mover = mover -> next;
     }
}

