#include <glad/glad.h>

#include <stdio.h>

void clearGLErrorsInternal()
{
    while(glGetError() != GL_NO_ERROR);
}

static void printErrorInternal(unsigned error)
{
    switch(error)
    {
    case GL_INVALID_ENUM:
        printf("GLERROR::INVALID ENUM\n");
        break;
    case GL_INVALID_VALUE:
        printf("GLERROR::INVALID VALUE\n");
        break;
    case GL_INVALID_OPERATION:
        printf("GLERROR::INVALID OPERATION\n");
        break;
    case GL_STACK_OVERFLOW:
        printf("GLERROR::STACK OVERFLOW\n");
        break;
    case GL_STACK_UNDERFLOW:
        printf("GLERROR::STACK UNDERFLOW\n");
        break;
    case GL_OUT_OF_MEMORY:
        printf("GLERROR::OUT OF MEMORY\n");
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        printf("GLERROR::INVALID FRAMEBUFFER OPERATION\n");
        break;
    default:
        printf("GLERROR::UNKNOWN ERROR\n");
    }
}

bool checkGLErrorsInternal()
{
    uint16_t error;
    bool res = true;

    while((error = glGetError()) != GL_NO_ERROR)
    {
        res = false;
        printErrorInternal(error);
    }

    return res;
}

void printDebugDataInternal(const char* file, unsigned line)
{
    printf("File %s, Line %d\n", file, line);
}

void logErrorInternal(const char* error, const char* message, const char* file, unsigned line)
{
    printf("ERROR::%s\n\tFile %s, line %u\n\t%s\n", error, file, line, message);
}

