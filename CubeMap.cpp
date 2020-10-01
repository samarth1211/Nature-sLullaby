#include"CubeMap.h"
#include "stb_image.h"

//Cube Map
//Cube Map


///CubeTex


CubeMap::CubeMap()
{

}

CubeMap::~CubeMap()
{

	if (gVboCubeMap)
	{
		glDeleteBuffers(1, &gVboCubeMap);
		gVboCubeMap = 0;
	}

	if (gVaoCubeMap)
	{
		glDeleteVertexArrays(1, &gVaoCubeMap);
		gVaoCubeMap = 0;
	}

	if (CubemapTexture)
	{
		glDeleteTextures(1, &CubemapTexture);
		CubemapTexture = NULL;
	}

	glUseProgram(0);

	if (gShaderProgramObjectCubemap)
	{
		GLsizei iShaderCount;
		GLsizei iShaderNumber;

		glUseProgram(gShaderProgramObjectCubemap);
		glGetProgramiv(gShaderProgramObjectCubemap, GL_ATTACHED_SHADERS, &iShaderCount);
		GLuint *pShaders = (GLuint*)calloc(iShaderCount, sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObjectCubemap, iShaderCount, &iShaderCount, pShaders);
			for (iShaderNumber = 0; iShaderNumber < iShaderCount; iShaderNumber++)
			{
				glDetachShader(gShaderProgramObjectCubemap, pShaders[iShaderNumber]);
				glDeleteShader(pShaders[iShaderNumber]);
				pShaders[iShaderNumber] = 0;
			}
			free(pShaders);
			pShaders = NULL;
		}
		glDeleteProgram(gShaderProgramObjectCubemap);
		gShaderProgramObjectCubemap = NULL;
		glUseProgram(0);
	}


}
void CubeMap::InitializeLightCube(GLuint ShaderProgObject)
{
	GLuint loadTexture(std::vector<std::string> faces);


	gShaderProgramObjectCubemap = ShaderProgObject;

	const GLfloat CubeVertices[] =
	{
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// For Quad
	glGenVertexArrays(1, &gVaoCubeMap);
	glBindVertexArray(gVaoCubeMap);

	// For Quad position
	glGenBuffers(1, &gVboCubeMap);
	glBindBuffer(GL_ARRAY_BUFFER, gVboCubeMap);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Texture for CubeMap
	//Create texture for Cubemap
	std::vector<std::string> faces
	{
		"NightCubemap/ft.jpg",
		"NightCubemap/bk.jpg",
		"NightCubemap/up.jpg",
		"NightCubemap/dn.jpg",
		"NightCubemap/rt.jpg",
		"NightCubemap/lf.jpg",
	};

	CubemapTexture = loadTexture(faces);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	

	glUseProgram(gShaderProgramObjectCubemap);
	glUniform1i(glGetUniformLocation(gShaderProgramObjectCubemap, "Cubemap"), 0);
	glUniform3fv(glGetUniformLocation(gShaderProgramObjectCubemap, "fogColor"), 1,glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));
	glUseProgram(0);
}

void CubeMap::DrawCubemap(glm::mat4 ViewMatrix, glm::mat4 gPerspectiveProjectionMatrix)
{
	
	//glVertexAttrib3fv(AMC_ATTRIBUTE_COLOR, glm::value_ptr(LightDiffuse));

	glUseProgram(gShaderProgramObjectCubemap);

	glUniformMatrix4fv(glGetUniformLocation(gShaderProgramObjectCubemap, "uViewMatrix"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(gShaderProgramObjectCubemap, "uProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(gPerspectiveProjectionMatrix));

	glBindVertexArray(gVaoCubeMap);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glBindVertexArray(0);
	glUseProgram(0);
}

GLuint loadTexture(std::vector<std::string> faces)
{
	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	int width, height, chanels;

	for (int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &chanels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
				GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
		else
		{
			/*fprintf(gpfile, "loadTextire fail");
			uninitialize();
			exit(0);*/
		}

	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture;
}