#pragma once
#ifndef LOG_H
#define LOG_H
// all Log(); messages will be appended to this file
#define LOGFILE	"debug.log"

#define LOG(format, ...) Log(__FILE__, __LINE__, format, __VA_ARGS__);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <windows.h>
	#include <Windows.h>
	#include <io.h>

	#define IOSTREAM_OUTPUT 1
	#define VISUAL_STUDIO_OUTPUT 2
	
	#define FILE_OUTPUT true
	#define OPTION_OUTPUT VISUAL_STUDIO_OUTPUT
#endif

// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;
static bool start = false;

#if defined(OPTION_OUTPUT) && OPTION_OUTPUT==IOSTREAM_OUTPUT
void RedirectIOToConsole();
#endif

// keeps track whether the log file is created or not
extern bool LogCreated;

// logs a message to LOGFILE
void Log(const char file[], int line, const char* format, ...);

#endif