/* 
 * prophecy.c 
 * The Prophet Johnny Maglash dispenses arbitrary wisdom
 *
 * Author: Johnny Maglash
 */

#include "plugin_headers.h"
  
PLUGIN_NAME(         "Prophecy Plug-in" )
PLUGIN_VERSION(      "1.0.1"  )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "the prophet johnny maglash" )

char* getEightBallPhrase()
{
  int a = rand()%9;
  
  switch( a )
  {
    case 0:
      return "Outlook hazy. Ask again later.";
    case 1:
      return "Don't count on it!";
    case 2:
      return "Of course!";
    case 3:
      return "I'm not going to touch that with an 80 foot pole.";
    case 4:
      return "Is Tony Shave a catholic?";
    case 5:
      return "Is Chris Dixon a hammy twaart?";
    case 6:
      return "Does Stuart Baird shit in the woods?";
    case 7:
      return "That's a negatory good buddy.";
    case 8:
      return "At this time I can neither confirm nor deny this.";
  }
  
  return "Whatever...";
}

void eightBall( user_t *u, int argc, char** argv )
{ 
  string_t* strDump;
  char*     strProphecy = getEightBallPhrase();
  int       i;
  
  strDump = string_alloc( "%s asked the great prophet Johnny Maglash... \"", u -> current_name );

  for ( i = 1; i < argc; i++ )
    string_append( strDump, argv[ i ] );

  string_append( strDump, "\"^w...and he replies: \"%s\"^w\n", strProphecy );
  
  group_output( CGAMES, universe -> connected, strDump -> data );
  
  string_free( strDump );
}

/* Generates a random prophecy and returns it as a string. Of course,
 * Johnny's prophecies are not random, but were made at an earlier time
 * and recorded in the source code. 
 */
char* getProphecy()
{
  int a = rand()%14;

  switch( a )
  {
    case 0:
      return "argv 0 = 'prophecy'";
    case 1:
      return "Epsilon Epsilon Epsilon Epsilon Epsilon Will Be Gone.";
    case 2:
      return "In 2004 the Hautins Ghetto will be a smoking hole.";
    case 3:
      return "Be wary! One day Arnold Pike will rule the world.";
    case 4:
      return "Curses! Dixon will continue to be hammy twaart.";
    case 5:
      return "Run for the hills! Stuart will never cheer up.";
    case 6:
      return "Ye Gods! Even I can't tell if Horace will graduate.";
    case 7:
      return "Awww! Graeme Christ will never make milkshake again.";
    case 8:
      return "Get your arse to Mars.";
    case 9:
      return "You will arrive when you get there.";
    case 10:
      return "Tony Shave will marry Colin Cook.";
    case 11:
      return "Horace will remain a dirty slut.";
    case 12:
      return "French snack food will continue to annoy Johnny Maglash.";
    case 13:
      return "Jimmy GalaK will one day lose his meteological prowess.";
    
  }     
  
  /* Your free prophecy, courtesy of Johnny.
   * Will only be returned if we run out of random numebrs
   */
  return "Hello Mrs. Cartman. How are you today?";
}

void simpleProphecy( user_t *u )
{ 
  string_t* strDump;
  char*     strProphecy = getProphecy();
  char*     strTopLine = "[ The Prophet ^YJohnny Maglash^w he say: ]";
  int       nLen = strlen( strProphecy );
  int       nLenTopLine = strlen( strTopLine );
  int       i;                              
                
  strDump = string_alloc( "+--%s", strTopLine );

  while( nLenTopLine < nLen )
  {
    string_append( strDump, "-" );
    nLenTopLine++;
  }
  
  if ( !nLenTopLine < nLen )
    string_append( strDump, "--" );
  
  string_append( strDump, "--+\n" );
  
  /* Insert a blank line
  */
  string_append( strDump, "|" );
  for ( i = 0; i < nLenTopLine + 2; i++ )
    string_append( strDump, " " );
  string_append( strDump, "|\n" );

  /* Insert the prophecy and pad the line accordingly
  */
  string_append( strDump, "| ^A%s^w ", strProphecy );
  while( nLen < nLenTopLine )
  {
    string_append( strDump, " " );
    nLen++;
  }
  string_append( strDump, "|\n" );

  /* Insert a blank line
  */
  string_append( strDump, "|" );
  for ( i = 0; i < nLen + 2; i++ )
    string_append( strDump, " " );
  string_append( strDump, "|\n" );
  
  /* Build the bottom of the box
  */
  string_append( strDump, "+" );
  for ( i = 0; i < nLen + 2; i++ )
    string_append( strDump, "-" );
  string_append( strDump, "+\n" );
    
  command_output( strDump -> data );
  
  string_free( strDump );
}

/* Called when someone requests a prophecy from Johnny Maglash.
 * This method decides if an explicit prophecy is required, or whether
 * an 8-ball style forecast will suffice.
 */
void prophecy( user_t *u, int argc, char *argv[] )
{
  if ( argc == 1 )
    simpleProphecy( u );
  else
    eightBall( u, argc, argv );
}

/* called as the plugin is loaded */
void plugin_init( plugin_s *p_handle )
{            
     add_command( "prophecy", prophecy, p_handle, CMD_GAMES ,0 );

     return;
}           

