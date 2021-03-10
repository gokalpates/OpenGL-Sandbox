#include "Debug.h"

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

void GLCheckError()
{
	while (GLenum err = glGetError())
	{
		std::cout << std::hex <<"[OpenGL Error] (" << err << ")" << std::endl;
	}
}
