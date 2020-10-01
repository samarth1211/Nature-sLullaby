#include "LoadShaders.h"

#include <cstdlib>
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

FILE *gpLogFile1;

static const GLchar *ReadShaders(const char *Filename)
{
	FILE *InputFile;
	fopen_s(&InputFile, Filename, "rb");

	if (!InputFile)
	{
		fprintf_s(gpLogFile1, "Unable to open file %s\n", Filename);
	}

	fseek(InputFile, 0, SEEK_END);
	int iLength = ftell(InputFile);
	fseek(InputFile, 0, SEEK_SET);

	GLchar *Source = new GLchar[iLength + 1];

	fread(Source, 1, iLength, InputFile);
	fclose(InputFile);

	Source[iLength] = 0;

	return(const_cast<const GLchar *>(Source));
}

GLuint LoadShaders(ShaderInfo *ShaderData)
{
	fopen_s(&gpLogFile1, "ModelLogFile.txt", "a+");
	if (!gpLogFile1)
	{
		exit(EXIT_FAILURE);
	}
	else
	{
		fprintf_s(gpLogFile1, "ModelLogFile.txt opened.\n");
	}


	if (ShaderData == NULL)
	{
		fprintf_s(gpLogFile1, "Shader Data is null\n");
		fclose(gpLogFile1);
		return(0);
	}
	
	GLuint Program = glCreateProgram();

	ShaderInfo *Entry = ShaderData;

	while (Entry->Type != GL_NONE)
	{
		GLuint Shader = glCreateShader(Entry->Type);

		Entry->Shader = Shader;

		const GLchar *Source = ReadShaders(Entry->Filename);
		if (Source == NULL)
		{
			for (Entry = ShaderData; Entry->Type != GL_NONE; ++Entry)
			{
				glDeleteShader(Entry->Shader);
				Entry->Shader = 0;
			}
			fclose(gpLogFile1);
			return(0);
		}

		glShaderSource(Shader, 1, &Source, NULL);
		delete[] Source;

		glCompileShader(Shader);

		GLint iCompiled;
		glGetShaderiv(Shader, GL_COMPILE_STATUS, &iCompiled);

		if (!iCompiled)
		{
			GLsizei iLength;
			glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &iLength);

			GLchar *Log = new GLchar[iLength + 1];
			glGetShaderInfoLog(Shader, iLength, &iLength, Log);
			std::string str(Log);
			fprintf_s(gpLogFile1, "Shader compilation failed:\n %s in file %s\n", str.c_str(), Entry->Filename);
			delete[] Log;
			fclose(gpLogFile1);
			return(0);
		}
		glAttachShader(Program, Shader);
		++Entry;
	}

	glLinkProgram(Program);

	GLint iLinked;

	glGetProgramiv(Program, GL_LINK_STATUS, &iLinked);
	if (!iLinked)
	{
		GLsizei iLength;
		glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &iLength);

		GLchar *Log = new GLchar[iLength + 1];
		glGetProgramInfoLog(Program, iLength, &iLength, Log);
		fprintf_s(gpLogFile1, "Shader linking failed: %s\n", Log);
		delete[] Log;

		for (Entry = ShaderData; Entry->Type != GL_NONE; ++Entry)
		{
			glDeleteShader(Entry->Shader);
			Entry->Shader = 0;
		}
		fclose(gpLogFile1);
		return(0);
	}
	
	fprintf_s(gpLogFile1, "ShaderLog closed.\n");
	fclose(gpLogFile1);
	return(Program);
}

#ifdef __cplusplus
};
#endif //__cplusplus
