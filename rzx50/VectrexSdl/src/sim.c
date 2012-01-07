
#define _CRT_SECURE_NO_WARNINGS  // For VC warning C4996: 'sprintf': This function or variable may be unsafe.

#include <stdlib.h>
#include <string.h>

/* file extension name */
int GetFileType(char* pname) {
	if(pname != NULL)
		strcpy(pname, "VECX"); // Emulator ROM extensions (use "EXT|EXT|EXT" for several file-type associations, not more than five)
	return 0;
}

/* to get default path */
int GetDefaultPath(char* path) {
	if(path != NULL)
		strcpy(path, "A:\\GAME\\VECX");
	return 0;
}

/* module description, optional */
int GetModuleName(char* name, int code_page) {
	if((name != NULL) && (code_page == 0)) // ansi
		strcpy(name, "VectrexSdl.sim");  //your emulator file name
	return 0;
}
