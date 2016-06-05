#include <stdlib.h>
#include <stdio.h>
#include "log.h"

#include <iostream>
#include <stdarg.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>

bool LogCreated = false;

void Log(const char file[], int line, const char* format, ...) {

	static char tmp_string[4096];
	static char message[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, format, ap);
	va_end(ap);
	sprintf_s(message, "\n%s(%d) : %s", file, line, tmp_string);


#if defined(OPTION_OUTPUT)
#if OPTION_OUTPUT==IOSTREAM_OUTPUT
	//using simple console, redirect output
	if (start == false) {
		start = true;
		RedirectIOToConsole();
	}

#else
	//using visual studio
	OutputDebugString(message);
#endif
#endif

	//using simple console with windows or unix
#if (defined(OPTION_OUTPUT) && OPTION_OUTPUT==IOSTREAM_OUTPUT) || !defined(OPTION_OUTPUT)
	//improve, add an option to indicate the level of log (error, warning, info...)
	//std::cerr << tmp_string2;
	std::clog << message;
#endif


#if defined(FILE_OUTPUT)
	#if FILE_OUTPUT==true
	FILE *fileLog;

	if (!LogCreated) {
		fopen_s(&fileLog, LOGFILE, "w");
		LogCreated = true;
	} else
		fopen_s(&fileLog, LOGFILE, "a");

	if (file == NULL) {
		if (LogCreated)
			LogCreated = false;
		return;
	} else {
		fputs(message, fileLog);

		fclose(fileLog);
	}

	if (file)
		fclose(fileLog);
	#endif
#endif
}

#if defined(OPTION_OUTPUT) && OPTION_OUTPUT==IOSTREAM_OUTPUT



#pragma warning(push)
#pragma warning(disable:4302)
#pragma warning(disable:4311)
#pragma warning(disable:4068)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"

void RedirectIOToConsole() {
	int hConHandle;
	long lStdHandle;
	FILE *fp;

	// allocate a console for this app
	AllocConsole();

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long) GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);


	// redirect unbuffered STDERR to the console
	lStdHandle = (long) GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "w");
	*stderr = *fp;
	setvbuf(stderr, NULL, _IONBF, 0);
}
#pragma GCC diagnostic pop
#pragma warning(pop)
#endif