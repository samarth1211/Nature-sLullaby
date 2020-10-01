#ifndef __LOAD_SHADER_FROM_FILE_H__
#define __LOAD_SHADER_FROM_FILE_H__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <map>

#include<GL\glew.h>
#include<gl\GL.h>


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

	char* LoadShaderSourceFromFile(const char *filename, const char *preamble, size_t *iSize);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif __LOAD_SHADER_FROM_FILE_H__

