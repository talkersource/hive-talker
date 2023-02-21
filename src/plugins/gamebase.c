#include <stdio.h>

#include "plugin_headers.h"

PLUGIN_NAME( "Game Base" )
PLUGIN_VERSION( "0.0.1"  )
PLUGIN_DEPENDENCIES( "textfiles.so" )
PLUGIN_AUTHORS_LIST( "galak" )

/* Dice Game */

char *dice_faces[ 6 ][ 3 ] =
{
  {
  "     ",
  "  *  ",
  "     ",
  },
  {
  "*    ",
  "     ",
  "    *",
  },
  {
  "*    ",
  "  *  ",
  "    *",
  },
  {
  "*   *",
  "     ",
  "*   *",
  },
  {
  "*   *",
  "  *  ",
  "*   *"
  },
  {
  "*   *",
  "*   *",
  "*   *",
  }
};

void dice( user_t *u, int argc, char *argv[] )
{
     int d1, d2;
     char result_str[ 80 ];
     char *num_word[ 6 ] = { "One", "Two", "Three", "Four", "Five", "Six"};
          
     d1 = rand( ) % 6;
     d2 = rand( ) % 6;

     if ( d1 == d2 )
     {
          switch( d1 )
          {
          case 0: strcpy( result_str, "Snake ears, Double Ones!" );
                  break;
          case 1: strcpy( result_str, "Stirling Moss, Double Twos" );
                  break;
          case 2: strcpy( result_str, "Milton Keynes, Double Threes" );
                  break;
          case 3: strcpy( result_str, "Uncle Monty, Double Fours" );
                  break;
          case 4: strcpy( result_str, "Snake eyes, Double Fives" );
                  break;
          case 5: strcpy( result_str, "Good Role, Double Sixes" );
                  break;
          }
     } 
     else 
     {
          if( ( d1 + d2 ) == 5 )
               sprintf( result_str, "DocKeN!" );
          else
               sprintf( result_str, "A %s and a %s", num_word[ d1 ], num_word[ d2 ] );
     }

     command_output( "\n   ^B-------     -------\n  ^B| ^Y%s^B |   | ^Y%s^B |      ^NYou roll two dice and get:\n  ^B| ^Y%s^B |   | ^Y%s^B |      ^N%s\n  ^B| ^Y%s^B |   | ^Y%s^B |\n   ^B-------     -------^N\n", 
                     dice_faces[d1][0], 
                     dice_faces[d2][0], 
                     dice_faces[d1][1], 
                     dice_faces[d2][1],
                     result_str, 
                     dice_faces[d1][2], 
                     dice_faces[d2][2] );
}             

void rules( user_t *u, int argc, char *argv[] )
{
     if( argc < 2 )
     {
          command_output( "Format : %s <game>\n", argv[ 0 ] );
          return;
     }

     if( !strcasecmp( argv[ 1 ], "dice" ) || !strcasecmp( argv[ 1 ], "roll" ) )
          command_output( get_textmessage( "rules_dice" ) );
     else if( !strcasecmp( argv[ 1 ], "hangman" ) || !strcasecmp( argv[ 1 ], "hm" ) )
          command_output( get_textmessage( "rules_hangman" ) );
     else if( !strcasecmp( argv[ 1 ], "battleships" ) 
               || !strcasecmp( argv[ 1 ], "bships" )
               || !strcasecmp( argv[ 1 ], "bsh" ) )
          command_output( get_textmessage( "rules_bships" ) );
     else if( !strcasecmp( argv[ 1 ], "quiz" ) )
          command_output( get_textmessage( "rules_quiz" ) );
     else if( !strcasecmp( argv[ 1 ], "connect4" ) || !strcasecmp( argv[ 1 ], "cfour" ) || !strcasecmp( argv[ 1 ], "c4" ) )
          command_output( get_textmessage( "rules_cfour" ) );
     else
          command_output( "Unknown game \'%s\'!\n", argv[ 1 ] );
}

void plugin_init( plugin_s *p_handle )
{          
       add_command( "dice", dice, p_handle, CMD_GAMES, 0 );
       add_command( "roll", dice, p_handle, CMD_INVIS, 0 );
       add_command( "d",    dice, p_handle, CMD_INVIS, 0 );

       add_command( "rules", rules, p_handle, CMD_GAMES ,0 );

       return;
}           

void plugin_fini( plugin_s *p_handle )
{
}





