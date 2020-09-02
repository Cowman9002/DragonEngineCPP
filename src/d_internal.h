#pragma once

#ifdef __DEBUG__
#include <stdio.h>

void clearGLErrorsInternal();
bool checkGLErrorsInternal();

void printDebugDataInternal(const char* file, unsigned line);
void logErrorInternal(const char* error, const char* message, const char* file, unsigned line);

#define glCall(func) clearGLErrorsInternal(); func; if(!checkGLErrorsInternal()) printDebugDataInternal(__FILE__, __LINE__)
#define logError(error, message) logErrorInternal(error, message, __FILE__, __LINE__)
#define logMessagef(str, ...) printf(str, __VA_ARGS__)
#define logMessage(str) printf(str)

#else
#define glCall(func) func
#define logError(error, message)
#endif // __DEBUG__
