/* Raw - Another World Interpreter
 * Copyright (C) 2004 Gregory Montoir
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdarg.h>
#include "util.h"


uint16 g_debugMask;

#ifndef USE_DINGOO

void debug(uint16 cm, const char *msg, ...) {
	char buf[1024];
	if (cm & g_debugMask) {
		va_list va;
		va_start(va, msg);
		vsprintf(buf, msg, va);
		va_end(va);
		printf("%s\n", buf);
		fflush(stdout);
	}
}

void error(const char *msg, ...) {
	char buf[1024];
	va_list va;
	va_start(va, msg);
	vsprintf(buf, msg, va);
	va_end(va);
	FILE * logfile = fopen("newraw.log", "a");
	fprintf(logfile, "ERROR: %s\n", buf);
	exit(-1);
}

void warning(const char *msg, ...) {
	char buf[1024];
	va_list va;
	va_start(va, msg);
	vsprintf(buf, msg, va);
	va_end(va);
	fprintf(stderr, "WARNING: %s!\n", buf);
}
#else//USE_DINGOO
static FSYS_FILE* log_file = NULL;
extern "C" int fsys_flush_cache(FSYS_FILE* file);
void write_log(const char* msg)
{
//	if(!log_file)
//		log_file = fsys_fopen("a:\\game\\aw\\aw.log", "w");
//	fsys_fwrite(msg, 1, strlen(msg), log_file);
//	fsys_fwrite("\n", 1, 1, log_file);
//	fsys_flush_cache(log_file);
}
void debug(uint16 cm, const char *msg, ...) { if (cm & g_debugMask) write_log(msg); }
void error(const char *msg, ...) { write_log(msg); }
void warning(const char *msg, ...) { write_log(msg); }
#endif//USE_DINGOO

void string_lower(char *p) {
	for (; *p; ++p) {
		if (*p >= 'A' && *p <= 'Z') {
			*p += 'a' - 'A';
		}
	}
}

void string_upper(char *p) {
	for (; *p; ++p) {
		if (*p >= 'a' && *p <= 'z') {
			*p += 'A' - 'a';
		}
	}
}
