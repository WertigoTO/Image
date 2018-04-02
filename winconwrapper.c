#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>

#define BUFFER_SIZE 256

bool bconwrapinit = false;
HANDLE outputhndl;
char buffer[BUFFER_SIZE];

//NORMAL
/*void wcprintf(const char* fmt, ...)
{	
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}*/

bool conwrapinit()
{
	if(!AllocConsole())
	{
		return false;
	}
	
	outputhndl = GetStdHandle(STD_OUTPUT_HANDLE);
	if(!outputhndl)
	{
		return false;
	}

	bconwrapinit = true;
	return true;
}

void wcprintf(const char* fmt, ...)
{
	if(!bconwrapinit)
	{
		if(!conwrapinit())
		{
			return;
		}
	}

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, BUFFER_SIZE, fmt, args);
	va_end(args);

	
	DWORD written;
	WriteConsole(outputhndl, buffer, strlen(buffer), &written, NULL);
}

