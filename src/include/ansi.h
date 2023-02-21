/*
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *       
 *
 *      Prototypes should only be placed in here if the function should
 *      only be accessible to the core, and not the plugins, otherwise it
 *      should be placed in plugin_externs.h
 *
 */      

#define OFF         "\033[0m"      /* nN */
#define BOLD        "\033[1m"      /* L */
#define FAINT       "\033[2m"      /* l */
#define UNDERLINE   "\033[4m"      /* uU */
#define BLINK       "\033[5m"      /* kK */
#define REVERSE     "\033[7m"      /* hH */

#define FG_BLACK_F  "\033[0;30m"   /* d */
#define FG_RED_F    "\033[0;31m"   /* r */
#define FG_GREEN_F  "\033[0;32m"   /* g */
#define FG_YELLOW_F "\033[0;33m"   /* y */
#define FG_BLUE_F   "\033[0;34m"   /* b */
#define FG_PURPLE_F "\033[0;35m"   /* p */
#define FG_CYAN_F   "\033[0;36m"   /* c */
#define FG_GREY     "\033[0;37m"   /* w */

#define FG_BLACK_B  "\033[1;30m"   /* D */
#define FG_RED_B    "\033[1;31m"   /* R */
#define FG_GREEN_B  "\033[1;32m"   /* G */
#define FG_YELLOW_B "\033[1;33m"   /* Y */
#define FG_BLUE_B   "\033[1;34m"   /* B */
#define FG_PURPLE_B "\033[1;35m"   /* P */
#define FG_CYAN_B   "\033[1;36m"   /* C */
#define FG_WHITE    "\033[1;37m"   /* W */

#define BG_BLACK    "\033[0;40m"   /* sS */
#define BG_RED      "\033[0;41m"   /* eE */
#define BG_GREEN    "\033[0;42m"   /* fF */
#define BG_YELLOW   "\033[0;43m"   /* tT */
#define BG_BLUE     "\033[0;44m"   /* vV */
#define BG_PURPLE   "\033[0;45m"   /* oO */
#define BG_CYAN     "\033[0;46m"   /* xX */
#define BG_GREY     "\033[0;47m"   /* qQ */
