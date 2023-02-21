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
 */

#ifndef __PLUGIN_HEADERS_H
        #define __PLUGIN_HEADERS_H

#include <stdarg.h>       
#include <string.h>
#include <stdlib.h>          

#define ALLOC( num, type ) ( type* ) calloc( num, sizeof( type ) )
#define FREE( x )  free( x ); x = NULL

#include "str.h"
#include "plugins.h"

#include "settings.h"
#include "structures.h"
#include "globals.h"

#include "recovery.h"

#include "binfiles.h"
#include "plugin_externs.h"

#endif

