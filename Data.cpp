#include "Data.h"

CCommon::CCommon()
{
}

CCommon::~CCommon()
{

}

void CCommon::_check_gl_error(FILE *pFile, const char *filename, const char *currentfile, int currentline)
{

	fopen_s(&pFile, filename, "a+");

	GLenum err(glGetError());

	while (err != GL_NO_ERROR)
	{
		std::string error;

		switch (err)
		{
		case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}

		//cerr << "GL_" << error.c_str() << " - " << file << ":" << line << endl;
		fprintf_s(pFile, "GL_%s File:%s Line:%d\n", error.c_str(), currentfile, currentline);
		err = glGetError();
	}

	fclose(pFile);
}