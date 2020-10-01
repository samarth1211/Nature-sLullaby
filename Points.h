#pragma once


#include"CommonHeader.h"
#include"Terrain.h"


// Shaders
GLuint g_ShaderProgramObject_Points = 0;

// All Vertex Buffers
GLuint g_VertexArrayObject_Points = 0;
GLuint g_VertexBufferObject_Position_Points = 0;

// Uniforms
GLuint g_Uniform_Points_Model_Matrix = 0;
GLuint g_Uniform_Points_View_Matrix = 0;
GLuint g_Uniform_Points_Projection_Matrix = 0;



int InitializePoints(CTerrain *pTerrainInst)
{
	// Shader Programs
	GLuint iVertexShaderObject = 0;
	GLuint iFragmentShaderObject = 0;


	/*Vertex Shader Start*/
	iVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	const GLchar *vertexShaderSourceCode = "#version 450 core"	\
		"\n" \
		"layout (location = 0)in vec4 vPosition;" \
		"uniform mat4 u_model_matrix,u_view_matrix,u_projection_matrix;" \
		"void main(void)" \
		"{" \
		"	gl_Position =  u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"}";

	glShaderSource(iVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	// Compile it
	glCompileShader(iVertexShaderObject);
	GLint iInfoLogLength = 0;
	GLint iShaderCompileStatus = 0;
	GLchar *szInfoLog = NULL;
	glGetShaderiv(iVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(iVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength>0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iVertexShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(g_pFile, "ERROR : Vertex Shader Compilation Log : %s \n", szInfoLog);
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
	const GLchar *fragmentShaderSourceCode = "#version 450 core"	\
		"\n"	\
		"layout (location = 0)out vec4 FragColor;"	\
		"void main(void)"	\
		"{"	\
		"	FragColor = vec4(1.0);"	\
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
		if (iInfoLogLength>0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(iFragmentShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf(g_pFile, "ERROR: Fragment Shader Compilation Log : %s \n", szInfoLog);
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
	g_ShaderProgramObject_Points = glCreateProgram();
	glAttachShader(g_ShaderProgramObject_Points, iVertexShaderObject);
	glAttachShader(g_ShaderProgramObject_Points, iFragmentShaderObject);
	glBindAttribLocation(g_ShaderProgramObject_Points, AMC_ATTRIBUTE_POSITION, "vPosition");

	glLinkProgram(g_ShaderProgramObject_Points);

	GLint iShaderLinkStatus = 0;
	iInfoLogLength = 0;
	glGetProgramiv(g_ShaderProgramObject_Points, GL_LINK_STATUS, &iShaderLinkStatus);
	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(g_ShaderProgramObject_Points, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength>0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(g_ShaderProgramObject_Points, GL_INFO_LOG_LENGTH, &written, szInfoLog);
				fprintf_s(g_pFile, "ERROR : Linking Shader Program Objects Failed %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return INIT_LINK_SHADER_PROGRAM_FAILED;

			}
		}
	}
	/* Shader Program End */

	/*Setup Uniforms Start*/
	g_Uniform_Points_Model_Matrix = glGetUniformLocation(g_ShaderProgramObject_Points, "u_model_matrix");
	g_Uniform_Points_Projection_Matrix = glGetUniformLocation(g_ShaderProgramObject_Points, "u_projection_matrix");
	g_Uniform_Points_View_Matrix = glGetUniformLocation(g_ShaderProgramObject_Points, "u_view_matrix");
	/*Setup Uniforms End*/
	glm::vec3 pointVerts[] =
	{
		glm::vec3(0.0f,0.0f,0.0f),
		glm::vec3(20.0f,0.0f,0.0f),
		glm::vec3(40.0f,0.0f,0.0f),
		glm::vec3(60.0f,0.0f,0.0f),
		glm::vec3(80.0f,0.0f,0.0f),
		glm::vec3(100.0f,0.0f,0.0f),
		glm::vec3(120.0f,0.0f,0.0f),
		glm::vec3(140.0f,0.0f,0.0f),
		glm::vec3(160.0f,0.0f,0.0f),
		glm::vec3(180.0f,0.0f,0.0f),
		glm::vec3(200.0f,0.0f,0.0f),
		glm::vec3(220.0f,0.0f,0.0f),
		glm::vec3(240.0f,0.0f,0.0f),
		glm::vec3(260.0f,0.0f,0.0f),
		glm::vec3(280.0f,0.0f,0.0f),
		glm::vec3(300.0f,0.0f,0.0f),
		glm::vec3(320.0f,0.0f,0.0f),
		glm::vec3(340.0f,0.0f,0.0f),
		glm::vec3(360.0f,0.0f,0.0f),
		glm::vec3(380.0f,0.0f,0.0f),
		glm::vec3(400.0f,0.0f,0.0f),
		glm::vec3(420.0f,0.0f,0.0f),
		glm::vec3(440.0f,0.0f,0.0f),
		glm::vec3(460.0f,0.0f,0.0f),
		glm::vec3(480.0f,0.0f,0.0f),
		glm::vec3(500.0f,0.0f,0.0f),
		glm::vec3(520.0f,0.0f,0.0f),
		glm::vec3(540.0f,0.0f,0.0f),
		glm::vec3(560.0f,0.0f,0.0f),
		glm::vec3(580.0f,0.0f,0.0f),
		glm::vec3(600.0f,0.0f,0.0f),
		glm::vec3(620.0f,0.0f,0.0f),
		glm::vec3(640.0f,0.0f,0.0f),
		glm::vec3(660.0f,0.0f,0.0f),
		glm::vec3(680.0f,0.0f,0.0f),
		glm::vec3(700.0f,0.0f,0.0f),
		glm::vec3(720.0f,0.0f,0.0f),
		glm::vec3(740.0f,0.0f,0.0f),
		glm::vec3(760.0f,0.0f,0.0f),
		glm::vec3(780.0f,0.0f,0.0f),
		glm::vec3(800.0f,0.0f,0.0f),
		glm::vec3(820.0f,0.0f,0.0f),
		glm::vec3(840.0f,0.0f,0.0f),
		glm::vec3(860.0f,0.0f,0.0f),
		glm::vec3(880.0f,0.0f,0.0f),
		glm::vec3(900.0f,0.0f,0.0f),
		glm::vec3(920.0f,0.0f,0.0f),
		glm::vec3(940.0f,0.0f,0.0f),
		glm::vec3(960.0f,0.0f,0.0f),
		glm::vec3(980.0f,0.0f,0.0f),
		glm::vec3(1000.0f,0.0f,0.0f),


	};



	for (size_t i = 0; i < 51; i++)
	{
		pointVerts[i].y = pTerrainInst->GetHeightAt(pointVerts[i]);
	}


	glGenVertexArrays(1, &g_VertexArrayObject_Points);//VAO
	glBindVertexArray(g_VertexArrayObject_Points);

	glGenBuffers(1, &g_VertexBufferObject_Position_Points);// vbo position
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_Position_Points);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointVerts), pointVerts, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	return INIT_ALL_OK;
}


void RenderPoints(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 ProjMatrix)
{
	/*glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);*/

	glUseProgram(g_ShaderProgramObject_Points);

	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
	//rotationMatrix = glm::rotate(rotationMatrix, glm::radians(g_Angle_Pyramid), glm::vec3(1.0, 1.0, 1.0));
	//modelMatrix = modelMatrix * rotationMatrix;

	//viewMatrix = triangleCamera.GetViewMatrix();
	glUniformMatrix4fv(g_Uniform_Points_Model_Matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(g_Uniform_Points_View_Matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(g_Uniform_Points_Projection_Matrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix));

	glDisable(GL_CULL_FACE);
	glPointSize(30.0);
	glBindVertexArray(g_VertexArrayObject_Points);
	glDrawArrays(GL_POINTS, 0, 51);
	glBindVertexArray(0);

	glUseProgram(0);

	glEnable(GL_CULL_FACE);
}



void CleanUpPoints()
{


	if (g_VertexBufferObject_Position_Points)
	{
		glDeleteBuffers(1, &g_VertexBufferObject_Position_Points);
		g_VertexBufferObject_Position_Points = NULL;
	}

	if (g_VertexArrayObject_Points)
	{
		glDeleteVertexArrays(1, &g_VertexArrayObject_Points);
		g_VertexArrayObject_Points = NULL;
	}

	glUseProgram(0);

	if (g_ShaderProgramObject_Points)
	{
		GLsizei iShaderCount;
		GLsizei iShaderNumber;
		glUseProgram(g_ShaderProgramObject_Points);
		glGetProgramiv(g_ShaderProgramObject_Points, GL_ATTACHED_SHADERS, &iShaderCount);
		GLuint *pShaders = (GLuint*)calloc(iShaderCount, sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(g_ShaderProgramObject_Points, iShaderCount, &iShaderCount, pShaders);
			for (iShaderNumber = 0; iShaderNumber < iShaderCount; iShaderNumber++)
			{
				glDetachShader(g_ShaderProgramObject_Points, pShaders[iShaderNumber]);
				glDeleteShader(pShaders[iShaderNumber]);
				pShaders[iShaderNumber] = 0;
			}
			free(pShaders);
			pShaders = NULL;
		}
		glDeleteProgram(g_ShaderProgramObject_Points);
		g_ShaderProgramObject_Points = NULL;
		glUseProgram(0);
	}
}


