#include <glad/glad.h>

#include <stdio.h>
#include <queue>
#include <string>

static std::queue<std::string> s_errors;

void clearGLErrorsInternal()
{
    while(glGetError() != GL_NO_ERROR);
}

static void printErrorInternal(unsigned error)
{
    switch(error)
    {
    case GL_INVALID_ENUM:
        s_errors.push("GLERROR::INVALID ENUM\n");
        break;
    case GL_INVALID_VALUE:
        s_errors.push("GLERROR::INVALID VALUE\n");
        break;
    case GL_INVALID_OPERATION:
        s_errors.push("GLERROR::INVALID OPERATION\n");
        break;
    case GL_STACK_OVERFLOW:
        s_errors.push("GLERROR::STACK OVERFLOW\n");
        break;
    case GL_STACK_UNDERFLOW:
        s_errors.push("GLERROR::STACK UNDERFLOW\n");
        break;
    case GL_OUT_OF_MEMORY:
        printf("GLERROR::OUT OF MEMORY\n");
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        s_errors.push("GLERROR::INVALID FRAMEBUFFER OPERATION\n");
        break;
    default:
        s_errors.push("GLERROR::UNKNOWN ERROR\n");
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

void logError(const char* error, const char* message)
{
    s_errors.push("ERROR::" + std::string(error) + " " + std::string(message) + "\n");
}

namespace dgn
{
    const char *getErrorString()
    {
        if(s_errors.size() < 1) return nullptr;

        const char *res = s_errors.front().c_str();
        s_errors.pop();
        return res;
    }
}

