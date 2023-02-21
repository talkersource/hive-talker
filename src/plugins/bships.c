/*                                               
 *        B a t t l e S h i p s
 *                                   
 *           _=| /)    _=| /)          
 *             |/ )      |/ )          
 *             /  )      /  )          
 *    \____\  /   )     /   )          
 *    /    /\/   /     /   /           
 *   /    / /)  /______)  /______.     
 * \/___\/ / ) /|======) /|====== ~`-,;
 *  \  _ \/_/)/||_     )/||_      _,~' 
 *   \ \\ _ \===================-~ .'  
 *    \   \\   \o\  \o\  \o\      /    
 * ~`-.\.-'~`-._.-'~`-._.-'~`-._.''~`-._.-'~`-._.-'~`-._.-
 */                                   

#include "plugin_headers.h"                       

PLUGIN_NAME( "Battleships Game" )
PLUGIN_VERSION( "1.8"  )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "galak" )

typedef struct player_s
{
     int userid;
     user_t *u;

     int flags;
     int score;

     int grid[ 8 ][ 8 ];
} player_t;
  
typedef struct game_s
{
     player_t *p1;
     player_t *p2;

     player_t *turn;
     
     struct game_s *next;
} game_t;

/* player flags */
#define ACCEPTED      ( 1 << 0 )
#define READY         ( 1 << 1 )

static char ship_colour[ 6 ] = "YYYPPG";

/* grid bits */

#define EMPTY         ( 1 << 0 )
#define SHIP2_1       ( 1 << 1 )
#define SHIP2_2       ( 1 << 2 )
#define SHIP2_3       ( 1 << 3 )
#define SHIP3_1       ( 1 << 4 )
#define SHIP3_2       ( 1 << 5 )
#define SHIP4_1       ( 1 << 6 )
#define BOMBED        ( 1 << 7 )
           
game_t *games;

game_t *alloc_game( user_t *u1, user_t *u2 )
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

void free_game( game_t *game )
{                 
     if( game == NULL )
          return;

     if( game -> p1 )
          FREE( game -> p1 );

     if( game -> p2 )
          FREE( game -> p2 );

     FREE( game );
}                

game_t *add_game( user_t *u1, user_t *u2 )
{
     game_t *new, *mover;

     new = alloc_game( u1, u2 );
     if( new == NULL )
          return NULL;

     /* add the new game to the end of the linked list */
     if( games == NULL )
          games = new;
     else
     {
          mover = games;

          while( mover -> next )
               mover = mover -> next;

          mover -> next = new;
     }
     return new;
}

void remove_game( game_t *game )
{
     game_t *prev, *mover;

     prev = NULL;
     mover = games;

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
          games = mover -> next;

     free_game( mover );
}

game_t *find_game_by_user( user_t *u )
{
     game_t *mover;

     mover = games;

     while( mover )
     {                          
          if( mover -> p1 -> u == u || mover -> p2 -> u == u )
               return mover;

          mover = mover -> next;
     }

     return NULL;
}

game_t *find_game_by_userid( int userid )
{
     game_t *mover;

     mover = games;

     while( mover )
     {                          
          if( mover -> p1 -> userid == userid || mover -> p2 -> userid == userid )
               return mover;

          mover = mover -> next;
     }

     return NULL;
}

player_t *get_player( user_t *u, game_t *game )
{
     if( game -> p1 -> u == u )
          return game -> p1;
     else if( game -> p2 -> u == u )
          return game -> p2;

     return NULL;
}
     
void randomise_board( player_t *p )
{
     int a, b, ship, done, dir, x, y, length;

     ship = SHIP4_1;
                              
     /* reset the board */
     for( a = 0; a < 8; a++ )
          for( b = 0; b < 8; b++ )
               p -> grid[ a ][ b ] = EMPTY;
                          
     /* select positions */
     for( a = 0; a < 6; a++ )
     {
          done = 0;

          switch( ship )
          {
          case SHIP4_1 : length = 4; break;
          case SHIP3_1 : 
          case SHIP3_2 : length = 3; break;
          case SHIP2_1 :
          case SHIP2_2 :
          case SHIP2_3 : length = 2; break;
          }

          while( !done )
          {
               dir = rand( ) % 2;

               x = ( !dir ) ? ( ( rand( ) % ( 9 - length) ) ) : ( ( rand( ) % 8 ) );
               y = ( !dir ) ? ( ( rand( ) % 8 ) )             : ( ( rand( ) % ( 9 - length) ) );

               done = 1;

               if (!dir)
               {
                    for( b = 0; b < length; b++)
                         if ( p -> grid[ x + b ][ y ] != EMPTY ) 
                              done = 0;
               }
               else
               {
                    for( b = 0; b < length; b++)
                         if ( p -> grid[ x ][ y + b ] != EMPTY )
                              done = 0;
               }

          }

          if ( !dir )
          {
               for ( b = 0; b < length; b++)
                  p -> grid[ x + b ][ y ] = ship;
          }
          else 
          {
               for ( b = 0; b < length; b++)
                  p -> grid[ x ][ y + b ] = ship;
          }         

          ship >>= 1;
     }

}

void render_line( string_t **buf_ptr, int *line, int visible )
{
     int index;
     char c, col;
     int i, j;

               
     for( i = 0; i < 8; i++ )
     {
          index = -1;
          for( j = 0; j < 6; j++ )
               if( *( line + i ) & ( 1 << ( j + 1 ) ) )
                    index = j;

          if( index >= 0 )
          {
               if( *( line + i ) & BOMBED )
               {
                    c = 'X';
                    col = 'R';
               }
               else
               {  
                    c   = ( visible ) ? '0' : '?';
                    col = ( visible ) ? ship_colour[ index ] : 'b';
               }
          }                    
          else
          {
               if( *( line + i ) & BOMBED )
               {
                    c = '=';
                    col = 'W';
               }
               else
               {
                    c = ( visible ) ? '-' : '?';
                    col = 'b';
               }
          }

          string_append( *buf_ptr, " ^%c%c", col, c );
     }
}

void render_preboard( player_t *p )
{
     string_t *buf;
     int i;

     buf = string_alloc( "\n   ^cA B C D E F G H\n" );

     for( i = 0; i < 8; i++ )
     {
          string_append( buf, " ^c%d", i + 1 );
          render_line( &buf, &( p -> grid[ i ][ 0 ] ), 1 );
          string_append( buf, "^n\n" );
     }

     user_output( CGAMES, NULL, p -> u, buf -> data );

     string_free( buf );
}

void render_board( player_t *p, player_t *opp )
{
     string_t *buf;
     int i;

     buf = string_alloc( "\n   ^cA B C D E F G H                      A B C D E F G H\n" );

     for( i = 0; i < 8; i++ )
     {        
          string_append( buf, " ^c%d", i + 1 );
          render_line( &buf, &( opp -> grid[ i ][ 0 ] ), 0 );
          string_append( buf, "                   " );
          string_append( buf, " ^c%d", i + 1 );
          render_line( &buf, &( p -> grid[ i ][ 0 ] ), 1 );
          string_append( buf, "^n\n" );
     }                   

     user_output( CGAMES, NULL, p -> u, "%s\n", buf -> data );

     string_free( buf );

}

void render_postboard( player_t *p, player_t *opp )
{
     string_t *buf;
     int i;

     buf = string_alloc( "\n   ^cA B C D E F G H                      A B C D E F G H\n" );

     for( i = 0; i < 8; i++ )
     {        
          string_append( buf, " ^c%d", i + 1 );
          render_line( &buf, &( opp -> grid[ i ][ 0 ] ), 1 );
          string_append( buf, "                   " );
          string_append( buf, " ^c%d", i + 1 );
          render_line( &buf, &( p -> grid[ i ][ 0 ] ), 1 );
          string_append( buf, "^n\n" );
     }                   

     user_output( CGAMES, NULL, p -> u, "%s\n", buf -> data );

     string_free( buf );

}

void render_viewboard( user_t *u, player_t *p1, player_t *p2 )
{
     string_t *buf;
     char s1[ 35 ], s2[ 35 ];
     int i;
               
     sprintf( s1, "%s's ships", p1 -> u -> current_name );
     sprintf( s2, "%s's ships", p2 -> u -> current_name );
     buf = string_alloc( "   %-30.30s       %-30.30s\n", s1, s2 );
     string_append( buf, "   ^cA B C D E F G H                      A B C D E F G H\n" );

     for( i = 0; i < 8; i++ )
     {        
          string_append( buf, " ^c%d", i + 1 );
          render_line( &buf, &( p1 -> grid[ i ][ 0 ] ), 0 );
          string_append( buf, "                   " );
          string_append( buf, " ^c%d", i + 1 );
          render_line( &buf, &( p2 -> grid[ i ][ 0 ] ), 0 );
          string_append( buf, "^n\n" );
     }                   

     command_output( "%s\n", buf -> data );

     string_free( buf );
}
               
/* sets x and y to grid positions     */
/* returns 1 on success, 1 on failure */
int evaluate_move( char *str, int *x, int *y )
{
     if( !*str || !*( str + 1 ) )
          return 1;

     if ( *str >= 'a' && *str <= 'h' )
          *x = ( int )( *str - 'a' );
     else if ( *str >= 'A' && *str <= 'H' )
          *x = ( int )( *str - 'A' );
     else 
          return 1;
  
     if ( *( str + 1 ) >= '1' && *( str + 1 ) <= '8' )
          *y = ( int )( *( str + 1 ) - '1' );
     else 
          return 1;
  
     return 0;
}

/* returns how many parts remaining if true, 0 if false */
int ship_afloat( player_t *p, int ship )
{
     int a, b;
     int r = 0;
     
     for( a = 0; a < 8; a++ )
          for( b = 0; b < 8; b++ )
               if( ( p -> grid[ a ][ b ] & ship ) && !( p -> grid[ a ][ b ] & BOMBED ) )
                    r++;

     return r;
}

void bsh( user_t *u, int argc, char *argv[] )
{
     game_t *game;
     user_t *opponent;          
     player_t *p, *opp;
     int x, y;
     int status;

     game = find_game_by_user( u );

     if( !game )
     {
          if( argc < 2 )
          {           
               command_output( "Format: %s <opponent>\n", argv[ 0 ] );
               return;
          }

          opponent = user_find_on_by_name( u, argv[ 1 ] );

          if( !opponent )
               return;                   

          game = find_game_by_user( opponent );
          if( game )
          {
               command_output( "Sorry, that user is already playing a game of battleships\n" );
               return;
          }

          game = add_game( u, opponent );
          if( !game )
          {
               command_output( "Sorry, an error occured, you may not play battleships at this time\n" );
               return;
          }           

          user_output( CGAMES, u, opponent, "^W%s has challenged you to a game of battleships\n^WType 'bsh accept' or 'bsh decline'^n\n", u -> current_name );
          command_output( "You challenge %s to a game of battleships...\n", opponent -> current_name );
     }
     else
     {
          p   = get_player( u, game );
          opp = ( p == game -> p1 ) ? game -> p2 : game -> p1;

          if( !( p -> flags & READY ) )
          {
               render_preboard( p );
               return;
          }                         
                      
          if( !( opp -> flags & READY ) )
          {
               command_output( "Waiting for your opponent to select a layout\n" );
               return;
          }

          if( opp -> u == NULL )
          {                   
               render_preboard( p );
               command_output( "Your opponent is logged out at the moment\n" );
               return;
          }

          if( p != game -> turn )
          {                                       
               render_board( p, opp );
               command_output( "It is %s's turn\n", opp -> u -> current_name );
               return;
          }

          if( evaluate_move( argv[ 1 ], &x, &y ) )
          {
               render_board( p, opp );
               command_output( "You must specify a grid position from A1 to H8!\n" );
               return;
          }

          if( opp -> grid[ y ][ x ] & BOMBED )
          {
               render_board( p, opp );
               command_output( "You have already attacked those co-ordinates!\n" );
               return;
          }
                              
          opp -> grid[ y ][ x ] |= BOMBED;


          if( !( opp -> grid[ y ][ x ] & EMPTY ) )
          {     
               p -> score++;

               if( p -> score < 16 )
               {
                    render_board( p, opp );
                    render_board( opp, p );

                    status = ship_afloat( opp, opp -> grid[ y ][ x ] & ~BOMBED );

                    if( status == 0 )
                    {
                         user_output( CGAMES, u, opp -> u, "%s tries %c%d, ^RHIT!^N They sunk a ship and get another turn\n", u -> current_name, ( char ) x + 'A', y + 1 );
                         command_output( "You try %c%d, ^RHIT!^N The ship is sunk! You get another turn\n", ( char ) x + 'A', y + 1 );
                    }
                    else
                    {
                         user_output( CGAMES, u, opp -> u, "%s tries %c%d, ^RHIT!^N They get another turn\n", u -> current_name, ( char ) x + 'A', y + 1 );
                         command_output( "You try %c%d, ^RHIT!^N You get another turn\n", ( char ) x + 'A', y + 1 );
                    }
                    return;
               }
               else
               {
                    render_board( p, opp );
                    render_postboard( opp, p );
                    group_output( CGAMES, universe -> connected, "^g->^n Shiver my timbers! %s just beat %s at Battleships!\n", u -> current_name, opp -> u -> current_name );
                    remove_game( game );
               }
          }
          else
          {           
               render_board( p, opp );
               render_board( opp, p );

               game -> turn = opp;
               user_output( CGAMES, u, opp -> u, "%s tries %c%d. ^WMISS!^N It is now your turn\n", u -> current_name, ( char ) x + 'A', y + 1 );
               command_output( "^WMISS!^N It is now %s's turn\n", opp -> u -> current_name );
          }
     }
}

void bsh_who( user_t *u, int argc, char *argv[] )
{
     game_t *mover;
     int i;
     string_t *buf;
                   
     mover = games;

     if( mover == NULL )
     {           
          command_output( "There are no Battleships games in progress\n" );
          return;
     }

     buf = string_alloc( "" );

     string_append_title( u, buf, LEFT, "Battleships Games in Progress" );
           
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

void bsh_view( user_t *u, int argc, char *argv[] )
{
     game_t *mover;
     int r;

     if( argc < 2 )
     {
          command_output( "Format: bsh %s <game number>\n" );
          return;
     }

     r = atoi( argv[ 1 ] ) - 1;

     mover = games;

     while( mover && r )
     {
          mover = mover -> next;
          r--;
     }        

     if( mover == NULL )
     {
          command_output( "Unknown game \'%s\'. Type \'bsh who\' for a list of current games\n", argv[ 1 ] );
          return;
     }

     if( mover -> p1 -> u == NULL || mover -> p2 -> u == NULL )
     {
          command_output( "That game is currently suspended\n" );
          return;
     }           

     if( !( mover -> p2 -> flags & READY ) )
     {
          command_output( "That game has not yet begun\n" );
          return;
     }

     render_viewboard( u, mover -> p1, mover -> p2 );
     
     if( u != mover -> p1 -> u && u != mover -> p2 -> u )
     {
          user_output( CGAMES, u, mover -> p1 -> u, "^G->^n %s just viewed your game\n", u -> current_name );
          user_output( CGAMES, u, mover -> p2 -> u, "^G->^n %s just viewed your game\n", u -> current_name );
     }
}

void bsh_accept( user_t *u, int argc, char *argv[ ] )
{
     game_t *game;

     game = find_game_by_user( u );

     if( !game )
     {
          command_output( "No one has challenged you to a game of battleships!\n" );
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

     randomise_board( game -> p1 );
     randomise_board( game -> p2 );

     render_preboard( game -> p1 );
     render_preboard( game -> p2 );
}

void bsh_decline( user_t *u, int argc, char *argv[ ] )
{
     game_t *game;                            

     game = find_game_by_user( u );

     if( !game )
     {
          command_output( "No one has challenged you to a game of battleships!\n" );
          return;
     }           

     if( game -> p2 -> u != u )
     {
          command_output( "You are the challenger! (use bsh quit instead)\n" );
          return;
     }

     if( game -> p2 -> flags & ACCEPTED )
     {
          command_output( "The game has already been accepted (use bsh quit instead)\n" );
          return;
     }           

     user_output( CGAMES, u, game -> p1 -> u, "%s declines your battleships offer\n", u -> current_name );
     command_output( "You decline the battleships offer from %s\n", game -> p1 -> u -> current_name );

     remove_game( game );
}

void bsh_random( user_t *u, int argc, char *argv[ ] )
{
     game_t *game;
     player_t *p;

     game = find_game_by_user( u );

     if( !game )
     {
          command_output( "You are not playing battleships!\n" );
          return;
     }           

     if( !( game -> p2 -> flags & ACCEPTED ) )
     {
          command_output( "The game has not yet been accepted\n" );
          return;
     }           

     p = get_player( u, game );

     randomise_board( p );
     render_preboard( p );
}

void bsh_ready( user_t *u, int argc, char *argv[ ] )
{
     game_t *game;
     player_t *p;
     player_t *opp;

     game = find_game_by_user( u );

     if( !game )
     {
          command_output( "You are not playing battleships!\n" );
          return;
     }           

     if( !( game -> p2 -> flags & ACCEPTED ) )
     {
          command_output( "The game has not yet been accepted\n" );
          return;
     }           

     p = get_player( u, game );

     if( p -> flags & READY )
          return;

     opp = ( p == game -> p1 ) ? game -> p2 : game -> p1;

     if( opp -> u == NULL )
     {
          command_output( "Your game is suspended because your opponent is logged out\n" );
          return;
     }

     p -> flags |= READY;


     if( !( opp -> flags & READY ) )
     {
          user_output( CGAMES, u, opp -> u, "%s is ready to play, waiting for you...\n", u -> current_name );
          command_output( "You are ready to play, waiting for %s...\n", opp -> u -> current_name );
          return;
     }

     game -> turn = ( rand( ) % 2 ) ? game -> p1 : game -> p2;
     opp = ( game -> turn == game -> p1 ) ? game -> p2 : game -> p1;

     render_board( game -> p1, game -> p2 );
     render_board( game -> p2, game -> p1 );

     user_output( CGAMES, NULL, game -> turn -> u, "You get to go first\n" );
     user_output( CGAMES, NULL, opp -> u, "%s gets to go first\n", game -> turn -> u -> current_name );
}

void bsh_quit( user_t *u, int argc, char *argv[ ] )
{
     game_t *game;
     player_t *p;
     player_t *opp;

     game = find_game_by_user( u );

     if( !game )
     {
          command_output( "You are not playing battleships!\n" );
          return;
     }           

     p = get_player( u, game );
     opp = ( p == game -> p1 ) ? game -> p2 : game -> p1;

     if( opp -> u )
          user_output( CGAMES, u, opp -> u, "%s has just quit your game of battleships!\n", u -> current_name );

     command_output( "You quit your game of battleships\n" );

     remove_game( game );
}

void plugin_init( plugin_s *p_handle )
{          
     add_command(    "bsh",            bsh,         p_handle, CMD_GAMES,    1 );
     add_subcommand( "bsh", "accept",  bsh_accept,  p_handle, CMD_GAMES,    0 );
     add_subcommand( "bsh", "decline", bsh_decline, p_handle, CMD_GAMES,    0 );
     add_subcommand( "bsh", "random",  bsh_random,  p_handle, CMD_GAMES,    0 );
     add_subcommand( "bsh", "rand",    bsh_random,  p_handle, CMD_INVIS,    0 );
     add_subcommand( "bsh", "ready",   bsh_ready,   p_handle, CMD_GAMES,    0 );
     add_subcommand( "bsh", "quit",    bsh_quit,    p_handle, CMD_GAMES,    0 );
     add_subcommand( "bsh", "who",     bsh_who,     p_handle, CMD_GAMES,    0 );
     add_subcommand( "bsh", "view",    bsh_view,    p_handle, CMD_GAMES,    0 );
}

void PLUGIN_USER_LOGIN( user_t *u )
{                       
     game_t *mover;
     player_t *p;
                                
     mover = games;

     while( mover )
     {             
          if( mover -> p1 -> userid == u -> userid )
          {
               mover -> p1 -> u = u;
               user_output( CGAMES, NULL, u, "Your game of battleships against %s has resumed\n", mover -> p2 -> u -> current_name );

               user_output( CGAMES, NULL, mover -> p2 -> u, "Your battleships game against %s has resumed\n", u -> current_name );
                 
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
               user_output( CGAMES, NULL, u, "Your game of battleships against %s has resumed\n", mover -> p1 -> u -> current_name );

               user_output( CGAMES, NULL, mover -> p1 -> u, "Your battleships game against %s has resumed\n", u -> current_name );
               
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
                              
     mover = games;

     while( mover )
     {             
          if( mover -> p1 -> u == u )
          {                
               if( !( mover -> p2 -> flags & ACCEPTED ) )
               {
                    user_output( CGAMES, NULL, mover -> p2 -> u, "%s withdraws their battleships game offer\n", u -> current_name );
                    remove = mover;
               }
               else if( mover -> p2 -> u != NULL )
               {
                    mover -> p1 -> u = NULL;
                    user_output( CGAMES, NULL, mover -> p2 -> u, "Your battleships game is suspended\n" );
               }                            
               else
                    remove = mover;
          }
          else if( mover -> p2 -> u == u )
          {                
               if( !( mover -> p2 -> flags & ACCEPTED ) )
               {
                    user_output( CGAMES, NULL, mover -> p1 -> u, "%s declines your battleships offer\n", u -> current_name );
                    remove = mover;
               }
               else if( mover -> p1 -> u != NULL )
               {
                    mover -> p2 -> u = NULL;
                    user_output( CGAMES, NULL, mover -> p1 -> u, "Your battleships game is suspended\n" );
               }                            
               else
                    remove = mover;
          }

          mover = mover -> next;
     }

     if( remove )
          remove_game( remove );          
}

void PLUGIN_USER_RECONNECT( user_t*old, user_t *new )
{
     game_t *mover;
                              
     mover = games;

     while( mover )
     {             
          if( mover -> p1 -> u == old )
               mover -> p1 -> u = new;
          else if( mover -> p2 -> u == old )
               mover -> p2 -> u = new;

          mover = mover -> next;
     }
}

