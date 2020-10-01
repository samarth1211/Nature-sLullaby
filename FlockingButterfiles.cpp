#include "FlockingButterfiles.h"




CFlockingButterflies::CFlockingButterflies()
{
	g_ShaderProgramObject_Particle = 0;
	g_ComputeProgramObject_Particle = 0;

	flock_buffer[2];
	flock_render_vao[2];
	geometry_buffer;

	// Uniforms
	g_Uniform_Model_Matrix = 0;
	g_Uniform_View_Matrix = 0;
	g_Uniform_Projection_Matrix = 0;

	g_uniform_NumberofRows = 0;
	g_uniform_Offset1 = 0;
	g_uniform_Offset2 = 0;
	g_uniform_blendFactor = 0;

	g_Uniform_CS_closest_allowed_dist = 0;
	g_Uniform_CS_rule1_weight = 0;
	g_Uniform_CS_rule2_weight = 0;
	g_Uniform_CS_rule3_weight = 0;
	g_Uniform_CS_rule4_weight = 0;
	g_Uniform_CS_goal = 0;
	g_Uniform_CS_timeStep = 0;

	swap_index = 0;
	// sampler
	g_uniform_ButterFlySampler = 0;
	g_TextureButterFly = 0;


	if (fopen_s(&buffterFliesLogFIle, "ButterFliesLogFile.txt", "w+"))
	{
		MessageBox(NULL, TEXT("Failed to ccreate log file"), TEXT("Error...!!"), MB_OK);
	}
	else
	{
		fprintf_s(buffterFliesLogFIle, "Log File Created\n");
	}

}

CFlockingButterflies::~CFlockingButterflies()
{

	if (g_TextureButterFly)
	{
		glDeleteTextures(1, &g_TextureButterFly);
		g_TextureButterFly = 0;
	}

	if (geometry_buffer)
	{
		glDeleteBuffers(1, &geometry_buffer);
		geometry_buffer = 0;
	}

	if (flock_buffer[0])
	{
		glDeleteBuffers(2, flock_buffer);
	}

	if (flock_render_vao[0])
	{
		glDeleteVertexArrays(2, flock_render_vao);
	}

	glUseProgram(0);

	if (g_ComputeProgramObject_Particle)
	{
		GLsizei iShaderCount;
		GLsizei iShaderNumber;

		glUseProgram(g_ComputeProgramObject_Particle);
		glGetProgramiv(g_ComputeProgramObject_Particle, GL_ATTACHED_SHADERS, &iShaderCount);
		GLuint *pShaders = (GLuint*)calloc(iShaderCount, sizeof(GLuint));

		if (pShaders)
		{
			glGetAttachedShaders(g_ComputeProgramObject_Particle, iShaderCount, &iShaderCount, pShaders);
			for (iShaderNumber = 0; iShaderNumber < iShaderCount; iShaderNumber++)
			{
				glDetachShader(g_ComputeProgramObject_Particle, pShaders[iShaderNumber]);
				glDeleteShader(pShaders[iShaderNumber]);
				pShaders[iShaderNumber] = 0;
			}
			free(pShaders);
			pShaders = NULL;
		}
		glUseProgram(0);
		glDeleteProgram(g_ComputeProgramObject_Particle);
		g_ComputeProgramObject_Particle = NULL;
	}

	if (g_ShaderProgramObject_Particle)
	{
		GLsizei iShaderCount;
		GLsizei iShaderNumber;

		glUseProgram(g_ShaderProgramObject_Particle);
		glGetProgramiv(g_ShaderProgramObject_Particle, GL_ATTACHED_SHADERS, &iShaderCount);
		GLuint *pShaders = (GLuint*)calloc(iShaderCount, sizeof(GLuint));

		if (pShaders)
		{
			glGetAttachedShaders(g_ShaderProgramObject_Particle, iShaderCount, &iShaderCount, pShaders);
			for (iShaderNumber = 0; iShaderNumber < iShaderCount; iShaderNumber++)
			{
				glDetachShader(g_ShaderProgramObject_Particle, pShaders[iShaderNumber]);
				glDeleteShader(pShaders[iShaderNumber]);
				pShaders[iShaderNumber] = 0;
			}
			free(pShaders);
			pShaders = NULL;
		}
		glUseProgram(0);
		glDeleteProgram(g_ShaderProgramObject_Particle);
		g_ShaderProgramObject_Particle = NULL;
	}

	if (buffterFliesLogFIle)
	{
		fprintf_s(buffterFliesLogFIle, "\n Closing the ButterFliesLogFile file. \n");
		
		fclose(buffterFliesLogFIle);
		buffterFliesLogFIle = NULL;
	}

}

glm::vec3 CFlockingButterflies::randomValues()
{
	glm::vec3 result;
	int i;

	for (i = 0; i < 3; i++)
	{
		result[i] = random();
	}
	return result;
}

float CFlockingButterflies::random()
{
	static unsigned int seed = 0x13371337;
	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}

int CFlockingButterflies::LoadPNGTexture(GLuint *texture, const char *filename, int *imgRows, int *imgCols)
{
	GLfloat Anisotropy = 4.0f;

	ILuint ImageName;
	ilGenImages(1, &ImageName);
	ilBindImage(ImageName);
	if (!ilLoadImage((ILstring)filename))
	{
		return -1;
	}

	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Anisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Anisotropy);

	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	*imgRows = ilGetInteger(IL_IMAGE_HEIGHT);
	*imgCols = ilGetInteger(IL_IMAGE_WIDTH);

	ilDeleteImages(1, &ImageName);

	glBindTexture(GL_TEXTURE_2D, 0);

	return 0;
}


int CFlockingButterflies::InitializeFlockingButterflies()
{

	// Shader Programs
	GLuint iVertexShaderObject = 0;
	GLuint iFragmentShaderObject = 0;

	GLuint iCoumuteShaderObject = 0;

	/// Sam : all Shader Code Start
	/*Compute Shader Program*/
	iCoumuteShaderObject = glCreateShader(GL_COMPUTE_SHADER);
	const GLchar *coupteSource =
		"#version 430 core"	\
		"\n"	\
		"layout (local_size_x=256)in;\n"	\
		"uniform float closest_allowed_dist = 50.0;\n"	\
		"uniform float rule1_weight = 0.18;\n"	\
		"uniform float rule2_weight = 0.05;\n"	\
		"uniform float rule3_weight = 0.17;\n"	\
		"uniform float rule4_weight = 0.02;\n"	\
		"uniform vec3 goal = vec3(0.0);\n"	\
		"uniform float timestep = 0.4;\n"	\
		"struct flock_member"	\
		"{"	\
		"	vec3 position;"	\
		"	vec3 velocity;"	\
		"};\n"	\
		"layout(std430,binding=0) readonly buffer members_in"	\
		"{"	\
		"	flock_member member[];"	\
		"}input_data;\n"	\
		"layout (std430, binding = 1) buffer members_out"	\
		"{"	\
		"	flock_member member[];"	\
		"}output_data;"	\
		"shared flock_member shared_member[gl_WorkGroupSize.x];"	\
		"vec3 rule1(vec3 my_position, vec3 my_velocity, vec3 their_position, vec3 their_velocity)"	\
		"{"	\
		"	vec3 d = my_position - their_position;\n"	\
		"	if (dot(d, d) < closest_allowed_dist)"	\
		"	{"	\
		"		return d;\n"	\
		"	}"	\
		"	return vec3(0.0);\n"	\
		"}\n"	\
		"vec3 rule2(vec3 my_position, vec3 my_velocity, vec3 their_position, vec3 their_velocity)"	\
		"{"	\
		"	vec3 d = their_position - my_position;"	\
		"	vec3 dv = their_velocity - my_velocity;"	\
		"	return dv / (dot(d, d) + 10.0);"	\
		"}\n"	\
		"void main ()"	\
		"{"	\
		"	uint i, j;\n"	\
		"	int global_id = int(gl_GlobalInvocationID.x);\n"	\
		"	int local_id  = int(gl_LocalInvocationID.x);\n"	\
		"	flock_member me = input_data.member[global_id];\n"	\
		"	flock_member new_me;\n"	\
		"	vec3 accelleration = vec3(0.0);"	\
		"	vec3 flock_center = vec3(0.0);"	\
		"	for (i = 0; i < gl_NumWorkGroups.x; i++)"	\
		"	{\n"	\
		"		flock_member them=input_data.member[i*gl_WorkGroupSize.x+local_id];\n"	\
		"		shared_member[local_id] = them;\n"	\
		"		memoryBarrierShared();"	\
		"		barrier();"	\
		"		for (j = 0; j < gl_WorkGroupSize.x; j++)\n"	\
		"		{"	\
		"			them = shared_member[j];\n"	\
		"			flock_center += them.position;\n"	\
		"			if(i*gl_WorkGroupSize.x+j!=global_id)\n"	\
		"			{"	\
		"				accelleration += rule1(me.position,me.velocity,them.position,them.velocity) * rule1_weight;\n"	\
		"				accelleration += rule2(me.position,me.velocity,them.position,them.velocity) * rule2_weight;\n"	\
		"			}"	\
		"		}"	\
		"		barrier();\n"	\
		"	}\n"	\
		"	flock_center /= float(gl_NumWorkGroups.x * gl_WorkGroupSize.x);\n"	\
		"	new_me.position = me.position + me.velocity * timestep;\n"	\
		"	accelleration+=normalize(goal-me.position)*rule3_weight;\n"	\
		"	accelleration+=normalize(flock_center-me.position)*rule4_weight;\n"	\
		"	new_me.velocity=me.velocity+accelleration*timestep;\n"	\
		"	if (length(new_me.velocity) > 10.0)"	\
		"	{"	\
		"		new_me.velocity=normalize(new_me.velocity)*10.0;"	\
		"	}"	\
		"	new_me.velocity=mix(me.velocity,new_me.velocity,0.4);"	\
		"	output_data.member[global_id] = new_me;\n"	\
		"}";

	glShaderSource(iCoumuteShaderObject, 1, (const GLchar**)&coupteSource, NULL);
	glCompileShader(iCoumuteShaderObject);
	GLint iInfoLogLength = 0;
	GLint iCompileStatus = 0;
	char *chErrorMessage = NULL;
	glGetShaderiv(iCoumuteShaderObject, GL_COMPILE_STATUS, &iCompileStatus);
	if (iCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iCoumuteShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			chErrorMessage = (char*)malloc(iInfoLogLength);
			if (chErrorMessage != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iCoumuteShaderObject, GL_INFO_LOG_LENGTH, &written, chErrorMessage);
				fprintf_s(buffterFliesLogFIle, "COMPUTE Shader Error : %s \n", chErrorMessage);
				free(chErrorMessage); chErrorMessage = NULL;
				return INIT_COMPUTE_SHADER_COMPILATION_FAILED;
			}
		}
	}

	g_ComputeProgramObject_Particle = glCreateProgram();

	glAttachShader(g_ComputeProgramObject_Particle, iCoumuteShaderObject);

	//Link Shader
	glLinkProgram(g_ComputeProgramObject_Particle);

	GLint iLinkStatus = 0;
	iInfoLogLength = 0;
	chErrorMessage = NULL;
	glGetProgramiv(g_ComputeProgramObject_Particle, GL_LINK_STATUS, &iLinkStatus);

	if (iLinkStatus == GL_FALSE)
	{
		glGetProgramiv(g_ComputeProgramObject_Particle, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			GLsizei written;
			chErrorMessage = (char*)malloc(iInfoLogLength);
			if (chErrorMessage != NULL)
			{
				glGetProgramInfoLog(g_ComputeProgramObject_Particle, GL_INFO_LOG_LENGTH, &written, chErrorMessage);
				fprintf_s(buffterFliesLogFIle, "Error In COMPUTE ShaderProgram: %s \n", chErrorMessage);
				free(chErrorMessage); chErrorMessage = NULL;
				return INIT_LINK_SHADER_PROGRAM_FAILED;
			}
		}
	}

	g_Uniform_CS_closest_allowed_dist = glGetUniformLocation(g_ComputeProgramObject_Particle, "closest_allowed_dist");
	g_Uniform_CS_rule1_weight = glGetUniformLocation(g_ComputeProgramObject_Particle, "rule1_weight");
	g_Uniform_CS_rule2_weight = glGetUniformLocation(g_ComputeProgramObject_Particle, "rule2_weight");
	g_Uniform_CS_rule3_weight = glGetUniformLocation(g_ComputeProgramObject_Particle, "rule3_weight");
	g_Uniform_CS_rule4_weight = glGetUniformLocation(g_ComputeProgramObject_Particle, "rule4_weight");
	g_Uniform_CS_goal = glGetUniformLocation(g_ComputeProgramObject_Particle, "goal");
	g_Uniform_CS_timeStep = glGetUniformLocation(g_ComputeProgramObject_Particle, "timestep");
	/*Compute Shader Program*/

	/*Vertex Shader Start*/
	iVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	const GLchar *vertexShaderSourceCode = "#version 430 core"	\
		"\n" \
		"layout (location = 0)in vec3 vPosition;\n" \
		"layout (location = 1)in vec3 vNormal;\n" \
		"layout (location = 2)in vec2 vTexCoord;\n" \

		"layout (location = 3)in vec3 vBirdPos;\n" \
		"layout (location = 4)in vec3 vBirdVel;\n" \

		"out vec3 out_Color;\n" \
		"out vec2 out_TexCoord;\n" \
		"uniform mat4 u_model_matrix;\n" \
		"uniform mat4 u_view_matrix;\n" \
		"uniform mat4 u_projection_matrix;\n" \
		"mat4 make_lookat(vec3 forward, vec3 up)"	\
		"{\n"	\
		"	vec3 side = cross(forward, up);"	\
		"	vec3 u_frame = cross(side, forward);"	\
		"	return mat4(vec4(side, 0.0),vec4(u_frame, 0.0),vec4(forward, 0.0),vec4(0.0, 0.0, 0.0, 1.0));"	\
		"}\n"	\
		"vec3 choose_color(float f)"	\
		"{\n"	\
		"	float R = sin(f * 6.2831853);\n"	\
		"	float G = sin((f + 0.3333) * 6.2831853);\n"	\
		"	float B = sin((f + 0.6666) * 6.2831853);\n"	\
		"	return vec3(R, G, B) * 0.25 + vec3(0.75);\n"	\
		"}\n"	\
		"\n"	\
		"void main(void)" \
		"{\n" \
		"	mat4 lookAt=make_lookat(normalize(vBirdVel),vec3(0.0, 1.0, 0.0));\n"	\
		"	vec4 obj_coord=lookAt*vec4(vPosition.xyz,1.0);"	\
		"	vec3 N = mat3(lookAt) * vNormal;"	\
		"	vec3 C = choose_color(fract(float(gl_InstanceID / float(1237.0))));"	\
		"	gl_Position =  u_projection_matrix * u_view_matrix * u_model_matrix*(obj_coord+vec4(vBirdPos,0.0));\n" \
		"	out_Color = mix(C , C* 0.2, smoothstep(0.0, 0.8, abs(N).z));\n"	\
		"	out_TexCoord = vec2(vTexCoord.x,-vTexCoord.y);\n"	\
		"}";

	glShaderSource(iVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	// Compile it
	glCompileShader(iVertexShaderObject);
	iInfoLogLength = 0;
	GLint iShaderCompileStatus = 0;
	GLchar *szInfoLog = NULL;
	glGetShaderiv(iVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iVertexShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(buffterFliesLogFIle, "ERROR : Vertex Shader Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_VERTEX_SHADER_COMPILATION_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}

	/*Vertex Shader End*/

	/*Fragment Shader Start*/
	iFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar *fragmentShaderSourceCode = "#version 430 core"	\
		"\n"	\
		"in vec3 out_Color;"	\
		"in vec2 out_TexCoord;\n" \
		"out vec4 FragColor;"	\
		"uniform sampler2D s_texture;"	\
		"uniform float numberOfRows;\n"	\
		"uniform vec2 offset1;\n"	\
		"uniform vec2 offset2;\n"	\
		"uniform float blendFactor;"	\
		"void main(void)"	\
		"{"	\
		"	vec4 texColor1;\n"	\
		"	vec4 texColor2;\n"	\
		"	vec4 finColor;\n"		\
		"	vec2 newTexcoords1 = (vec2(out_TexCoord.x,out_TexCoord.y)/numberOfRows) + offset1;\n"	\
		"	vec2 newTexcoords2 = (vec2(out_TexCoord.x,out_TexCoord.y)/numberOfRows) + offset2;\n"	\
		"	texColor1 = texture(s_texture,newTexcoords1);"	\
		"	texColor2 = texture(s_texture,newTexcoords2);"	\
		"	finColor = mix(texColor1,texColor2,blendFactor);"	\
		"	if( finColor.a <= 0.3)"	\
		"	{\n"	\
		"		discard;\n"	\
		"	}\n"	\
		"	FragColor = vec4(finColor.r,finColor.g,finColor.b,1.0);"	\
		"}";

	glShaderSource(iFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	glCompileShader(iFragmentShaderObject);
	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(iFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iFragmentShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf(buffterFliesLogFIle, "ERROR: Fragment Shader Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_FRAGMENT_SHADER_COMPILATION_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}
	/*Fragment Shader End*/

	/* Shader Program Start */
	g_ShaderProgramObject_Particle = glCreateProgram();
	glAttachShader(g_ShaderProgramObject_Particle, iVertexShaderObject);
	glAttachShader(g_ShaderProgramObject_Particle, iFragmentShaderObject);
	//glBindAttribLocation(g_ShaderProgramObject_Particle, SAM_ATTRIBUTE_POSITION, "vPosition");
	//glBindAttribLocation(g_ShaderProgramObject_Particle, SAM_ATTRIBUTE_COLOR, "vColor");
	glLinkProgram(g_ShaderProgramObject_Particle);

	GLint iShaderLinkStatus = 0;
	iInfoLogLength = 0;
	glGetProgramiv(g_ShaderProgramObject_Particle, GL_LINK_STATUS, &iShaderLinkStatus);
	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(g_ShaderProgramObject_Particle, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(g_ShaderProgramObject_Particle, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(buffterFliesLogFIle, "ERROR : Linking Shader Program Objects Failed %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_LINK_SHADER_PROGRAM_FAILED;
				//DestroyWindow(g_hwnd);
				//exit(EXIT_FAILURE);
			}
		}
	}
	/* Shader Program End */

	/*Setup Uniforms Start*/
	g_Uniform_Model_Matrix = glGetUniformLocation(g_ShaderProgramObject_Particle, "u_model_matrix");
	g_Uniform_Projection_Matrix = glGetUniformLocation(g_ShaderProgramObject_Particle, "u_projection_matrix");
	g_Uniform_View_Matrix = glGetUniformLocation(g_ShaderProgramObject_Particle, "u_view_matrix");

	g_uniform_ButterFlySampler = glGetUniformLocation(g_ShaderProgramObject_Particle, "s_texture");
	g_uniform_NumberofRows = glGetUniformLocation(g_ShaderProgramObject_Particle, "numberOfRows");
	g_uniform_Offset1 = glGetUniformLocation(g_ShaderProgramObject_Particle, "offset1");
	g_uniform_Offset2 = glGetUniformLocation(g_ShaderProgramObject_Particle, "offset2");
	g_uniform_blendFactor = glGetUniformLocation(g_ShaderProgramObject_Particle, "blendFactor");
	/*Setup Uniforms End*/
	/// Sam : all Shader Code End

	/*const vmath::vec3 geometry[] =
	{
		// Positions
		vmath::vec3(-5.0f, 1.0f, 0.0f),
		vmath::vec3(-1.0f, 1.5f, 0.0f),
		vmath::vec3(-1.0f, 1.5f, 7.0f),
		vmath::vec3(0.0f, 0.0f, 0.0f),
		vmath::vec3(0.0f, 0.0f, 10.0f),
		vmath::vec3(1.0f, 1.5f, 0.0f),
		vmath::vec3(1.0f, 1.5f, 7.0f),
		vmath::vec3(5.0f, 1.0f, 0.0f),

		// Normals
		vmath::vec3(0.0f),
		vmath::vec3(0.0f),
		vmath::vec3(0.107f, -0.859f, 0.00f),
		vmath::vec3(0.832f, 0.554f, 0.00f),
		vmath::vec3(-0.59f, -0.395f, 0.00f),
		vmath::vec3(-0.832f, 0.554f, 0.00f),
		vmath::vec3(0.295f, -0.196f, 0.00f),
		vmath::vec3(0.124f, 0.992f, 0.00f),
	};*/

	static const glm::vec3 geometry[] =
	{
		// vertices
		glm::vec3(-10.0f,-10.0f,0.0f),
		glm::vec3(-10.0f,10.0f,0.0f),
		glm::vec3(10.0f,-10.0f,0.0f),

		glm::vec3(10.0f,10.0f,0.0f),

		// Normals
		glm::vec3(0.0f,1.0f,0.0f),
		glm::vec3(0.0f,1.0f,0.0f),
		glm::vec3(0.0f,1.0f,0.0f),

		glm::vec3(0.0f,1.0f,0.0f),

		// Texcoords
		glm::vec3(0.0f,0.0f,0.0f),
		glm::vec3(0.0f,1.0f,0.0f),
		glm::vec3(1.0f,0.0f,0.0f),

		glm::vec3(1.0f,1.0f,0.0f),
	};


	static const GLfloat geoMetryVNT[] =
	{
		// vertices			// Normal			// Texcoord
		-10.0f,-10.0f,0.0f,	0.0f,1.0f,0.0f,		0.0f,0.0f,
		10.0f,-10.0f,0.0f,	0.0f,1.0f,0.0f,		1.0f,0.0f,
		-10.0f,10.0f,0.0f,	0.0f,1.0f,0.0f,		0.0f,1.0f,

		10.0f,10.0f,0.0f,	0.0f,1.0f,0.0f,		1.0f,1.0f,

	};

	glGenBuffers(2, flock_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, flock_buffer[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE * sizeof(flock_member), NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, flock_buffer[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE * sizeof(flock_member), NULL, GL_DYNAMIC_COPY);

	int i;

	glGenBuffers(1, &geometry_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geoMetryVNT), geoMetryVNT, GL_STATIC_DRAW);

	glGenVertexArrays(2, flock_render_vao);

	for (i = 0; i < 2; i++)
	{
		glBindVertexArray(flock_render_vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, geometry_buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

		glBindBuffer(GL_ARRAY_BUFFER, flock_buffer[i]);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(flock_member), NULL);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(flock_member), (void *)sizeof(glm::vec4));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
	}

	glBindBuffer(GL_ARRAY_BUFFER, flock_buffer[0]);
	flock_member * ptr = reinterpret_cast<flock_member *>(glMapBufferRange(GL_ARRAY_BUFFER, 0, FLOCK_SIZE * sizeof(flock_member), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

	for (i = 0; i < FLOCK_SIZE; i++)
	{
		ptr[i].position = (randomValues() - glm::vec3(0.5f)) * 300.0f;
		ptr[i].velocity = (randomValues() - glm::vec3(0.5f));
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);



	ilInit();
	if (LoadPNGTexture(&g_TextureButterFly, "atlas.png", &g_iImgRows, &g_iImgCols))
	{
		MessageBox(NULL, TEXT("Image Not Loaded"), TEXT("msg"), MB_OK | MB_ICONINFORMATION);
	}


	return INIT_ALL_OK;
}

void CFlockingButterflies::DeleteFlockingButterflies()
{
	this->~CFlockingButterflies();
}

void CFlockingButterflies::RenderFlockingButterflies(glm::mat4 Model, glm::mat4 View, glm::mat4 Projection)
{

	Update();

	int stageCount = iNumberOfRows * iNumberOfRows;
	float atlasProgression = g_AnimTime * stageCount;
	int index1 = (int)floor(atlasProgression);
	int index2 = index1 < stageCount - 1 ? index1 + 1 : index1;

	double integer;
	float blendFactor = (float) modf(atlasProgression, &integer);

	/* v2Offset1,v2Offset2 and fBlendfactor mist be updated per frame */
	// column = (iTextureIndex%iNumberOfRows)
	// xOffset = column / totalNumberOfColumn;
	float xOffset = (float) (index1%iNumberOfRows) / iNumberOfRows;
	// row = floor(iTextureIndex/iNumberOfRows)
	// yOffset = row / totalNumberOfrows
	float yOffset = (float) floor(index1 / iNumberOfRows) / iNumberOfRows;
	glm::vec2 v2Offset1 = glm::vec2(xOffset, yOffset);

	xOffset = (float)(index2%iNumberOfRows) / (float)iNumberOfRows;
	yOffset = (float)floor(index2 / iNumberOfRows) / (float)iNumberOfRows;
	glm::vec2 v2Offset2 = glm::vec2(xOffset, yOffset);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(g_ComputeProgramObject_Particle);
	//glm::vec3 goal = glm::vec3(2.0*sinf(g_AnimTime * 0.34f),
	//	2.0f*cosf(g_AnimTime * 0.29f),
	//	sinf(g_AnimTime * 0.12f) * cosf(g_AnimTime * 0.5f));
	glm::vec3 goal = glm::vec3(2.0*sinf(g_FlockTime * 0.34f),
		2.0f*cosf(g_FlockTime * 0.29f),
		sinf(g_FlockTime * 0.12f) * cosf(g_FlockTime * 0.5f));

	//goal = goal * glm::vec3(35.0f, 25.0f, 60.0f);
	goal = goal * glm::vec3(50.0f, 0.0f, 50.0f);
	glUniform3fv(g_Uniform_CS_goal, 1, glm::value_ptr(goal));
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, flock_buffer[swap_index]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, flock_buffer[swap_index ^ 1]);
	glDispatchCompute(NUM_WORKGROUPS, 1, 1);
	glUseProgram(0);
	///swap_index

	glm::mat4 modelMatrix = glm::mat4(1.0F);
	glm::mat4 viewMatrix = glm::mat4(1.0F);

	//viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, -400.0f),	glm::vec3(0.0f, 0.0f, 0.0f),	glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = Model;
	viewMatrix = View;

	g_PersPectiveProjectionMatrix = Projection;
	glUseProgram(g_ShaderProgramObject_Particle);

	glUniformMatrix4fv(g_Uniform_Model_Matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(g_Uniform_View_Matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(g_Uniform_Projection_Matrix, 1, GL_FALSE, glm::value_ptr(g_PersPectiveProjectionMatrix));
	glUniform1f(g_uniform_NumberofRows, (float)iNumberOfRows);
	glUniform2fv(g_uniform_Offset1, 1, glm::value_ptr(v2Offset1));
	glUniform2fv(g_uniform_Offset2, 1, glm::value_ptr(v2Offset2));
	glUniform1f(g_uniform_blendFactor, blendFactor);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_TextureButterFly);
	glUniform1i(g_uniform_ButterFlySampler, 0);
	//glUniform1f(g_Uniform_CS_timeStep, g_AnimTime);

	glBindVertexArray(flock_render_vao[swap_index]);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, FLOCK_SIZE);

	glUseProgram(0);

	swap_index ^= 1;
}



void CFlockingButterflies::Update(void)
{
	g_AnimTime = g_AnimTime + 0.04f;
	if (g_AnimTime > 1.0f)
	{
		g_AnimTime = 0.0f;
	}

	g_FlockTime = GetTickCount() / 1000.0f;
}

