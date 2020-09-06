#pragma once

#include <stdio.h>

void clearGLErrorsInternal();
bool checkGLErrorsInternal();

void logError(const char* error, const char* message);

#define glCall(func) clearGLErrorsInternal(); func; checkGLErrorsInternal()

