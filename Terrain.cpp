#include"Terrain.h"


// Enable mutitexture blending across the terrain
#ifndef ENABLE_MULTITEXTURE
#define ENABLE_MULTITEXTURE 1
#endif

// Enable the blend constants based on the slope of the terrain
#ifndef ENABLE_SLOPE_BASED_BLEND
#define ENABLE_SLOPE_BASED_BLEND 1
#endif


#define BUFFER_OFFSET(i) ((char*)NULL + (i))

// fstream to read files
namespace fs = std::experimental::filesystem;



CTerrain::CTerrain()
{
	iShaderProgramObject_Terrain = 0;
	iVertexArrayObject_Terrain=0;
	iVertexBufferObject_Position_Terrain=0;
	iVertexBufferObject_Color_Terrain=0;

	iUniform_Model_Matrix_Terrain=0;
	iUniform_View_Matrix_Terrain=0;
	iUniform_Projection_Matrix_Terrain=0;

	iTexture_Stone=0;
	iTexture_BG=0;
	iTexture_R=0;
	iTexture_G=0;
	iTexture_B=0;
	iTexture_BlendMap=0;
	iTexture_Normal=0;

	m4TerrainPerspectiveProjectionMatrix = glm::mat4(1.0);

	if (fopen_s(&terrainLogFIle, "TerrainLogFile.txt", "w+"))
	{
		MessageBox(NULL, TEXT("Failed to ccreate log file"), TEXT("Error...!!"), MB_OK);
	}
	else
	{
		fprintf_s(terrainLogFIle, "Log File Created\n");
	}

	/*HeightScale = 5000.0;
	BlockScale = 80.0f;	*/
	HeightScale = 300.0;
	BlockScale = 10.0f;
}

int CTerrain::SetUpTerrain()
{

	// Shaders
	GLuint iVertexShaderObject_ter = 0;
	GLuint iFragmentShaderObject_ter = 0;
	size_t iShaderSourceLength = 0;


	/*Vertex Shader Start*/
	iVertexShaderObject_ter = glCreateShader(GL_VERTEX_SHADER);
/*	const GLchar *vertexShaderSourceCode_ter = "#version 450 core"	\
		"\n" \
		"layout (location = 0)in vec3 vPosition;" \
		"layout (location = 1)in vec4 vColor;" \
		"layout (location = 2)in vec3 vNormal;" \
		"layout (location = 3)in vec3 vTangent;" \
		"layout (location = 5)in vec2 vTexCoord0;" \
		"out VS_OUT\n"	\
		"{"	\
		"	vec2 TexCoords0;\n"	\
		"	vec3 FragPos;\n"	\
		"	vec3 FragPosWorldCoord;\n"	\
		"	vec3 FragPosEyeCoord;\n"	\
		"	vec3 Normal;\n"	\
		"	vec4 ShadowCoord;\n"	\
		"	vec4 OutColor;\n"	\
		"}vs_out;"	\
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform mat4 u_Light_view_matrix;\n" \
		"uniform mat4 u_Light_projection_matrix;\n" \
		"uniform mat4 u_Light_scaleBias_matrix;\n"	\
		"void main(void)" \
		"{" \
		"	mat4 shadowMatrix = u_Light_scaleBias_matrix * u_Light_projection_matrix * u_Light_view_matrix;\n"	\
		"	vs_out.FragPos = vec3(u_model_matrix * vec4(vPosition, 1.0));\n"	\
		"	vs_out.FragPosWorldCoord = vPosition;\n"	\
		"	vs_out.FragPosEyeCoord = vec3(u_view_matrix * u_model_matrix * vec4(vPosition, 1.0));\n"	\
		"	vs_out.Normal = mat3(transpose(inverse(u_model_matrix))) * vNormal;\n"	\
		"	vs_out.TexCoords0 = vTexCoord0;\n"	\
		"	vs_out.OutColor = vColor;\n"	\
		"	vec4 WorldPos = u_model_matrix * vec4(vPosition, 1.0);\n"	\
		"	vs_out.ShadowCoord = shadowMatrix * WorldPos;\n"	\
		"	gl_Position =  u_projection_matrix * u_view_matrix * u_model_matrix * vec4(vPosition,1.0);\n" \
		"}";
*/
	
	GLchar *vertexShaderSourceCode_ter = NULL;
	
	vertexShaderSourceCode_ter = LoadShaderSourceFromFile("TerrainData/shaders/TerrainVertexShadet.vert", "", &iShaderSourceLength);

	if (vertexShaderSourceCode_ter == NULL)
	{
		fprintf_s(terrainLogFIle, "Terrain:ERROR : Vertex Shader Terrain File reading issue\n");
		return INIT_VERTEX_SHADER_COMPILATION_FAILED;
	}

	glShaderSource(iVertexShaderObject_ter, 1, (const GLchar**)&vertexShaderSourceCode_ter, NULL);

	// Compile it
	glCompileShader(iVertexShaderObject_ter);
	GLint iInfoLogLength = 0;
	GLint iShaderCompileStatus = 0;
	GLchar *szInfoLog = NULL;
	glGetShaderiv(iVertexShaderObject_ter, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iVertexShaderObject_ter, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength>0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iVertexShaderObject_ter, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(terrainLogFIle, "Terrain:ERROR : Vertex Shader Terrain Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				this->DeleteTerrain();
				return INIT_VERTEX_SHADER_COMPILATION_FAILED;

			}
		}
	}

	/*Vertex Shader End*/

	/*Fragment Shader Start*/
	iFragmentShaderObject_ter = glCreateShader(GL_FRAGMENT_SHADER);
	/*GLchar *fragmentShaderSourceCode_ter = "#version 450 core"	\
		"\n"	\
		"layout (location = 0)out vec4 FragColor;"	\
		"in VS_OUT\n"	\
		"{"	\
		"	vec2 TexCoords0;\n"	\
		"	vec3 FragPos;\n"	\
		"	vec3 FragPosWorldCoord;\n"	\
		"	vec3 FragPosEyeCoord;\n"	\
		"	vec3 Normal;\n"	\
		"	vec4 ShadowCoord;\n"	\
		"	vec4 OutColor;\n"	\
		"}fs_in;\n"	\
		"struct Light"	\
		"{\n"	\
		"	vec3 direction;\n"	\
		"	vec3 ambient;\n"	\
		"	vec3 diffuse;\n"	\
		"	vec3 specular;\n"	\
		"};\n"	\
		"struct Material"	\
		"{\n"	\
		"	sampler2D DiffuseMap;\n"	\
		"	sampler2D SpecularMap;\n"	\
		"	float MaterialShininess;\n"	\
		"};\n"	\
		"uniform float uXPixelOffset;\n"\
		"uniform float uYPixelOffset;\n"\
		"uniform Material TerrainTexture;\n"\
		"uniform vec3 u_veiw_position;\n"\
		"layout(binding=0)uniform sampler2D u_texture0_bg_sampler;\n"\
		"layout(binding=1)uniform sampler2D u_texture1_r_sampler;\n"\
		"layout(binding=2)uniform sampler2D u_texture2_g_sampler;\n"\
		"layout(binding=3)uniform sampler2D u_texture3_b_sampler;\n"\
		"layout(binding=4)uniform sampler2D u_texture4_blendmap_sampler;\n"\
		"layout(binding=5)uniform sampler2DShadow u_texture5_depth_sampler;\n"\
		"uniform Light SunLight;\n"\
		"void main(void)"\
		"{"\
		"	vec3 phong_ads;\n"\
		"	vec3 normalized_normal = normalize(fs_in.Normal);\n"\
		"	vec3 view_direction=normalize(u_veiw_position-fs_in.FragPos);\n"\
		"	vec4 blendMapColor = texture(u_texture4_blendmap_sampler,fs_in.TexCoords0);\n"\
		"	float backTextureAmount = 1.0-(blendMapColor.r+blendMapColor.g+blendMapColor.b);\n"\
		"	vec2 tiledCoords=fs_in.TexCoords0*80;\n"\
		"	vec4 backgroundTextureColor = texture(u_texture0_bg_sampler,tiledCoords)*backTextureAmount;\n"\
		"	vec4 rTextureColor=texture(u_texture1_r_sampler,tiledCoords)*blendMapColor.r;\n"\
		"	vec4 gTextureColor=texture(u_texture2_g_sampler,tiledCoords)*blendMapColor.g;\n"\
		"	vec4 bTextureColor=texture(u_texture3_b_sampler,tiledCoords)*blendMapColor.b;\n"\
		"	vec4 FinalTerrainColor = backgroundTextureColor+rTextureColor+gTextureColor+bTextureColor;\n"\
		"	vec3 ambient = SunLight.ambient * FinalTerrainColor.rgb;\n"\
		"	vec3 light_direction = normalize(-SunLight.direction);\n"\
		"	float diffuse_multiplier = max(dot(normalized_normal,light_direction),0.0);\n"\
		"	vec3 diffuse = SunLight.diffuse * diffuse_multiplier * FinalTerrainColor.rgb;\n"\
		"	vec3 half_vector = normalize(light_direction+view_direction);\n"\
		"	float specular_multiplier = pow(max(dot(normalized_normal,half_vector),0.0),TerrainTexture.MaterialShininess);\n"\
		"	float shadow = textureProj(u_texture5_depth_sampler,fs_in.ShadowCoord);\n"\
		"	FragColor = vec4(ambient + (shadow* diffuse),1.0);"\
		"	FragColor = FinalTerrainColor;\n"	\
		"}\n";*/

	GLchar *fragmentShaderSourceCode_ter=LoadShaderSourceFromFile("TerrainData/shaders/TerrainFragmentShadet.frag", "", &iShaderSourceLength);

	if (fragmentShaderSourceCode_ter == NULL)
	{
		fprintf_s(terrainLogFIle, "Terrain:ERROR : Fragment Shader Terrain File reading issue\n");
		return INIT_FRAGMENT_SHADER_COMPILATION_FAILED;
	}

	glShaderSource(iFragmentShaderObject_ter, 1, (const GLchar**)&fragmentShaderSourceCode_ter, NULL);
	glCompileShader(iFragmentShaderObject_ter);
	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(iFragmentShaderObject_ter, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iFragmentShaderObject_ter, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength>0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iFragmentShaderObject_ter, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf(terrainLogFIle, "Terrain:ERROR: Fragment Shader Compilation Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				this->DeleteTerrain();
				return INIT_FRAGMENT_SHADER_COMPILATION_FAILED;

			}
		}
	}
	/*Fragment Shader End*/


	/* Shader Program Start */
	iShaderProgramObject_Terrain = glCreateProgram();
	glAttachShader(iShaderProgramObject_Terrain, iVertexShaderObject_ter);
	glAttachShader(iShaderProgramObject_Terrain, iFragmentShaderObject_ter);
	glBindAttribLocation(iShaderProgramObject_Terrain, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(iShaderProgramObject_Terrain, AMC_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(iShaderProgramObject_Terrain, AMC_ATTRIBUTE_TANGENT, "vTangent");
	glBindAttribLocation(iShaderProgramObject_Terrain, AMC_ATTRIBUTE_NORMAL, "vNormal");

	glLinkProgram(iShaderProgramObject_Terrain);

	GLint iShaderLinkStatus = 0;
	iInfoLogLength = 0;
	glGetProgramiv(iShaderProgramObject_Terrain, GL_LINK_STATUS, &iShaderLinkStatus);
	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(iShaderProgramObject_Terrain, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength>0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iShaderProgramObject_Terrain, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(terrainLogFIle, "Terrain:ERROR : Linking Shader Program Terrain Objects Failed %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				this->DeleteTerrain();
				return INIT_LINK_SHADER_PROGRAM_FAILED;

			}
		}
	}
	/* Shader Program End */

	// get uniform locations

	iUniform_Model_Matrix_Terrain = glGetUniformLocation(iShaderProgramObject_Terrain, "u_model_matrix");
	iUniform_View_Matrix_Terrain = glGetUniformLocation(iShaderProgramObject_Terrain, "u_view_matrix");
	iUniform_Projection_Matrix_Terrain = glGetUniformLocation(iShaderProgramObject_Terrain, "u_projection_matrix");

	iUniform_ScaleBias_Matrix_Terrain = glGetUniformLocation(iShaderProgramObject_Terrain, "u_Light_scaleBias_matrix");
	iUniform_Light_View_Matrix_Terrain = glGetUniformLocation(iShaderProgramObject_Terrain, "u_Light_view_matrix");
	iUniform_Light_Projection_Matrix_Terrain = glGetUniformLocation(iShaderProgramObject_Terrain, "u_Light_projection_matrix");

	iUniform_Texture0_BG_Sampler = glGetUniformLocation(iShaderProgramObject_Terrain, "u_texture0_bg_sampler");
	iUniform_Texture1_R_Sampler = glGetUniformLocation(iShaderProgramObject_Terrain, "u_texture1_r_sampler");
	iUniform_Texture2_G_Sampler = glGetUniformLocation(iShaderProgramObject_Terrain, "u_texture2_g_sampler");
	iUniform_Texture3_B_Sampler = glGetUniformLocation(iShaderProgramObject_Terrain, "u_texture3_b_sampler");
	iUniform_Texture4_BlendMap_Sampler = glGetUniformLocation(iShaderProgramObject_Terrain, "u_texture4_blendmap_sampler");
	iUniform_Texture5_DepthMap_Sampler = glGetUniformLocation(iShaderProgramObject_Terrain, "u_texture5_depth_sampler");

	iUniform_xPixelOffset = glGetUniformLocation(iShaderProgramObject_Terrain, "uXPixelOffset");
	iUniform_yPixelOffset = glGetUniformLocation(iShaderProgramObject_Terrain, "uYPixelOffset");

	/*GLint UniformTerrain_XPixelOffset = glGetUniformLocation(iShaderProgramObject_Terrain, "uXPixelOffset");
	GLint UniformTerrain_YPixelOffset = glGetUniformLocation(iShaderProgramObject_Terrain, "uYPixelOffset");
	GLint UniformTerrain_ViewPosition = glGetUniformLocation(iShaderProgramObject_Terrain, "u_veiw_position");
	GLint UniformTerrain_shadowBias = glGetUniformLocation(iShaderProgramObject_Terrain, "u_Light_scaleBias_matrix");
	GLint UniformTerrain_lightprojrction_Matrix = glGetUniformLocation(iShaderProgramObject_Terrain, "u_Light_projection_matrix");
	GLint UniformTerrain_lightView_Matrix = glGetUniformLocation(iShaderProgramObject_Terrain, "u_Light_view_matrix");

	GLint UniformTerrain_sunAmbient = glGetUniformLocation(iShaderProgramObject_Terrain, "SunLight.ambient");
	GLint UniformTerrain_sunSpecular = glGetUniformLocation(iShaderProgramObject_Terrain, "SunLight.specular");
	GLint UniformTerrain_sunDiffuse = glGetUniformLocation(iShaderProgramObject_Terrain, "SunLight.diffuse");
	GLint UniformTerrain_sunDirection = glGetUniformLocation(iShaderProgramObject_Terrain, "SunLight.direction");*/

	// lets print all uniforms in our shader program....!!!
	GLint i;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 255; // maximum name length
	GLchar name[bufSize]; // variable name in GLSL
	GLsizei length; // name length

	glGetProgramiv(iShaderProgramObject_Terrain, GL_ACTIVE_UNIFORMS, &count);

	fprintf_s(terrainLogFIle, "Active Uniforms: %d\n",count);
	for (i = 0; i < count; i++)
	{
		glGetActiveUniform(iShaderProgramObject_Terrain, (GLuint)i, bufSize, &length, &size, &type, name);
		fprintf_s(terrainLogFIle,"Uniform #%d Type: %u Name: %s\n", i, type, name);
	}

	// Fill all data to vbo
	glGenVertexArrays(1, &iVertexArrayObject_Terrain);
	glBindVertexArray(iVertexArrayObject_Terrain);

	glGenBuffers(1, &iVertexBufferObject_Position_Terrain);
	glBindBuffer(GL_ARRAY_BUFFER, iVertexBufferObject_Position_Terrain);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vTerrainPositionBuffer.size(), &vTerrainPositionBuffer[0], GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &iVertexBufferObject_Color_Terrain);
	glBindBuffer(GL_ARRAY_BUFFER, iVertexBufferObject_Color_Terrain);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*vTerrainColorBuffer.size(), &vTerrainColorBuffer[0], GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &iVertexBufferObject_Normal_Terrain);
	glBindBuffer(GL_ARRAY_BUFFER, iVertexBufferObject_Normal_Terrain);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vTerrainNormalBuffer.size(), &vTerrainNormalBuffer[0], GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &iVertexBufferObject_Tangent_Terrain);
	glBindBuffer(GL_ARRAY_BUFFER, iVertexBufferObject_Tangent_Terrain);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vTerrainTangentBuffer.size(), &vTerrainTangentBuffer[0], GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TANGENT, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TANGENT);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &iVertexBufferObject_Texture_Terrain);
	glBindBuffer(GL_ARRAY_BUFFER, iVertexBufferObject_Texture_Terrain);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*vTerrainTexCoordBuffer.size(), &vTerrainTexCoordBuffer[0], GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &iVertexBufferObject_Idices_Terrain);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iVertexBufferObject_Idices_Terrain);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,vTerrainIndexBuffer.size() * sizeof(GLuint),&vTerrainIndexBuffer[0],GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	/*Load all textures*/

	iTexture_Stone = tgaLoader.LoadTGATexture("TerrainData/blackrock-albedo.tga");
	iTexture_BG = tgaLoader.LoadTGATexture("TerrainData/mixedmoss-albedo2.tga");
	//iTexture_Normal = tgaLoader.LoadTGATexture("TerrainData/mixedmoss-normal2.tga");
	iTexture_R = tgaLoader.LoadTGATexture("TerrainData/dry-dirt2-albedo.tga");
	iTexture_G = tgaLoader.LoadTGATexture("TerrainData/ground_1024_v2_Q3.tga");
	iTexture_B = tgaLoader.LoadTGATexture("TerrainData/octostoneAlbedo.tga");

	if (LoadBMPTexture(MAKEINTRESOURCE(IDBITMAP_BLENDMAP), &iTexture_BlendMap))
	{
		fprintf_s(terrainLogFIle, "IDBITMAP_BLENDMAP loaded successfully\n");
	}

	return INIT_ALL_OK;

}



CTerrain::~CTerrain()
{

	if (iTexture_Stone)
	{
		glDeleteTextures(1, &iTexture_Stone);
		iTexture_Stone = NULL;
	}
	if (iTexture_BG)
	{
		glDeleteTextures(1, &iTexture_BG);
		iTexture_BG = NULL;
	}

	if (iTexture_R)
	{
		glDeleteTextures(1, &iTexture_R);
		iTexture_R = NULL;
	}

	if (iTexture_G)
	{
		glDeleteTextures(1, &iTexture_G);
		iTexture_G = NULL;
	}

	if (iTexture_B)
	{
		glDeleteTextures(1, &iTexture_B);
		iTexture_B = NULL;
	}

	if (iTexture_BlendMap)
	{
		glDeleteTextures(1, &iTexture_BlendMap);
		iTexture_BlendMap = NULL;
	}

	if (iTexture_Normal)
	{
		glDeleteTextures(1, &iTexture_Normal);
		iTexture_Normal = NULL;
	}
	
	
	if (iVertexBufferObject_Color_Terrain)
	{
		glDeleteBuffers(1, &iVertexBufferObject_Color_Terrain);
		iVertexBufferObject_Color_Terrain = NULL;
	}

	if (iVertexBufferObject_Position_Terrain)
	{
		glDeleteBuffers(1, &iVertexBufferObject_Position_Terrain);
		iVertexBufferObject_Position_Terrain = NULL;
	}

	if (iVertexArrayObject_Terrain)
	{
		glDeleteVertexArrays(1, &iVertexArrayObject_Terrain);
		iVertexArrayObject_Terrain = NULL;
	}

	glUseProgram(0);

	if (iShaderProgramObject_Terrain)
	{
		GLsizei iShaderCount;
		GLsizei iShaderNumber;


		glUseProgram(iShaderProgramObject_Terrain);
		glGetProgramiv(iShaderProgramObject_Terrain, GL_ATTACHED_SHADERS, &iShaderCount);
		GLuint *pShaders = (GLuint*)calloc(iShaderCount, sizeof(GLuint));

		if (pShaders)
		{
			glGetAttachedShaders(iShaderProgramObject_Terrain, iShaderCount, &iShaderCount, pShaders);
			for (iShaderNumber = 0; iShaderNumber < iShaderCount; iShaderNumber++)
			{
				glDetachShader(iShaderProgramObject_Terrain, pShaders[iShaderNumber]);
				glDeleteShader(pShaders[iShaderNumber]);
				pShaders[iShaderNumber] = 0;
			}
			free(pShaders);
			pShaders = NULL;
		}

		glDeleteProgram(iShaderProgramObject_Terrain);
		iShaderProgramObject_Terrain = NULL;

		glUseProgram(0);

	}

	if (terrainLogFIle)
	{
		fprintf(terrainLogFIle,"Terrain Log File is closing\n");
		fclose(terrainLogFIle);
		terrainLogFIle = NULL;
	}
}

void CTerrain::SetProjection(glm::mat4 newprojection)
{
	m4TerrainPerspectiveProjectionMatrix = newprojection;
}

int CTerrain::GetFileLength(std::istream &file)
{
	int pos = (int)file.tellg();
	file.seekg(0, std::ios::end);
	int length = (int)file.tellg();
	file.seekg(pos);
	return length;
}

float CTerrain::GetHeightValue(const unsigned char *data, unsigned char numBytes)
{
	switch (numBytes)
	{
		case 1:// 1 byte
			return ( (unsigned int)data[0]/(float)(0xff) );
			break;

		case 2:// 2 byte
			return ((unsigned int) (data[1]<<8 | data[0]) / (float)(0xffff));
			break;

		case 3:// 4 byte
			return ((unsigned int)(data[3] << 24 | data[2] << 16 | data[1] << 8)/(float)(0xffffffff));
			break;

		default:
			assert(false);//feight field with non standard pixel value
			break;
	}
	return 0.0f;
}


unsigned short CTerrain::GetHeightValueShort(const unsigned char *data, unsigned char numBytes)
{
	switch (numBytes)
	{
	case 1:// 1 byte
		return ((unsigned int)data[0] / (unsigned short)(0xff));
		break;

	case 2:// 2 byte
		return ((unsigned int)(data[1] << 8 | data[0]) / (unsigned short)(0xffff));
		break;

	case 3:// 4 byte
		return ((unsigned int)(data[3] << 24 | data[2] << 16 | data[1] << 8) / (unsigned short)(0xffffffff));
		break;

	default:
		assert(false);//feight field with non standard pixel value
		break;
	}
	return 0;
}

float CTerrain::GetPercentage(float value, const float min, const float max)
{
	value = glm::clamp(value, min, max);
	return (value - min) / (max - min);
}

int CTerrain::LoadBMPTexture(TCHAR imageResourceId[], GLuint * Texture)
{
	GLfloat Anisotropy = 0.0f;

	// variable declarations
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus = FALSE;

	// code
	glGenTextures(1, Texture);// 1 image
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap)// if bitmap exists ( means hBitmap is not null )
	{
		iStatus = TRUE;
		GetObject(hBitmap, sizeof(bmp), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);// set 1 rather than default 4, for better performance
		glBindTexture(GL_TEXTURE_2D, *Texture);// bind texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		if (GLEW_EXT_texture_filter_anisotropic)
		{
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Anisotropy);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Anisotropy);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);

		// Create mipmaps for this texture for better image quality
		glGenerateMipmap(GL_TEXTURE_2D);
		DeleteObject(hBitmap);// delete unwanted bitmap handle
	}
	return(iStatus);
}

int CTerrain::LoadHeightmap(const std::string &filename, unsigned char bitsPerPixel, unsigned int width, unsigned int height)
{
	//read file
	std::ifstream ifs;
	
	if (!fs::exists(filename))
	{
		fprintf_s(terrainLogFIle, "Terrain:Could not find the file %s\n", filename.c_str());
		return INIT_RESOURCE_LOAD_FAILED;
	}

	//open file
	ifs.open(filename, std::ifstream::binary);

	//check if it is opened or not
	if (ifs.fail())
	{
		fprintf_s(terrainLogFIle, "Terrain:Failed to open %s\n", filename.c_str());
		return INIT_RESOURCE_LOAD_FAILED;
	}

	const unsigned int bytesPerPixel = bitsPerPixel / 8;
	const unsigned int expectedFileSize = (bytesPerPixel * width * height);
	const unsigned int fileSize = GetFileLength(ifs);

	if (expectedFileSize != fileSize)
	{
		fprintf(terrainLogFIle,"Terrain:Expected file size=>%d\nActual File size =>%d",expectedFileSize,fileSize);
		return INIT_RESOURCE_LOAD_FAILED;
	}

	unsigned char *heightMap = new unsigned char[fileSize];

	ifs.read((char*)heightMap,fileSize);
	
	if (ifs.fail())
	{
		fprintf(terrainLogFIle,"Terrain: Error reading heightmap file %s\n",filename.c_str());
		ifs.close();
		delete[] heightMap;
		//this->~CTerrain();
		return INIT_RESOURCE_LOAD_FAILED;
	}
	ifs.close();

	unsigned int numVertices = width*height;
	vTerrainPositionBuffer.resize(numVertices);
	vTerrainColorBuffer.resize(numVertices);
	vTerrainNormalBuffer.resize(numVertices);
	vTerrainTangentBuffer.resize(numVertices);
	vTerrainTexCoordBuffer.resize(numVertices);
	//vTerrainIndexBuffer;

	HeightMapDimensions = glm::uvec2(width,height);

	// size of terrain in world units
	float terrainWidth = (width - 1)*BlockScale;
	float terrainHeight = (height - 1)*BlockScale;

	// half terrain
	float halfTerrainWidth = terrainWidth*0.5f;
	float halfTerrainHeight = terrainHeight*0.5f;

	for (unsigned int j = 0; j < height; ++j)
	{
		for (unsigned int i = 0; i < width; ++i)
		{
			unsigned int index = (j * width) + i;
			assert(index* bytesPerPixel < fileSize);

			float heightValue = GetHeightValue(&heightMap[index* bytesPerPixel], bytesPerPixel);
			unsigned short shrtHeightValue = GetHeightValueShort(&heightMap[index* bytesPerPixel], bytesPerPixel);

			float S = ((float)i / (float)(width - 1));
			float T = ((float)j / (float)(height - 1));

			float X = (S*terrainWidth) - halfTerrainWidth;
			float Y = heightValue*HeightScale;
			float Z = (T*terrainHeight) - halfTerrainHeight;

			// Blend 3 textures depending on terrain heights
			float tex0Contribution = 1.0f - GetPercentage(heightValue,0.0f,0.75f);
			float tex2Contribution = 1.0f - GetPercentage(heightValue,0.75f,1.0f);


			//fill buffers with values
			vTerrainNormalBuffer[index]= glm::vec3(0.0f);
			vTerrainTangentBuffer[index]= glm::vec3(0.0f);
			vTerrainPositionBuffer[index]= glm::vec3(X, Y, Z);

#if ENABLE_MULTITEXTURE
			vTerrainColorBuffer[index] = glm::vec4(tex0Contribution, tex0Contribution, tex0Contribution, tex2Contribution);
#else
			vTerrainColorBuffer[index] = glm::vec4(1.0f);
#endif
			vTerrainTexCoordBuffer[index] = glm::vec2(S, T);

		}
	}

	fprintf_s(terrainLogFIle,"Terrain : Loaded Successfullt\n");
	delete[] heightMap;

	
	GenerateIndexBuffer();
	GenerateNormals();
	return INIT_ALL_OK;

}

float CTerrain::GetHeightAt(const glm::vec3 & position)
{
	float height = -FLT_MAX; // max value of float

	// validate terrain dimensions
	if (HeightMapDimensions.x < 2 || HeightMapDimensions.y < 2)
	{
		return height;
	}

	// get width and height of terrain in world units
	float terrainWidth = (HeightMapDimensions.x  -1)*BlockScale;
	float terrainHeight = (HeightMapDimensions.y - 1)*BlockScale;
	float halfWidth = terrainWidth*0.5f;
	float halfHeight = terrainHeight*0.5f;

	// multiply position by inverse of the terrain matrix to get  the position in terrain local space
	glm::vec3 terrainPosition = glm::vec3(glm::vec4(position,1.0));
	glm::vec3 invBlockScale(1.0f/BlockScale,0.0f,1.0f/BlockScale);

	// Calculate offet and scale to get vertex indices
	glm::vec3 offset(halfWidth,0.0f,halfHeight);

	// get 4 vertices that make up the triangle we were over
	glm::vec3 vertexIndices = (terrainPosition + offset) * invBlockScale;

	GLuint u0 = (GLuint)floorf(vertexIndices.x);
	GLuint u1 = u0 + 1;
	GLuint v0 = (GLuint)floorf(vertexIndices.z);
	GLuint v1 = v0 + 1;

	if (u0 >= 0 && u1 < HeightMapDimensions.x && v0 >= 0 && v1 < HeightMapDimensions.y)
	{
		glm::vec3 p00 = vTerrainPositionBuffer[(v0*HeightMapDimensions.x)+u0];// Top Left
		glm::vec3 p10 = vTerrainPositionBuffer[(v0*HeightMapDimensions.x) + u1];// Top Right
		glm::vec3 p01 = vTerrainPositionBuffer[(v1*HeightMapDimensions.x) + u0];// bottom left
		glm::vec3 p11=vTerrainPositionBuffer[(v1*HeightMapDimensions.x)+u1];// bottom left

		// which triangle are we over?
		float percentU = vertexIndices.x - u0;
		float percentV = vertexIndices.z - v0;

		glm::vec3 dU, dV;
		if (percentU > percentV)
		{
			// top triangle
			dU = p10 - p00;
			dV = p11 - p00;
		}
		else
		{
			// bottom triangle
			dU = p11 - p01;
			dV = p01 - p00;
		}

		glm::vec3 heightPos = p00 + (dU*percentU) + (dV*percentV);

		// convert back to world space bt multiplying by terrains world matrix
		heightPos = glm::vec3(glm::vec4(heightPos,1));
		height = heightPos.y;
	}

	return height;
}

void CTerrain::DeleteTerrain()
{
	this->~CTerrain();
}

void CTerrain::GenerateIndexBuffer()
{
	if (HeightMapDimensions.x < 2 || HeightMapDimensions.y < 2)
	{
		// terrain is not loaded or not of correct size
		return;
	}

	const unsigned int terrainWidth = HeightMapDimensions.x;
	const unsigned int terrainHeight = HeightMapDimensions.y;

	// Two triangles for every quad of terrain
	const unsigned int  numTriangles = (terrainWidth - 1)*(terrainHeight - 1) * 2;

	// 3 indices for each triangle in terrain mesh
	vTerrainIndexBuffer.resize(numTriangles * 3);

	unsigned int index = 0;

	for (unsigned int j = 0; j < (terrainHeight - 1); ++j)
	{
		for (unsigned int i = 0; i < (terrainWidth - 1); ++i)
		{
			int vertexIndex = (j*terrainWidth) + i;

			// Top Triangle T0
			vTerrainIndexBuffer[index++] = vertexIndex;//v0
			vTerrainIndexBuffer[index++] = vertexIndex + terrainWidth + 1;//v3
			vTerrainIndexBuffer[index++] = vertexIndex + 1;//v1

			// Bottom triangle
			vTerrainIndexBuffer[index++] = vertexIndex;//v0
			vTerrainIndexBuffer[index++] = vertexIndex + terrainWidth;//v2
			vTerrainIndexBuffer[index++] = vertexIndex + terrainWidth + 1;//v3

		}
	}
}

void CTerrain::GenerateNormals()
{
	glm::vec3 v0 = glm::vec3(0.0f);
	glm::vec3 v1 = glm::vec3(0.0f);
	glm::vec3 v2 = glm::vec3(0.0f);

	glm::vec3 normal = glm::vec3(0.0f);

	glm::vec3 Edge1 = glm::vec3(0.0f);
	glm::vec3 Edge2 = glm::vec3(0.0f);

	glm::vec2 deltaUV1 = glm::vec2(0.0f);
	glm::vec2 deltaUV2 = glm::vec2(0.0f);

	glm::vec2 v0_uv = glm::vec2(0.0f);
	glm::vec2 v1_uv = glm::vec2(0.0f);
	glm::vec2 v2_uv = glm::vec2(0.0f);

	glm::vec3 Tangent = glm::vec3(0.0f);

	GLfloat f = 0.0f;

	for (unsigned int i = 0; i < vTerrainIndexBuffer.size(); i += 3)
	{
		v0 = vTerrainPositionBuffer[vTerrainIndexBuffer[i + 0]];
		v1 = vTerrainPositionBuffer[vTerrainIndexBuffer[i + 1]];
		v2 = vTerrainPositionBuffer[vTerrainIndexBuffer[i + 2]];

		normal = glm::normalize(glm::cross(v1-v0,v2-v0));

		vTerrainNormalBuffer[vTerrainIndexBuffer[i + 0]] += normal;
		vTerrainNormalBuffer[vTerrainIndexBuffer[i + 1]] += normal;
		vTerrainNormalBuffer[vTerrainIndexBuffer[i + 2]] += normal;

		// calculate the tangent
		v0_uv = vTerrainTexCoordBuffer[vTerrainIndexBuffer[i + 0]];
		v0_uv = vTerrainTexCoordBuffer[vTerrainIndexBuffer[i + 1]];
		v0_uv = vTerrainTexCoordBuffer[vTerrainIndexBuffer[i + 2]];

		// calculate Tangent Vector
		Edge1 = v1 - v0;
		Edge2 = v2 - v0;

		deltaUV1 = v1_uv - v0_uv;
		deltaUV2 = v2_uv - v0_uv;

		f = 1.0f / (deltaUV1.x*deltaUV2.y - deltaUV2.x*deltaUV1.y);

		Tangent.x = f*(deltaUV2.y*Edge1.x - deltaUV1.y*Edge2.x);
		Tangent.y = f*(deltaUV2.y*Edge1.y - deltaUV1.y*Edge2.y);
		Tangent.z = f*(deltaUV2.y*Edge1.z - deltaUV1.y*Edge2.z);
		Tangent = glm::normalize(Tangent);

		vTerrainTangentBuffer[vTerrainIndexBuffer[i + 0]] += Tangent;
		vTerrainTangentBuffer[vTerrainIndexBuffer[i + 1]] += Tangent;
		vTerrainTangentBuffer[vTerrainIndexBuffer[i + 2]] += Tangent;

	}
	const glm::vec3 UP(0.0f, 1.0f, 0.0f);

	unsigned int nSize = (unsigned int)vTerrainNormalBuffer.size();

	for (unsigned int i = 0; i < nSize; i++)
	{
		vTerrainNormalBuffer[i] = glm::normalize(vTerrainNormalBuffer[i]);

#if ENABLE_SLOPE_BASED_BLEND
		float fTextureContribution = glm::clamp(glm::dot(vTerrainNormalBuffer[i],UP)-0.1f,0.0f,1.0f);
		vTerrainColorBuffer[i] = glm::vec4(fTextureContribution, fTextureContribution, fTextureContribution,vTerrainColorBuffer[i].w);
#endif
	}

	unsigned int nSizeTangent =(unsigned int) vTerrainTangentBuffer.size();
	for (unsigned int i = 0; i < nSizeTangent; i++)
	{
		vTerrainTangentBuffer[i] = glm::normalize(vTerrainTangentBuffer[i]);
	}

}



void CTerrain::RenderTerrain(glm::mat4 Model, glm::mat4 View, glm::mat4 Projection, GLuint depthTexture)
{


	glUseProgram(this->iShaderProgramObject_Terrain);
	glUniformMatrix4fv(this->iUniform_Model_Matrix_Terrain, 1, GL_FALSE, glm::value_ptr(Model));
	glUniformMatrix4fv(this->iUniform_View_Matrix_Terrain, 1, GL_FALSE, glm::value_ptr(View));
	glUniformMatrix4fv(this->iUniform_Projection_Matrix_Terrain, 1, GL_FALSE, glm::value_ptr(Projection));

	//bind all textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,iTexture_BG);
	glUniform1i(iUniform_Texture0_BG_Sampler,0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, iTexture_R);
	glUniform1i(iUniform_Texture1_R_Sampler, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, iTexture_G);
	glUniform1i(iUniform_Texture2_G_Sampler, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, iTexture_B);
	glUniform1i(iUniform_Texture3_B_Sampler, 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, iTexture_BlendMap);
	glUniform1i(iUniform_Texture4_BlendMap_Sampler, 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(iUniform_Texture5_DepthMap_Sampler, 5);
	
	glBindVertexArray(iVertexArrayObject_Terrain);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iVertexBufferObject_Idices_Terrain);
	glDrawElements(GL_TRIANGLES,(GLsizei)vTerrainIndexBuffer.size(),GL_UNSIGNED_INT,0);
	glBindVertexArray(0);


	glUseProgram(0);

}

void CTerrain::RenderTerrainShadow()
{
	glBindVertexArray(iVertexArrayObject_Terrain);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iVertexBufferObject_Idices_Terrain);
	glDrawElements(GL_TRIANGLES, (GLsizei)vTerrainIndexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

GLuint CTerrain::GetShaderProgram()
{
	return iShaderProgramObject_Terrain;
}
