#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stddef.h> // for offsetof
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

#include <glm\glm.hpp>
#include <glm\geometric.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\string_cast.hpp>
#include <glm\gtc\quaternion.hpp>

#include<FreeImage.h>

#include"resource.h"
#include"Camera.h"
#include"LoadTGATexture.h"
#include"LoadShaderFromFile.h"


#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
#define DELTA 0.66666666666667f

#define MSAA_SAMPLES	4
#define FRUSTUM_NEAR	0.1f
#define FRUSTUM_FAR		100.0f

enum InitErrorCodes
{
	INIT_AUDIO_LOAD_FAILED = -18,
	INIT_AUDIO_MANAGER_INIT_FAILED = -17,
	INIT_COMPUTE_SHADER_COMPILATION_FAILED = -16,
	INIT_MODEL_LOADING_FAILED = -15,
	INIT_FRAMEBUFFER_FAILED = -14,
	INIT_RESOURCE_LOAD_FAILED = -13,
	INIT_GEOMETRY_SHADER_COMPILATION_FAILED = -12,
	INIT_TESSILATION_EVALUATION_SHADER_COMPILATION_FAILED = -11,
	INIT_TESSILATION_CONTROL_SHADER_COMPILATION_FAILED = -10,
	INIT_VERTEX_SHADER_COMPILATION_FAILED = -9,
	INIT_FRAGMENT_SHADER_COMPILATION_FAILED,
	INIT_LINK_SHADER_PROGRAM_FAILED,
	INIT_FAIL_GLEW_INIT,
	INIT_FAIL_BRIDGE_CONTEX_SET,
	INIT_FAIL_BRIDGE_CONTEX_CREATION,
	INIT_FAIL_SET_PIXEL_FORMAT,
	INIT_FAIL_NO_PIXEL_FORMAT,
	INIT_FAIL_NO_HDC,
	INIT_ALL_OK,
};

enum attributeBindLocations
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TANGENT,
	AMC_ATTRIBUTE_BITANGENT,
	AMC_ATTRIBUTE_TEXTURE0,


	AMC_ATTRIBUTE_ID,
	AMC_ATTRIBUTE_WEIGHTS,

	AMC_VELOCITY_ARRAY,
	AMC_START_TIME_ARRAY,
};


