#include <stdio.h>
#include <stdarg.h>

#include "plugin_headers.h"

PLUGIN_NAME( "Quiz" )
PLUGIN_VERSION( "0.6" )
PLUGIN_DEPENDENCIES( "" )
PLUGIN_AUTHORS_LIST( "Grimace" )

struct USERLIST
{
	user_t *u;
	int score;
	int tick;
	char *answer;
	struct USERLIST *next;
};

struct QUIZ
{
	user_t *quizmaster;
	int round;
	char *question;
	char *answer;
	struct USERLIST *players;
};

struct QUIZ *quiz;
struct USERLIST *ghosts;

char *quiz_messages[] =
{
	"^nNo quiz is currently running.\n",
	"^nNo quiz is currently running. Type ^yquiz start^n to start one.\n",
};

void quiz_tell_players_sandwich(string_t *str, char *format, ...)
{
	struct USERLIST *mover = quiz -> players;
	string_t *sandwich;

	char *title;
	va_list argp;
     
	if (mover == NULL)
		return;

	va_start( argp, format );
	vasprintf( &title, format, argp  );
	va_end( argp );
	
	while (mover)
	{
		sandwich = string_alloc("");
		if (sandwich)
		{
			string_append_title(mover -> u, sandwich, LEFT, title);
			string_append(sandwich, str -> data);
			string_append_line(mover -> u, sandwich);
            user_output(CGAMES, mover -> u, NULL, sandwich -> data); 
			string_free(sandwich);
		}
		mover = mover -> next;
	}
}

void quiz_tell_players(char *format, ...)
{
	struct USERLIST *mover = quiz -> players;
	char *str;
	va_list argp;
     
	va_start( argp, format );
	vasprintf( &str, format, argp  );
	va_end( argp );
                    
	if (mover == NULL)
		return;

	while (mover)
	{      
        user_output( CGAMES, mover -> u, NULL, str );
		mover = mover -> next;
	}
}

void quiz_tell_all(char *format, ...)
{
	char *str;
	va_list argp;

	va_start( argp, format );
	vasprintf( &str, format, argp  );
	va_end( argp );

	quiz_tell_players(str);
    user_output( CGAMES, quiz -> quizmaster, NULL, str );
}

struct USERLIST *quiz_find_player(user_t *u)
{
	struct USERLIST *mover = quiz -> players;

	if (mover == NULL)
		return NULL;

	while (mover)
	{
		if (mover -> u -> userid == u -> userid)
			return mover;
		mover = mover -> next;
	}
	return NULL;
}

struct USERLIST *quiz_find_player_by_name(user_t *user, char *u)
{
	struct USERLIST *mover = quiz -> players;
	user_t *temp;
	
	if (mover == NULL)
		return NULL;

	if (!(temp = user_find_on_by_name(user, u)))
		return NULL;

	while (mover)
	{
		if (mover -> u == temp)
			return mover;
		mover = mover -> next;
	}
	return NULL;
}

void quiz_replace_player(user_t *old, user_t *new)
{
	struct USERLIST *node;
	if ((node = quiz_find_player(old)))
		node -> u = new;
}

void quiz_add_ghost(struct USERLIST *ghost)
{
	
}

void quiz_new_quiz(user_t *quizmaster)
{
	quiz -> quizmaster = quizmaster;
	quiz -> round = 0;
	quiz -> question = NULL;
	quiz -> answer = NULL;
	quiz -> players = NULL;

	ghosts = ALLOC(1, struct USERLIST);
}

void quiz_announce_winners()
{
	struct USERLIST *mover;
	int highest = -1;
	int winners = 0;
	string_t *str;

	if (!(str = string_alloc("")))
	{
		add_log("error", "Out of memory: quiz_announce_winners in quiz.c");
		return;
	}

	if (quiz -> quizmaster)
	{
		if (quiz -> players != NULL)
		{
			mover = quiz -> players;

			while (mover)
			{
				if (mover -> score > highest)
				{
					highest = mover -> score;
					*(str -> data) = 0;
					string_append(str, mover -> u -> current_name);
					winners = 1;
				}
				else if (mover -> score == highest)
				{
					string_append(str, "^n, %s", mover -> u -> current_name);					
					winners++;
				}
				mover = mover -> next;
			}
			string_append(str, ".^n\n");
			quiz_tell_players("^WThe quiz has finished!^n\n");			

			if (highest != 0 && winners > 0)
				quiz_tell_players_sandwich(str, "The winner%s", (winners == 1) ? " is..." : "s are...");
		}
	}
	string_free(str);
}

void quiz_end_quiz()
{
	struct USERLIST *mover;
	struct USERLIST *utemp;
	
	if (quiz -> quizmaster)
	{
		if (quiz -> players != NULL)
		{
			quiz_announce_winners();
			mover = quiz -> players;
			do
			{
				utemp = mover -> next;
				if ((mover -> answer) != NULL)
					free (mover -> answer);
				FREE (mover);
				mover = utemp;
			} while (utemp != NULL);
		}
		quiz -> quizmaster = NULL;
	}
}

int quiz_contains_player(user_t *u)
{
	struct USERLIST *mover = quiz -> players;

	if (quiz -> quizmaster -> userid == u -> userid)
		return -1;
	if (mover == NULL)
		return 0;

	while (mover)
	{
		if (mover -> u -> userid == u -> userid)
			return 1;
		mover = mover -> next;
	}
	return 0;
}

void quiz_add_player(user_t *u)
{
	struct USERLIST *mover = quiz -> players;
	struct USERLIST *new;

	if ((new = ALLOC(1, struct USERLIST)) == NULL)
	{
		add_log("error", "Out of memory: quiz_add_player in quiz.c");
		return;
	}

	new -> u = u;
	new -> score = 0;
	new -> tick = 0;
	new -> answer = NULL;
	new -> next = NULL;

	if (mover == NULL)
	{
		quiz -> players = new;
	}
	else
	{
		while (mover -> next != NULL)
			mover = mover -> next;
		mover -> next = new;
	}
}

void quiz_remove_player(user_t *u)
{
	struct USERLIST *mover = quiz -> players;
	struct USERLIST *temp;

	if (u -> userid == mover -> u -> userid)
	{
		quiz_add_ghost(mover);
		quiz -> players = mover -> next;
		FREE(mover);
	}
	else
	{
		while (mover -> next)
		{
			if (u -> userid == mover -> u -> userid)
			{
				quiz_add_ghost(mover);
				temp = mover -> next;
				mover -> next = mover -> next -> next;
				mover = mover -> next;
				FREE(temp);
				return;
			}
			mover = mover -> next;
		}
	}
}

void quiz_players(user_t *user, int argc, char *argv[])
{
	struct USERLIST *mover = quiz -> players;
	string_t *report;
	
	if (!(report = string_alloc("")))
	{
		add_log("error", "Out of memory: quiz_players in quiz.c");
		return;
	}
			
	if (!quiz -> quizmaster)
		command_output(quiz_messages[1]); /* no quiz; type quiz start */
	else
	{
		string_append_title(user, report, LEFT, "Quiz players");
		string_append(report, "^n* %-20.20s ^W(Quizmaster)^n\n", quiz -> quizmaster -> current_name);
		while (mover)
		{
			string_append(report, "^n* %-20.20s %d point%s.^n\n", mover -> u -> current_name, mover -> score, (mover -> score != 1) ? "s" : "");
			mover = mover -> next;
		}
		string_append_line(user, report);
		command_output(report -> data);
	}
	
	string_free(report);
}

void quiz_ask_question(char *question)
{
	struct USERLIST *mover = quiz -> players;
	while (mover)
	{
		free(mover -> answer);
		mover -> answer = NULL;
		mover = mover -> next;
	}
	if (quiz -> question)
	{
		free(quiz -> question);
		quiz -> question = NULL;
	}
	if (quiz -> answer)
	{
		free(quiz -> answer);
		quiz -> answer = NULL;
	}
	
	quiz -> round++;
	quiz -> question = strdup(question);
	quiz_tell_players("^yQuestion %d^n : %s\n", quiz -> round, question);
}

void quiz_make_answer(user_t *user, char *answer)
{
	struct USERLIST *u = quiz_find_player(user);

	if (u)
	{
		if (!u -> answer)
		{
			command_output("You answer '^y%s^n'\n", answer);
			u -> answer = strdup(answer);
            user_output( CGAMES, quiz -> quizmaster, NULL, "^y%s^n answers '^y%s^n'\n", u -> u -> current_name, answer);
		}
		else
		{
			command_output("You've already answered ('^y%s^n')\n", u -> answer);
		}
	}
}

void quiz_do_scores(user_t *u)
{
	struct USERLIST *mover = quiz -> players;
	string_t *str;
	int len;

	if (!(str = string_alloc("")))
	{
		add_log("error", "Out of memory: quiz_do_scores in quiz.c");
		return;
	}

	len = strlen(str -> data);

	while (mover)
	{
		if (mover -> tick > 0)
		{
			mover -> score += mover -> tick;
            user_output( CGAMES, mover -> u, NULL, "Your answer was correct!\n");
			string_append(str, "%s, ", mover -> u -> current_name);
		}
		else
		{
			if (quiz -> answer)
                user_output( CGAMES, mover -> u, NULL, "You were wrong. The correct answer was ^y%s^n\n", quiz -> answer);
			else
                user_output( CGAMES, mover -> u, NULL, "The correct answer was ^y%s^n\n", quiz -> answer);
		}
		mover -> tick = 0;
		mover = mover -> next;
	}
	if (len == strlen(str -> data))
	{
		string_append(str, "Nobody got the answer right this round.^n\n");
		quiz_tell_players(str -> data);
	}
	else
	{
		string_truncate(str, strlen(str -> data) - 2);
		string_append(str, ".^n\n");
		quiz_tell_players_sandwich(str, "Correct Answers");
	}
	string_free(str);
}

void quiz_start_quiz(user_t *user, int argc, char *argv[])
{
	user_t *u;

	if ((quiz -> quizmaster) != NULL)
	{
		if (quiz -> quizmaster -> userid == user -> userid)
		{
			command_output("You're the Quizmaster already.\n");
			if (quiz -> round == 0)
				command_output("(to start round 1, you should ask a question: use ^yquiz question^n.\n");
			else
				command_output("(to start the next round, you should ask a question: use ^yquiz question^n.\n");
		}
		else
		{
			u = quiz -> quizmaster;
			command_output("^y%s^n is already running a quiz. Type ^yquiz join^n to join in.\n", u -> current_name);
		}
	}
	else
	{
		command_output("You set up yourself the quiz.\n");
		group_output( CGAMES, universe -> connected, "^Y-=*> ^W%s^n is running a quiz! Type ^yquiz join^n to join in.\n", user -> current_name);
		quiz_new_quiz(user);
	}
}

void quiz_stop_quiz(user_t *user, int argc, char *argv[])
{
	if (!quiz -> quizmaster)
		command_output(quiz_messages[0]);
	else if (user -> userid != quiz -> quizmaster -> userid)
		command_output("You're not the Quizmaster\n");
	else
	{
		if (quiz -> round != 0)
			quiz_do_scores(user);
		command_output("You end the quiz.\n");
		quiz_end_quiz();
	}
}

void quiz_join_quiz(user_t *user, int argc, char *argv[])
{
	if (!quiz -> quizmaster)
		command_output(quiz_messages[0]);
	else if (user -> userid == quiz -> quizmaster -> userid)
		command_output("You can't play in the quiz you're running!\n");
	else if (quiz_contains_player(user) > 0)
		command_output("You're already playing the quiz sucka.\n");
	else
	{
		command_output("You join the quiz.\n");
		quiz_tell_all("^y%s^n has joined the quiz!\n", user -> current_name);
		quiz_add_player(user);
	}
}

void quiz_leave_quiz(user_t *user, int argc, char *argv[])
{
	if (!quiz -> quizmaster)
		command_output(quiz_messages[0]);
	else if (user -> userid == quiz -> quizmaster -> userid)
		command_output("You're the Quizmaster. To finish the quiz, use ^yquizstop^n.\n");
	else if (!quiz_contains_player(user) > 0)
		command_output("You're not playing the quiz.\n");
	else
	{
		command_output("You leave the quiz.\n");
		quiz_remove_player(user);
		quiz_tell_all("^y%s^n leaves the quiz.\n", user -> current_name);
	}
}

void quiz_handle_answer(user_t *user, char *str)
{
	if (strcmp(str, "") != 0)
	{
		quiz -> answer = strdup(str);
		user -> func = 0;
		do_prompt(user);
	}
	else
	{
		prompt_user(user, "Please enter the correct answer : ");
		return;
	}
}

void quiz_question(user_t *user, int argc, char *argv[])
{
	if (!quiz -> quizmaster)
		command_output(quiz_messages[0]);
	else if (user -> userid != quiz -> quizmaster -> userid)
		if (quiz -> question == NULL)
			command_output("No question has been asked for this round yet.\n");
		else
            user_output( CGAMES, user, NULL, "^yQuestion %d^n : %s\n", quiz -> round, quiz -> question);
	else
	{
		if (argc != 2)
			command_output("format: quiz question <question>\n");
		else if (quiz -> players == NULL)
			command_output ("There are no players yet!\n");
		else
		{
			if (quiz -> round != 0)
				quiz_do_scores(user);
			command_output ("You ask '^y%s^n'\n", argv[1]);
			quiz_ask_question(argv[1]);
			prompt_user(user, "^WPlease enter the correct answer^n : ");
			user -> func = quiz_handle_answer;
		}
	}
}

void quiz_answer(user_t *user, int argc, char *argv[])
{
	if (!quiz -> quizmaster)
		command_output(quiz_messages[0]);
	else if (user -> userid == quiz -> quizmaster -> userid)
	{
		if (argc != 2)
		{
			command_output("format: quiz answer <the correct answer>\n");
			command_output("(the players won't see it until the round is over.)\n");
		}
		else
		{
			quiz -> answer = strdup(argv[1]);
			command_output("You set the answer to '^y%s^n'\n", quiz -> answer);
		}
	}
	else
	{
		if (argc != 2)
			command_output("format: quiz answer <your answer>\n");
		else if (quiz -> question == NULL)
			command_output("No question has been asked for this round yet.\n");
		else
			quiz_make_answer(user, argv[1]);
	}
}

void quiz_tick(user_t *user, int argc, char *argv[])
{
	struct USERLIST *u;

	if (!quiz -> quizmaster)
		command_output(quiz_messages[0]);
	else if (user -> userid != quiz -> quizmaster -> userid)
		command_output("Only the Quizmaster may mark questions.\n");
	else
	{
		if (argc != 2)
			command_output("format: quiz tick <player>\n");
		else
		{
			if (!(u = quiz_find_player_by_name(user, argv[1])))
				command_output("format: quiz tick <player>\n");
			else if (u -> answer == NULL)
				command_output("That player hasn't submitted an answer yet.\n");
			else if (u -> tick != 0)
				command_output("That player has already been marked (but check out ^yquiz bonus^n.)^n\n");
			else
			{
				command_output("You mark a correct answer for %s\n", u -> u -> current_name);
				u -> tick = 1;
			}
		}
	}
}

void quiz_cross(user_t *user, int argc, char *argv[])
{
	struct USERLIST *u;

	if (!quiz -> quizmaster)
		command_output(quiz_messages[0]);
	else if (user -> userid != quiz -> quizmaster -> userid)
		command_output("Only the Quizmaster may mark questions.\n");
	else
	{
		if (argc != 2)
			command_output("format: quiz cross <player>\n");
		else
		{
			if (!(u = quiz_find_player_by_name(user, argv[1])))
				command_output("format: quiz cross <player>\n");
			else if (u -> answer == NULL)
				command_output("That player hasn't submitted an answer yet.\n");
			else
			{
				command_output("You mark a ^rwrong^n answer for %s\n", u -> u -> current_name);
				u -> tick = 0;
			}
		}
	}
}

void quiz_report(user_t *user, int argc, char *argv[])
{
	struct USERLIST *mover = quiz -> players;
	string_t *report;
	
	if (!(report = string_alloc("")))
	{
		add_log("error", "Out of memory: quiz_report in quiz.c");
		return;
	}
			
	if (!quiz -> quizmaster)
		command_output(quiz_messages[0]);
	else if (user -> userid == quiz -> quizmaster -> userid)
	{
		if (mover == NULL)
		{
			command_output("^yNo one has joined your quiz yet.^n\n");
		}
		else
		{
			string_append_title(user, report, LEFT, "Quiz answers");
			while (mover)
			{
				string_append(report, "^n * %s: '%s'\n", mover -> u -> current_name, (mover -> answer) ? mover -> answer : "(no answer)");
				mover = mover -> next;
			}
			string_append_line(user, report);
			command_output(report -> data);
		}
	}
	else
	{
		if (mover == NULL)
		{
			command_output("^yNo one has joined the quiz yet.^n\n");
		}
		else
		{
			string_append_title(user, report, LEFT, "Quiz Players");
			while (mover)
			{
				string_append(report, "^n * %-20.20s - %d point%s.\n", mover -> u -> current_name, mover -> score, (mover -> score != 1) ? "s" : "");
				mover = mover -> next;
			}
			string_append_line(user, report);
			command_output(report -> data);
		}
	}
	string_free(report);
}


void plugin_init( plugin_s *p_handle )
{
	struct QUIZ *dummy;

	if (!(dummy = ALLOC(1, struct QUIZ)))
	{
		add_log("error", "Out of memory: plugin_init in quiz.c");
		return;
	}

	dummy -> quizmaster = NULL;
	dummy -> players = NULL;
	dummy -> round = 0;
	dummy -> question = NULL;
	dummy -> answer = NULL;
	quiz = dummy;
	ghosts = NULL;

	add_command(	"quiz",			quiz_players,	 p_handle, CMD_GAMES, 1 );
	add_subcommand( "quiz", "who",		quiz_players,	 p_handle, CMD_GAMES, 0 );
	add_subcommand( "quiz", "tick", 	quiz_tick,	 p_handle, CMD_GAMES, 0 );
		add_subcommand( "quiz", "y", 	quiz_tick,	 p_handle, CMD_INVIS, 0 );
	add_subcommand( "quiz", "cross", 	quiz_cross,	 p_handle, CMD_GAMES, 0 );
		add_subcommand( "quiz", "n", 	quiz_cross,	 p_handle, CMD_INVIS, 0 );
	add_subcommand( "quiz", "stop", 	quiz_stop_quiz,	 p_handle, CMD_GAMES, 0 );
		add_subcommand( "quiz", "end", 	quiz_stop_quiz,	 p_handle, CMD_INVIS, 0 );
	add_subcommand( "quiz", "start", 	quiz_start_quiz, p_handle, CMD_GAMES, 0 );
	add_subcommand( "quiz", "report", 	quiz_report,	 p_handle, CMD_GAMES, 0 );
	add_subcommand( "quiz", "question",	quiz_question,	 p_handle, CMD_GAMES, 0 );
		add_subcommand( "quiz", "ask",	quiz_question,	 p_handle, CMD_INVIS, 0 );
	add_subcommand( "quiz", "leave", 	quiz_leave_quiz, p_handle, CMD_GAMES, 0 );
		add_subcommand( "quiz", "quit", quiz_leave_quiz, p_handle, CMD_INVIS, 0 );
	add_subcommand( "quiz", "join", 	quiz_join_quiz,	 p_handle, CMD_GAMES, 0 );
	add_subcommand( "quiz", "answer",	quiz_answer,	 p_handle, CMD_GAMES, 0 );
		add_subcommand( "quiz", "ans",	quiz_answer,	 p_handle, CMD_INVIS, 0 );

	/* add_command( "rules", 		quiz_rules, 	 p_handle, CMD_GAMES ,0 ); */
	return;
}

void plugin_fini( plugin_s *p_handle )
{
	FREE(quiz);
}

void PLUGIN_USER_LOGOUT(user_t *u)
{
	if (quiz -> quizmaster)
	{
		if (u -> userid == quiz -> quizmaster -> userid)
		{
			if (quiz -> round != 0)
				quiz_do_scores(u);
			quiz_tell_players("%s has disconnected: the quiz is over..\n", u -> current_name);
			quiz_end_quiz();
		}
		else
		{
			if (quiz -> players != NULL)
				quiz_remove_player(u);
		}
	}
}
     
void PLUGIN_USER_RECONNECT(user_t *old, user_t *new)
{
	if (quiz -> quizmaster)
	{
		if (old -> userid == quiz -> quizmaster -> userid)
		{
			quiz -> quizmaster = new;	
		}
		else
		{
			if (quiz -> players != NULL)
				quiz_replace_player(old, new);
		}
	}
}

