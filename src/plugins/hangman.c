#include <stdio.h>

#include "plugin_headers.h"

/* Hangman Game */
/* by Horace */

PLUGIN_NAME( "Hangman" )
PLUGIN_VERSION( "0.11" )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "horace" )

char *hangman_board[11][6] =
{
  {
  " _____  ",
  " |/  |  ",
  " |   O  ",
  " |  /|\\ ",
  " |  / \\ ",
  "--------",
  },
  {
  " _____  ",
  " |/  |  ",
  " |   O  ",
  " |  /|\\ ",
  " |  /   ",
  "--------",
  },
  {
  " _____  ",
  " |/  |  ",
  " |   O  ",
  " |  /|\\ ",
  " |      ",
  "--------",
  },
  {
  " _____  ",
  " |/  |  ",
  " |   O  ",
  " |  /|  ",
  " |      ",
  "--------",
  },
  {
  " _____  ",
  " |/  |  ",
  " |   O  ",
  " |   |  ",
  " |      ",
  "--------",
  },
  {
  " _____  ",
  " |/  |  ",
  " |   O  ",
  " |      ",
  " |      ",
  "--------",
  },
  {
  " _____  ",
  " |/     ",
  " |      ",
  " |      ",
  " |      ",
  "--------",
  },
  {
  " _____  ",
  " |      ",
  " |      ",
  " |      ",
  " |      ",
  "--------",
  },
  {
  "        ",
  " |      ",
  " |      ",
  " |      ",
  " |      ",
  "--------",
  },
  {
  "        ",
  "        ",
  "        ",
  "        ",
  "        ",
  "--------",
  },
  {
  "        ",
  " Nothing",
  "   so   ",
  "  far!  ",
  "        ",
  "        ",
  }
};

struct HANGMAN{
  int p1uid;
  int p2uid;
  char *p1word;
  char *p2word;
  char *p1gword;
  char *p2gword;
  int p1status;
  int p2status;
  int numturns;
  int turn;
  struct HANGMAN *next;
};

struct HANGMAN *hangman_top = NULL;

void end_game_message(user_t *u, struct HANGMAN *game, int player, int show_both_words);
void remove_game (struct HANGMAN *game);
void show_board(struct HANGMAN *game, int player);
char *make_guess_word(char *gword);
void display_guess_word(user_t *u, user_t *opp, char *gword);

void hangman_add_game(int player, int opponent, char *word, char *gword)
{
  struct HANGMAN *new;

  if ( (new = (struct HANGMAN *) malloc(sizeof(struct HANGMAN))) == NULL)
  {
    add_log("error", "Out of memory to malloc in hangman.c");
    return;
  }

  new->p1uid = player;
  new->p2uid = opponent;
  new->p1word = word;
  new->p2word = 0;
  new->p1gword = 0;
  new->p2gword = gword;
  new->p1status = 10;
  new->p2status = 10; 
  new->turn = player;
  new->numturns = 0;

  if (hangman_top == NULL) {
    hangman_top = new;
    new->next = hangman_top;
  } else {
    new->next = hangman_top;
    hangman_top = new;
  }
}

void remove_game (struct HANGMAN *game) {
     struct HANGMAN *mover = hangman_top->next;

     /* Check to see if any games */
     if (hangman_top == NULL) {
       return;
     }

     /* Find game and remove game*/
     if (hangman_top == game) {
       if(hangman_top->next == hangman_top) {
          hangman_top = NULL;
          free(game);
       } else {
         hangman_top = game->next;
         free(game);
       }
     } else {
       while (hangman_top != mover->next) {
         if (mover == game) {
           free(game);
         }
         mover = mover->next;
       }
     }
}

/* Returns game if found otherwise NULL */
struct HANGMAN *find_game_by_opp ( int opponent ) {

     struct HANGMAN *mover = hangman_top;

     /* Check to see if any games */
     if (hangman_top == NULL) {
       return NULL;
     }

     /* Find opponent */
     if (mover->p2uid == opponent) {
       return mover;
     }
     
     while (mover->next != hangman_top) {
       if (mover->p2uid == opponent) {
         return mover;
       }
       mover = mover->next;
     }
     return NULL;
}

/* Returns game if found otherwise NULL */
struct HANGMAN *find_game_by_chal ( int challenger ) {
     struct HANGMAN *mover = hangman_top;

     /* Check to see if any games */
     if (hangman_top == NULL) {
       return NULL;
     }

     /* Find opponent */
     if (mover->p1uid == challenger) {
       return mover;
     }

     while (mover->next != hangman_top) {
       if (mover->p1uid == challenger) {
         return mover;
       }
       mover = mover->next;
     }
     return NULL;
}

/* Returns true if user is already in a game */
int find_user_in_game( int uid ) {
     struct HANGMAN *mover = hangman_top;
     
     /* Check to see if any games */
     if (hangman_top == NULL) {
       return 0;
     }
     
     /* Find user */
     if (mover->p1uid == uid
         || mover->p2uid == uid) {
       return 1;
     }

     while (mover->next != hangman_top) {
       if (mover->p1uid == uid 
           || mover->p2uid == uid) {
         return 1;
       }
       mover = mover->next;
     }
     return 0;
}

void hangman_accept( user_t *u, int argc, char *argv[] )
{
     int challenger;
     user_t *s_chal;
     struct HANGMAN *game;

     /* Check to see if correct number of arguments entered */
     if (argc != 2) {
       tell_user( u ,"^RWrong number of arguments!^N\n");
       tell_user( u ,"^RUsage: hmacpt [challenge word]^N\n");
       return;
     }
     
     /* Check to see if player has been challenged */
     game = find_game_by_opp( u->userid );

     if (game == NULL) {
       if (find_user_in_game(u->userid) == 0) {
         tell_user ( u , "^RYou have not been challenged to a game!^N\n");
         tell_user ( u , "^RUse hmchal [user] [challenge word] to start a new game.^N\n");
         return;
       } else {
         tell_user ( u, "^RYou are currently challenging a player to a game.^N\n");
         return;
       }       
     }
     
     /* Check to see if game is not already started */
     if (game->numturns != 0) {
       tell_user( u, "^RGame has already started, no need to accept^N\n");
       return;
     }
     
     /* Game is started */
     tell_user ( u , "^RGame has begun!^N\n" );
     tell_user ( u , "^RWaiting for challenger to make first guess.^N\n");
     challenger = game->p1uid;
     s_chal = user_find_on_by_id( u , challenger);
     tell_user ( s_chal , "^RGame has begun!^N\n");
     tell_user ( s_chal , "Opponent has given you a %d letter(s) word.\n", strlen(argv[1]));
     tell_user ( s_chal , "Use hm [guess letter] to play\n");
     
     game->p2word = strdup( argv[1] );
     
     game->p1gword = make_guess_word(strdup(argv[1]));
     game->numturns = 1;
}

char *make_guess_word(char *gword)
{
     int x;
     char *tempword, *ptr;;

     tempword = ALLOC( strlen( gword ) + 1, char );
     ptr = tempword;

     for (x=1; x <= strlen(gword); x++) {
       *ptr++ = '_';
     }
     *ptr = 0;

     return tempword;
}   
                                   
void hangman_decline( user_t *u, int argc, char *argv[] )
{
     struct HANGMAN *game = NULL;
     int challenger;
     user_t *s_chal;

     /* Check to see if player has been challenged */
     game = find_game_by_opp( u->userid );

     if (game == NULL) {
       tell_user ( u , "^RYou have not been challenged to a game!^N\n");
       return;
     }

     /* Remove game */
     challenger = game->p1uid;
     remove_game(game);
     tell_user( u , "^RGame cancelled!^N\n");
     s_chal = user_find_on_by_id( u , challenger);
     tell_user( s_chal , "^RHangman is denied!^N\n");     
}

void hangman_guess( user_t *u, int argc, char *argv[] )
{
     int x;
     struct HANGMAN *game;
     int player;
     int status;
     char *word;
     char *gword;     
     char *ptr_word, *ptr_guess;
     int success = 0;
     char guess_let;
     user_t *opp;
       
     /* Check to see if right number of arguments */
     if (argc != 2) {
        tell_user( u , "usage of command hm [guess letter]\n");
        return;
     }
     
     /* Check to see if only one letter in argument */
     if (strlen(argv[1]) != 1) {
        tell_user( u, "^REnter only one letter at a time!^N\n");
        return;
     }
     
     /* Check to see if user in game */
     if (find_user_in_game(u->userid) != 1) {
       tell_user( u ,"^RYou are not currently in a game of hangman!^N\n");
       tell_user( u ,"To start a game type [opponent name] [challenge word]\n");
       return;
     }
     
     guess_let = *argv[1];
     
     /* Find game & player and setup */
     game = find_game_by_chal (u->userid);
     if (game == NULL) {
       game = find_game_by_opp (u->userid);
       player = 2;
       status = game->p2status;
       word = game->p1word;
       gword = game->p2gword;
       opp = user_find_on_by_id ( u , game->p1uid);               
     } else {
       player = 1;
       status = game->p1status;
       word = game->p2word;
       gword = game->p1gword;
       opp = user_find_on_by_id ( u , game->p2uid);
     }
     
     /* Check to see if game has been accepted */
     if(game->numturns == 0) {
       tell_user( u , "User has not accepted game yet!\n");
       return;
     }
     
     /* Check to see if users turn */
     if(game->turn != u->userid) {
       tell_user ( u , "^RIt is not your go!^N\n");
       return;
     }
 
     /* Check word for guessed letter */
     ptr_word = word;
     ptr_guess = gword;

     for(x=0; x <= strlen(word); x++, ptr_word++, ptr_guess++) {
        if(*ptr_word == guess_let) {
          memcpy(ptr_guess, &guess_let, 1);
          success = 1;
        }
     }
     
     if (success == 1) {
       if (strcmp(word,gword) == 0) {
         end_game_message(u , game, player, 0);
         return;
       } else {         
         tell_user ( u , "You have guessed a letter correctly!\n\n");
       }
     } else {
        status--;
        tell_user ( u , "That letter is not in the word!\n\n");
     }
 
     /* Check to see if game has ended */
     if (status == 0) {
       if(player==1) {
         end_game_message(u, game, 2, 1);
       } else {
         end_game_message(u, game, 1, 1);
       }
       return;
     }

     /* Update game */
     game->numturns++;
     if(player == 1) {
        game->p1gword = gword;
        game->p1status = status; 
        if(success == 0) {
          game->turn = game->p2uid;
        }
     } else {
        game->p2gword = gword;
        game->p2status = status;
        if(success == 0) {
          game->turn = game->p1uid;
        }
     }
     
     show_board(game, player);
     display_guess_word(u, u, gword);
     
     if(success == 1) {
         tell_user(u, "^RYou get another go!^N\n");
         tell_user(u, "Type hm [guess letter] to play.\n");
         tell_user(opp, "^R%s guessed a letter correctly and gets another go.^N\n", u->current_name);
         display_guess_word(opp, u, gword);
     } else {
        tell_user( opp, "^RYour go at hangman!^N\n");
        tell_user( opp, "Type hm [guess letter] to play.\n");
     }
}

void display_guess_word(user_t *u, user_t *opp, char *gword)
{
     char *ptr_word, *temp;
     int x;
     
     ptr_word = gword;
     temp = ALLOC(strlen(gword) * 2 + 1, char );
     for (x = 0; x <= strlen(gword); x++, ptr_word++)
     {
       *(temp + (x*2)) = *ptr_word;
       *(temp + (x*2) + 1) = ' ';
     }
     tell_user( u , "^G%s : %s^N\n\n", opp->current_name, temp);
}

void show_board(struct HANGMAN *game, int player)
{
     int cur_stat, opp_stat;
     char *cur_name, *opp_name;
     user_t *cur_user, *opp_user;
     int x;

     if(player==1) {
       cur_stat = game->p1status;
       opp_stat = game->p2status;
       cur_user = user_find_on_by_id(cur_user, game->p1uid);
       opp_user = user_find_on_by_id(opp_user, game->p2uid);
     } else {
       cur_stat = game->p2status;
       opp_stat = game->p1status;
       cur_user = user_find_on_by_id(cur_user, game->p2uid);
       opp_user = user_find_on_by_id(opp_user, game->p1uid);
     }
     cur_name = cur_user->current_name;
     opp_name = opp_user->current_name;
     
     tell_user( cur_user , "^b%-12.12s %-12.12s^N\n\n", cur_name, opp_name);         
     for (x=0; x<=5; x++) {
       tell_user( cur_user , "^b%s     %s^N\n",hangman_board[cur_stat][x], hangman_board[opp_stat][x]);
     }
}

void end_game_message(user_t *u, struct HANGMAN *game, int player, int show_both_words) 
{
     user_t *winner, *loser;
     char *w_word, *l_word;
  
     if(player == 1) {
       winner = user_find_on_by_id(u , game->p1uid);
       loser = user_find_on_by_id(u , game->p2uid);
       w_word = game->p2word;
       l_word = game->p1word;
     } else {
       winner = user_find_on_by_id(u , game->p2uid);
       loser = user_find_on_by_id(u , game->p1uid);
       w_word = game->p1word;
       l_word = game->p2word;
     }
     
     tell_user ( winner, "^RCONGRATULATIONS! You have won hangman against %s!!^N\n", loser->current_name);
     if (show_both_words == 1) {
       tell_user ( winner, "You were trying to guess the word %s.\n", w_word);
     }
            
     tell_user ( loser, "^RGUTTED! You have lost hangman against %s.^N\n", winner->current_name);      

     tell_user ( loser, "You were trying to guess the word %s.\n", l_word);
      
     remove_game(game);
}                          
    
void hangman_challenge( user_t *u, int argc, char *argv[] )
{
     user_t *opponent;
     
     /* Check to see if correct number of arguments entered */
     if (argc != 3) {
       tell_user( u ,"^RWrong number of arguments!^N\n");
       tell_user( u ,"Usage: hmchal [challenge user] [challenge word]\n");
       return;
     }
     
     /* Find opponent */
     opponent = user_find_on_by_name( u , argv[1] );
     
     /* Check to see if challenging themselfs */
     if (opponent == u) {
       return;
     }
     
     /* Check to see if opponent exists */
     if (opponent == NULL) {
       return;
     }

     /* Check to see if they are already in a game */
     if (find_game_by_chal( u->userid ) != NULL ||
         find_game_by_opp( u->userid ) != NULL ) {
       tell_user ( u , "^RYou cannot play more than one game at a time.^N\n");
       return;
     }

     /* Check to see if opponent is already in a game */
     if (find_game_by_chal( opponent->userid ) != NULL ||
         find_game_by_opp( opponent->userid ) != NULL ) {
       tell_user ( u , "^RUser is already playing hangman.^N\n");
       return;
     }
          
     tell_user ( u , "^RWaiting for opponent to accept.^N\n");
     
     tell_user ( opponent , "^RYou have been challanged to a game of hangman!!^N\n");
     tell_user ( opponent , "Challenged word is %d letter(s) long.\n", strlen(argv[2])); 
     tell_user ( opponent , "Type hmacpt [word] to accept game OR hmdcln to decline game\n");
     
     hangman_add_game(u->userid , opponent->userid, strdup(argv[2]), make_guess_word(strdup(argv[2])));
}

void hangman_debug( user_t *u, int argc, char *argv[] )
{
   struct HANGMAN *mover = hangman_top;
   int index = 1;

   if(hangman_top == NULL) {
     tell_user( u , "hangman_top = NULL\n\n");
   } else {
     tell_user( u , "hangman_top is not NULL\n\n");
     tell_user ( u , "********* GAME %d *********\n", index );
     tell_user ( u , "p1uid = %d\n", mover->p1uid );
     tell_user ( u , "p2uid = %d\n", mover->p2uid );
     tell_user ( u , "p1word = %s\n", mover->p1word );
     tell_user ( u , "p2word = %s\n", mover->p2word );
     tell_user ( u , "p1gword = %s\n", mover->p1gword );
     tell_user ( u , "p2gword = %s\n", mover->p2gword );
     tell_user ( u , "turn = %d\n", mover->turn );
     tell_user ( u , "numturns = %d\n", mover->numturns );
     while (mover != hangman_top->next) {
       index++;
       tell_user ( u , "********* GAME %d *********\n", index );
       tell_user ( u , "p1uid = %d\n", mover->p1uid );
       tell_user ( u , "p2uid = %d\n", mover->p2uid );
       tell_user ( u , "p1word = %s\n", mover->p1word );
       tell_user ( u , "p2word = %s\n", mover->p2word );
       tell_user ( u , "p1gword = %s\n", mover->p1gword );
       tell_user ( u , "p2gword = %s\n", mover->p2gword );
       tell_user ( u , "turn = %d\n", mover->turn );
       tell_user ( u , "numturns = %d\n", mover->numturns );
       if (mover->next == NULL) {
         tell_user ( u , "next = NULL!!!! this should not happen!\n");
         return;
       }
     }
     mover = mover->next;
  }   
}


void rules( user_t *u, int argc, char *argv[] )
{
     if( argc < 2 )
     {
          tell_user( u, "Format : %s <game>\n", argv[ 0 ] );
          return;
     }

     if( !strcasecmp( argv[ 1 ], "hangman") 
      || !strcasecmp( argv[ 1 ], "hmchal")
      || !strcasecmp( argv[ 1 ], "hmacpt")
      || !strcasecmp( argv[ 1 ], "hmdcln")
      || !strcasecmp( argv[ 1 ], "hmdebug") 
      || !strcasecmp( argv[ 1 ], "hm") )
          tell_user( u, get_textmessage( "hangman" ) );
     else
          tell_user( u, "Unknown game \'%s\'!\n", argv[ 1 ] );
}
 
void plugin_init( plugin_s *p_handle )
{          
       add_command( "hmchal", hangman_challenge, p_handle, CMD_GAMES    ,1 );
       add_command( "hm", hangman_guess , p_handle, CMD_GAMES ,0 );
       add_command( "hmacpt", hangman_accept , p_handle, CMD_GAMES ,0 );
       add_command( "hmdcln", hangman_decline , p_handle, CMD_GAMES, 0);
       /*add_command( "hmdebug", hangman_debug, p_handle, CMD_GAMES, 0);*/

       return;
}           

void plugin_fini( plugin_s *p_handle )
{
}
