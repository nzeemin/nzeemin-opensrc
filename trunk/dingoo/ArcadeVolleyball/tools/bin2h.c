// Copyright (c) 2009 Harteex Productions
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define VERSION "1.0"

uint8_t*     buffer;
char*        fileName = NULL;
unsigned int fileSize;
char         fileNameNoExtension[256];
char         newFileName[256];
char         variableName[256];
char*        newline = "\r\n";
bool         includeSize = true;

bool loadFile()
{
	if (fileName == NULL)
	{
		printf("Filename missing\n");
		return false;
	}

	FILE* pFile = fopen(fileName, "rb");
	if (pFile == NULL)
	{
		printf("Unable to open %s\n", fileName);
		return false;
	}

	// Get file size
	fseek(pFile, 0, SEEK_END);
	fileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	// Allocate memory
	buffer = (uint8_t *)malloc(sizeof(uint8_t) * fileSize);
	if (buffer == NULL)
	{
		printf("Unable to allocate %u bytes\n", fileSize);
		return false;
	}

	// Copy file into buffer
	size_t result = fread(buffer, 1, fileSize, pFile);
	if (result != fileSize)
	{
		printf("Reading error\n");
		free(buffer);
		return false;
	}

	fclose(pFile);
	return true;
}

bool output()
{
	if (newFileName == NULL)
	{
		printf("Filename missing\n");
		return false;
	}

	FILE* pFile = fopen(newFileName, "wb");
	if (pFile == NULL)
	{
		printf("Unable to open %s for writing\n", newFileName);
		return NULL;
	}

	if (includeSize)
	{
		fprintf(pFile, "unsigned int %sSize = %u;%s", variableName, fileSize, newline);
	}

	fprintf(pFile, "unsigned char %s[] = {", variableName);
	int i;
	for (i = 0; i < fileSize; i++)
	{
		if (i % 0x10 == 0)
		{
			fprintf(pFile, "%s", newline);
		}
		
		fprintf(pFile, "0x%02x", buffer[i]);
		
		if (i < fileSize - 1)
		{
			fprintf(pFile, ",");
		}
	}
	fprintf(pFile, "%s};%s", newline, newline);

	fclose(pFile);
	printf("Done!\nVariable %s saved to %s\n", variableName, newFileName);
	return true;
}

void printUsage(const char* appName)
{
	printf("bin2h v%s\n\n", VERSION);
	printf("Usage: %s [/O name] [/S] [/U] [/V name] <filename>\n", appName);
	printf("  /O name	Name of the output file\n");
	printf("  /S		Don't include size variable\n");
	printf("  /U		Use Unix-style line endings\n");
	printf("  /V name	Name of the variable\n");
	printf("\nCopyright 2009 Harteex Productions\n");
}

bool handleArguments(int argc, char** argv)
{
	char curflag = 0;
	int i;
	int skip;
	for (i = 1, skip = 0; i < argc; i++)
	{
		if (skip == 0) // Expecting a new flag or files
		{
			if (argv[i][0] == '/') // A flag
			{
				curflag = toupper(argv[i][1]);
				switch (curflag)
				{
					case 'O':
						skip = 1;
						break;
					case 'S':
						includeSize = false;
						break;
					case 'U':
						newline = "\n";
						break;
					case 'V':
						skip = 1;
						break;
					default:
						printf("Invalid flag: %s\n", argv[i]);
						return false;
				}
			}
			else
			{
				if (fileName != NULL)
				{
					printf("Error: Input file is already specified as %s\n", fileName);
					return false;
				}
				fileName = argv[i];
			}
		}
		else
		{
			if (argv[i][0] == '/') // A flag
			{
				printf("Error: Expected argument for flag %c, found new flag.\n", curflag);
				return false;
			}

			if (strlen(argv[i]) >= 255)
			{
				printf("Argument too long!\n");
				return false;
			}

			switch(curflag)
			{
				case 'O':
					sprintf(newFileName, "%s", argv[i]);
					skip = 0;
					break;
				case 'V':
					sprintf(variableName, "%s", argv[i]);
					skip = 0;
					break;
			}
		}
	}

	if (skip > 0)
	{
		printf("Error: Expected argument for flag %c\n", curflag);
		return false;
	}

	if (fileName == NULL)
	{
		printf("Error: No input file specified.\n");
		return false;
	}

	return true;
}

int main(int argc, char** argv)
{
	if (argc <= 1 || (argc > 1 && strcmp(argv[1], "/?") == 0))
	{
		if (argc == 0)
			printUsage("bin2h");
		else
			printUsage(argv[0]);

		return 1;
	}
	
	newFileName[0] = '\0';
	variableName[0] = '\0';

	// Handle arguments
	if (!handleArguments(argc, argv))
	{
		return 1;
	}

	sprintf(fileNameNoExtension, fileName);
	char* lastDot = strrchr(fileNameNoExtension, '.');
	if (lastDot != NULL)
	{
		*lastDot = '\0';
	}

	if (strlen(variableName) == 0)
		sprintf(variableName, "%s", fileNameNoExtension);

	if (strlen(newFileName) == 0)
		sprintf(newFileName, "%s.h", variableName);

	// Load file and output result
	if (!loadFile())
	{
		return 2;
	}

	bool status = output();
	
	free(buffer);

	if (!status)
	{
		return 3;
	}

	return 0;
}
