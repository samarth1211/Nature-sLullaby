#ifndef __LOAD_SHADERS_H__
#define __LOAD_SHADERS_H__

#include <GL\glew.h>
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus



typedef struct 
{
	GLenum Type;
	const char *Filename;
	GLuint Shader;
}ShaderInfo;

GLuint LoadShaders(ShaderInfo *ShaderData);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //__LOAD_SHADERS_H__
