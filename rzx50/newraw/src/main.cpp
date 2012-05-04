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

#include "engine.h"
#include "systemstub.h"
#include "util.h"

int main(int argc, char**argv)
{
	char app_path[256];
	char dataPath[256];
	char savePath[256];
	app_path[0] = '\0';
#ifdef USE_DINGOO
	strcpy(app_path, argv[0]);
	char* p = app_path + strlen(app_path);
	while(p > app_path && *p != '/' && *p != '\\')
		--p;
	if(p != app_path)
	{
		++p;
		*p = '\0';
	}
	strcpy(dataPath, app_path);
#else
	strcpy(dataPath, app_path);
	strcat(dataPath, "res");
#endif//USE_DINGOO
	strcpy(savePath, app_path);
	strcat(savePath, "save");
	g_debugMask = DBG_INFO;// | DBG_LOGIC | DBG_BANK | DBG_VIDEO | DBG_SER | DBG_SND;
#if defined(USE_DINGOO)
	SystemStub *stub = SystemStub_Dingoo_create();
#elif defined(USE_SDL)
	SystemStub *stub = SystemStub_SDL_create();
#endif//USE_DINGOO | USE_SDL
	Engine *e = new Engine(stub, dataPath, savePath);
	e->run();
	delete e;
	delete stub;
	return 0;
}
