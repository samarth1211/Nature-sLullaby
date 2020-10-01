#pragma once
//Various models Main


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#define TREE_MODEL 20
#define TREE_MODEL_ACTUAL_DRAW 3

class TreeModel
{
private:

	StaticModelLoadingModel Tree1, Tree2, Tree3, Tree4;
	//uniforms for model Loading
	GLuint 	diffuseSampler_Uniform_staticModel, specularSampler_Uniform_staticModel, normalSampler_Uniform_staticModel, heightSampler_Uniform_staticModel
		, mUniform_staticModel, vUniform_staticModel, pUniform_staticModel, gBonesUniform_staticModel;

	GLuint LDUniform_staticModel, KDUniform_staticModel,
		LAUniform_staticModel, KAUniform_staticModel,
		KSUniform_staticModel, LSUniform_staticModel,
		LightPositionUniform_staticModel,
		MaterialShininessUniform_staticModel;


	GLchar *vertexShaderSourceCode_staticModel;
	GLchar *fragmentShaderSourceCode_staticModel;
	GLuint gVertexShaderObject_staticModel;
	GLuint gFragmentShaderObject_staticModel;
	GLuint gShaderProgramObject_staticModel;

	//Lights Variable
	float lightAmbient_staticModel[4] = { 0.0f,0.0f,0.0f,1.0f };
	float lightDiffused_staticModel[4] = { 1.0f,1.0f,1.0f,1.0f };
	float lightSpecular_staticModel[4] = { 1.0f,1.0f,1.0f,1.0f };
	float lightPosition_staticModel[4] = { 100.0f,1000.0f,-2000.0f,1.0f };

	float materialAmbient_staticModel[4] = { 0.0f,0.0f,0.0f,1.0f };
	float materialDiffused_staticModel[4] = { 1.0f,1.0f,1.0f,1.0f };
	float materialSpecular_staticModel[4] = { 1.0f,1.0f,1.0f,1.0f };
	float materialShininess_staticModel = 250.0f;


	/*float modelTreePos[TREE_MODEL][3] = 
	{ 
		{ 0,0,0 },{ 20,0,0 },{ -60,0,0 },
		{ 80,20,0 },{ 30,20,0 },{ -40,20,0 } ,
		{ 80,40,0 },{ -30,40,0 },{ -80,40,0 } ,
		{ 120,40,0 } 
	};*/

	glm::vec3 modelTreePos[TREE_MODEL] =
	{
		glm::vec3(0.0f,0.0f,0.0f),
		glm::vec3(20.0f,0.0f,0.0f),
		glm::vec3(-200.0f,0.0f,-50.0f),
		glm::vec3(80.0f,20.0f,0.0f),
		glm::vec3(30.0f,20.0f,0.0f),
		glm::vec3(-40.0f,20.0f,0.0f),
		glm::vec3(80.0f,40.0f,0.0f),
		glm::vec3(-30.0f,40.0f,0.0f),
		glm::vec3(-80.0f,0.0f,-40.0f),
		glm::vec3(120.0f,0.0f,40.0f),
	};

	GLuint m_Tree_location[TREE_MODEL];

public:
	//Write Vertex Shader Object
	TreeModel() 
	{
		vertexShaderSourceCode_staticModel = "#version 420 core" \
			"\n" \
			"layout(location = 0) in vec4 vPosition;" \
			"layout(location = 1) in vec2 vTexCoord;" \
			"layout(location = 2) in vec3 vNormal;" \
			"layout(location = 3) in vec3 vTangent;" \
			"layout(location = 4) in vec3 vBiTangent;" \
			"uniform sampler2D u_HeightSampler;" \
			"uniform vec3 offset[100];" \
			"uniform mat4 u_m_matrix;" \
			"uniform mat4 u_v_matrix;" \
			"uniform mat4 u_p_matrix;" \
			"uniform int u_LKeyIsPressed;" \
			"out vec2 out_texCoord;" \
			"out vec3 outNormal;" \
			"out vec3 tNormVertexShader;" \
			"out vec4 eye_coordinatesVertexShader;" \
			"out VS_OUT{" \
			"vec3 FragmentPosition;" \
			"vec3 Normal;" \
			"vec2 TexCoords;" \
			"vec3 TangentLightPos;" \
			"vec3 TangentCameraPos;" \
			"vec3 TangentFragPos;" \
			"} vs_out;" \

			"void main(void)" \
			"{" \
			"vs_out.FragmentPosition = vec3(u_m_matrix * vPosition);\n" \
			"vs_out.Normal = transpose(inverse(mat3(u_m_matrix))) * vNormal;" \
			"vs_out.TexCoords = vTexCoord;" \
			"mat3 normalMatrix = transpose(inverse(mat3(u_m_matrix)));" \
			"vec3 T = normalize(normalMatrix * vTangent);" \
			"vec3 N = normalize(normalMatrix * vNormal);" \
			"vec3 B = normalize(normalMatrix * cross(T, N));\n" \
			"mat3 TBN = transpose(mat3(T, B, N));" \
			"vs_out.TangentLightPos = TBN *vec3(1.0,1.0,1.0);" \
			"vs_out.TangentCameraPos = TBN*vec3(1.0,1.0,1.0) ;" \
			"vs_out.TangentFragPos = TBN * vs_out.FragmentPosition;" \
			"vec4 dummyPosition = vPosition+vec4(offset[gl_InstanceID],1.0);" \
			
			"float tempHeight = offset[gl_InstanceID].y;"	\
			//"dummyPosition = vPosition + vec4(0.0,tempHeight,0.0,0.0);"	
			"eye_coordinatesVertexShader=u_v_matrix*u_m_matrix*dummyPosition;" \
			"tNormVertexShader=mat3(u_v_matrix*u_m_matrix)*vNormal;" \
			"gl_Position = u_p_matrix*u_v_matrix*u_m_matrix* dummyPosition;" \
			"out_texCoord=vTexCoord;" \
			"}";

		//Write Vertex Shader Object
		fragmentShaderSourceCode_staticModel =
			"#version 420 core" \
			"\n" \
			"in vec2 out_texCoord;" \
			"in vec3 outNormal;" \
			"in vec3 tNormVertexShader;" \
			"in vec4 eye_coordinatesVertexShader;" \
			"uniform vec3 u_Ld;" \
			"uniform vec3 u_Kd;" \
			"uniform vec3 u_Ls;" \
			"uniform vec3 u_Ks;" \
			"uniform vec3 u_La;" \
			"uniform vec3 u_Ka;" \
			"uniform vec4 u_Light_Position;" \
			"uniform float u_MaterialShininess;" \
			"uniform sampler2D u_DiffuseSampler;" \
			"uniform sampler2D u_HeightSampler;" \
			"uniform sampler2D u_SpecularSampler;" \
			"uniform sampler2D u_NormalSampler;" \
			"out vec4 FragColor;" \
			"in VS_OUT{" \
			"vec3 FragmentPosition;" \
			"vec3 Normal;" \
			"vec2 TexCoords;" \
			"vec3 TangentLightPos;" \
			"vec3 TangentCameraPos;" \
			"vec3 TangentFragPos;" \
			"}fs_in;" \
			"void main(void)" \
			"{" \
			"vec3 normal, lightDirection, viewDirection;" \
			"normal = texture(u_NormalSampler, fs_in.TexCoords).rgb;" \
			"normal = normalize(normal * 2.0 - 1.0);" \
			"lightDirection = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);" \
			"viewDirection = normalize(fs_in.TangentCameraPos - fs_in.TangentFragPos);" \
			"vec3 texColor = texture(u_DiffuseSampler, fs_in.TexCoords).rgb;" \
			"float ambientStrength = 0.1;" \
			"vec3 ambient = ambientStrength * texColor;" \
			"float diff = max(dot(normal, vec3(1.0,1.0,1.0)), 0.0);" \
			"vec3 diffuse = diff * texColor;" \
			"vec3 I = normalize(fs_in.FragmentPosition - vec3(1.0,1.0,1.0));" \
			"vec3 R = reflect(I, normalize(normal));" \
			"float reflectIntensity = texture(u_SpecularSampler, fs_in.TexCoords).r;" \
			"float specularStrength = 0.2f;" \
			"float spec = 0.0f;" \
			"vec3 halfwayDirection = normalize(lightDirection + viewDirection);" \
			"spec = pow(max(dot(normal, halfwayDirection), 0.0f), /*1*/32);" \
			"vec3 specular = vec3(specularStrength) * spec * texture(u_SpecularSampler, fs_in.TexCoords).rgb;" \
			"vec3 diffuse_color = texture(u_DiffuseSampler, fs_in.TexCoords).rgb;" \
			"FragColor = vec4(ambient+specular+diffuse_color , 1.0f);" \
			"if(FragColor.a <0.5)" \
			"discard;" \
			"}";



	}


/*
TreeModel() {
	//Write Vertex Shader Object
		vertexShaderSourceCode_staticModel = "#version 420 core" \
		"\n" \
		"layout(location = 0) in vec4 vPosition;" \
		"layout(location = 1) in vec2 vTexCoord;" \
		"layout(location = 2) in vec3 vNormal;" \
		"layout(location = 3) in ivec4 s_vIDs;" \
		"layout(location = 4) in vec4 s_vWeights;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform int u_LKeyIsPressed;" \
		"out vec2 out_texCoord;" \
		"out vec3 outNormal;" \
		"out vec3 tNormVertexShader;" \
		"out vec4 eye_coordinatesVertexShader;" \
		"void main(void)" \
		"{" \
		"eye_coordinatesVertexShader=u_v_matrix*u_m_matrix*vPosition;" \
		"tNormVertexShader=mat3(u_v_matrix*u_m_matrix)*vNormal;" \
		"gl_Position = u_p_matrix*u_v_matrix*u_m_matrix* vPosition;" \
		"out_texCoord=vTexCoord;" \
		"}";


		fragmentShaderSourceCode_staticModel =
		"#version 420 core" \
		"\n" \
		"in vec2 out_texCoord;" \
		"in vec3 outNormal;" \
		"in vec3 tNormVertexShader;" \
		"in vec4 eye_coordinatesVertexShader;" \
		"uniform vec3 u_Ld;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ls;" \
		"uniform vec3 u_Ks;" \
		"uniform vec3 u_La;" \
		"uniform vec3 u_Ka;" \
		"uniform vec4 u_Light_Position;" \
		"uniform float u_MaterialShininess;" \
			"uniform sampler2D u_DiffuseSampler;" \
			"out vec4 FragColor;"
		"void main(void)" \
		"{" \
		"vec3 tNorm=normalize(tNormVertexShader);"
		"vec3 lightDirection=normalize(vec3(u_Light_Position-eye_coordinatesVertexShader));" \
		"float tndotld=max(dot(lightDirection,tNorm),0.0);" \
		"vec3 ReflectionVector=reflect(-lightDirection,tNorm);" \
		"vec3 viewerVector=normalize(vec3(-eye_coordinatesVertexShader.xyz));" \
		"vec3 ambient=u_La*u_Ka;" \
		"vec3 diffused=u_Ld*u_Kd*tndotld;" \
		"vec3 specular=u_Ls*u_Ks*pow(max(dot(ReflectionVector,viewerVector),0.0),u_MaterialShininess);" \
		"vec3 phong_ads_light = ambient+diffused+specular;" \
		"vec4 lightColor=vec4(phong_ads_light,1.0);" \
		"FragColor=vec4(phong_ads_light,1.0);"
		"}";

}
*/

	bool Tree_Init(CTerrain *pTerrain)
	{
		//Vertex Shader
		gVertexShaderObject_staticModel = glCreateShader(GL_VERTEX_SHADER);

		//Write Vertex Shader Object
		glShaderSource(gVertexShaderObject_staticModel, 1, (const GLchar **)&vertexShaderSourceCode_staticModel, NULL);
		glCompileShader(gVertexShaderObject_staticModel);
		//Error Check
		GLint iShaderComileStatus = 0;
		GLint iInfoLogLength = 0;
		GLchar* szLogInfo = NULL;

		glGetShaderiv(gVertexShaderObject_staticModel, GL_COMPILE_STATUS, &iShaderComileStatus);
		if (iShaderComileStatus == GL_FALSE)
		{
			glGetShaderiv(gVertexShaderObject_staticModel, GL_INFO_LOG_LENGTH, &iInfoLogLength);
			if (iInfoLogLength > 0)
			{
				szLogInfo = (GLchar *)malloc(iInfoLogLength);
				if (szLogInfo != NULL)
				{
					GLsizei written;
					glGetShaderInfoLog(gVertexShaderObject_staticModel, iInfoLogLength, &written, szLogInfo);
					fprintf_s(g_pFile, "ERROR : TREE MODEL Vertex Shader Compilation Log : %s \n", szLogInfo);
					free(szLogInfo);
					return false;
				}
			}
		}

		//Fragment Shader
		gFragmentShaderObject_staticModel = glCreateShader(GL_FRAGMENT_SHADER);

		//Write Vertex Shader Object
		glShaderSource(gFragmentShaderObject_staticModel, 1, (const GLchar **)&fragmentShaderSourceCode_staticModel, NULL);
		glCompileShader(gFragmentShaderObject_staticModel);
		//Error Check
		iShaderComileStatus = 0;
		iInfoLogLength = 0;
		szLogInfo = NULL;

		glGetShaderiv(gFragmentShaderObject_staticModel, GL_COMPILE_STATUS, &iShaderComileStatus);
		if (iShaderComileStatus == GL_FALSE)
		{
			glGetShaderiv(gFragmentShaderObject_staticModel, GL_INFO_LOG_LENGTH, &iInfoLogLength);
			if (iInfoLogLength > 0)
			{
				szLogInfo = (GLchar *)malloc(iInfoLogLength);
				if (szLogInfo != NULL)
				{
					GLsizei written;
					glGetShaderInfoLog(gFragmentShaderObject_staticModel, iInfoLogLength, &written, szLogInfo);
					fprintf_s(g_pFile, "ERROR : Fragment Shader Compilation Log : %s \n", szLogInfo);
					free(szLogInfo);
					exit(0);
				}
			}
		}

		//Create Program
		gShaderProgramObject_staticModel = glCreateProgram();
		glAttachShader(gShaderProgramObject_staticModel, gVertexShaderObject_staticModel);
		glAttachShader(gShaderProgramObject_staticModel, gFragmentShaderObject_staticModel);


		glLinkProgram(gShaderProgramObject_staticModel);
		GLint iShaderLinkStatus = 0;
		iInfoLogLength = 0;
		szLogInfo = NULL;

		glGetProgramiv(gShaderProgramObject_staticModel, GL_LINK_STATUS, &iShaderLinkStatus);
		if (iShaderLinkStatus == GL_FALSE)
		{
			glGetProgramiv(gShaderProgramObject_staticModel, GL_INFO_LOG_LENGTH, &iInfoLogLength);
			if (iInfoLogLength > 0)
			{
				szLogInfo = (GLchar *)malloc(iInfoLogLength);
				if (szLogInfo != NULL)
				{
					GLsizei written;
					glGetProgramInfoLog(gShaderProgramObject_staticModel, iInfoLogLength, &written, szLogInfo);
					MessageBox(NULL, (LPCWSTR)szLogInfo, TEXT("Linking Shader"), MB_OK);
					free(szLogInfo);
					return false;
				}
			}
		}
		diffuseSampler_Uniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_DiffuseSampler");
		specularSampler_Uniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_SpecularSampler");
		heightSampler_Uniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_HeightSampler");
		normalSampler_Uniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_NormalSampler");
		mUniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_m_matrix");
		vUniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_v_matrix");
		pUniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_p_matrix");

		LDUniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_Ld");
		KDUniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_Kd");
		LAUniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_La");
		KAUniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_Ka");
		LSUniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_Ls");
		KSUniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_Ks");
		MaterialShininessUniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_MaterialShininess");
		LightPositionUniform_staticModel = glGetUniformLocation(gShaderProgramObject_staticModel, "u_Light_Position");

		for (unsigned int i = 0; i < 10; i++) // get location all matrices of bones
		{
			std::string name = "offset[" + std::to_string(i) + "]";// name like in shader
			m_Tree_location[i] = glGetUniformLocation(gShaderProgramObject_staticModel, name.c_str());
		}
		//Initialize the Model
		//model_Static = StaticModelLoadingModel();
		////model_Static.LoadModel("models/SunflowerStatic/10455_Sunflower_v1_max2010_it2.obj");
		////model_Static.LoadModel("models/meshredusedtreemodel/EU43_2_1.obj");
		//model_Static.LoadModel("models/tree/Lemon_Fruit.obj");

		for (int i = 0; i < TREE_MODEL_ACTUAL_DRAW; i++)
		{
			
			//modelTreePos[i].x = 4.482649f + i*10.0f ;
			//modelTreePos[i].y = 24.098759f ;//13.307393f/300.0f;
			//modelTreePos[i].z = 101.507782f + i * 10.0f;
			modelTreePos[i].y = (pTerrain->GetHeightAt(modelTreePos[i]));
			modelTreePos[i].y /= 300.0f;
			//modelTreePos[i].x += -120.0f;
			
			//modelTreePos[i].y = 0.0f;
		}

		Tree1 = StaticModelLoadingModel();
		Tree1.LoadModel("models/Flower01/untitled.obj");
		
		/*Tree2 = StaticModelLoadingModel();
		Tree2.LoadModel("models/tree/Tree03/chestnut.obj");
		*/
		/*Tree3 = StaticModelLoadingModel();
		Tree3.LoadModel("models/tree/Tree05/autumnMaple.obj");*/
		
		/*Tree4 = StaticModelLoadingModel();
		Tree4.LoadModel("models/Modl/Flower/stem2.obj");*/

		glEnable(GL_TEXTURE_2D);
		return true;
	}
	
	void Tree_UnInitialize()
	{
		Tree1.ClearModel();
		Tree2.ClearModel();
		Tree3.ClearModel();
		Tree4.ClearModel();

		GLsizei shaderCount;
		GLsizei shaderNumber;
		if (gShaderProgramObject_staticModel)
		{
		glUseProgram(gShaderProgramObject_staticModel);

		glGetProgramiv(gShaderProgramObject_staticModel,GL_ATTACHED_SHADERS,&shaderCount);
		GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint)* shaderCount);
		if (pShaders)
		{
		glGetAttachedShaders(gShaderProgramObject_staticModel,shaderCount,&shaderCount,pShaders);
		for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
		{
		glDetachShader(gShaderProgramObject_staticModel, pShaders[shaderNumber]);
		glDeleteShader(pShaders[shaderNumber]);
		pShaders[shaderNumber] = 0;
		}
		free(pShaders);
		}
		glDeleteProgram(gShaderProgramObject_staticModel);
		gShaderProgramObject_staticModel = 0;
		glUseProgram(0);
		}
		
	}


	void Tree1_Display(glm::mat4 modelMatrix1, glm::mat4 viewMatrix1, glm::mat4 ProjMatrix)
	{
		glUseProgram(gShaderProgramObject_staticModel);

		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 translationMatrix;
		glm::mat4 scaleMatrix;
		glm::mat4 rotationMatrix;
		modelMatrix = glm::mat4();
		viewMatrix = viewMatrix1;

		translationMatrix = glm::mat4();
		scaleMatrix = glm::mat4();
		rotationMatrix = glm::mat4();
		
		//For tree
		//translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 30.0f, 0.0f));
		//translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));
		//modelMatrix1 = modelMatrix1*translationMatrix;
		
		//For sunflower
		scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(50.0f, 50.0f, 50.0f));
		modelMatrix1 = modelMatrix1*scaleMatrix;
		

		glUniformMatrix4fv(mUniform_staticModel,
			1,
			GL_FALSE,
			glm::value_ptr(modelMatrix1));
		glUniformMatrix4fv(vUniform_staticModel,
			1,
			GL_FALSE,
			glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(pUniform_staticModel,
			1,
			GL_FALSE,
			glm::value_ptr(ProjMatrix));


		glUniform3fv(LDUniform_staticModel, 1, (GLfloat*)lightDiffused_staticModel);
		glUniform3fv(KDUniform_staticModel, 1, (GLfloat*)materialDiffused_staticModel);
		glUniform3fv(LAUniform_staticModel, 1, (GLfloat*)lightAmbient_staticModel);
		glUniform3fv(KAUniform_staticModel, 1, (GLfloat*)materialAmbient_staticModel);
		glUniform3fv(LSUniform_staticModel, 1, (GLfloat*)lightSpecular_staticModel);
		glUniform3fv(KSUniform_staticModel, 1, (GLfloat*)materialSpecular_staticModel);

		glUniform1f(MaterialShininessUniform_staticModel, materialShininess_staticModel);
		glUniform4fv(LightPositionUniform_staticModel, 1, (GLfloat*)lightPosition_staticModel);


		for (unsigned int i = 0; i < TREE_MODEL_ACTUAL_DRAW; i++)
		{
			glUniform3f(m_Tree_location[i], modelTreePos[i].x, modelTreePos[i].y, modelTreePos[i].z);
			//glUniform3fv(m_Tree_location[i], 1, glm::value_ptr(modelTreePos[i]));
		}

		glUniform1i(diffuseSampler_Uniform_staticModel, 0);
		glUniform1i(normalSampler_Uniform_staticModel, 1);
		glUniform1i(specularSampler_Uniform_staticModel, 2);
		glUniform1i(heightSampler_Uniform_staticModel, 3);

		//Render Model
		Tree1.RenderModel(TREE_MODEL_ACTUAL_DRAW,gShaderProgramObject_staticModel);

		glUseProgram(0);
	}


//	void Tree2_Display(glm::mat4 modelMatrix1, glm::mat4 viewMatrix1, glm::mat4 ProjMatrix)
//	{
//		glUseProgram(gShaderProgramObject_staticModel);
//
//		glm::mat4 translationMatrix;
//		glm::mat4 scaleMatrix;
//		glm::mat4 rotationMatrix;
//	
//		translationMatrix = glm::mat4();
//		scaleMatrix = glm::mat4();
//		rotationMatrix = glm::mat4();
//		
//		//For tree
//		//translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 10.0f, -3.0f));
//		translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));
//		modelMatrix1 = modelMatrix1*translationMatrix;
//
//		//For sunflower
//		scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(20.0f, 20.0f, 20.0f));
//		modelMatrix1 = modelMatrix1*scaleMatrix;
//		
//		glUniformMatrix4fv(mUniform_staticModel,
//			1,
//			GL_FALSE,
//			glm::value_ptr(modelMatrix1));
//		glUniformMatrix4fv(vUniform_staticModel,
//			1,
//			GL_FALSE,
//			glm::value_ptr(viewMatrix1));
//		glUniformMatrix4fv(pUniform_staticModel,
//			1,
//			GL_FALSE,
//			glm::value_ptr(ProjMatrix));
//
//
//		glUniform3fv(LDUniform_staticModel, 1, (GLfloat*)lightDiffused_staticModel);
//		glUniform3fv(KDUniform_staticModel, 1, (GLfloat*)materialDiffused_staticModel);
//		glUniform3fv(LAUniform_staticModel, 1, (GLfloat*)lightAmbient_staticModel);
//		glUniform3fv(KAUniform_staticModel, 1, (GLfloat*)materialAmbient_staticModel);
//		glUniform3fv(LSUniform_staticModel, 1, (GLfloat*)lightSpecular_staticModel);
//		glUniform3fv(KSUniform_staticModel, 1, (GLfloat*)materialSpecular_staticModel);
//
//		glUniform1f(MaterialShininessUniform_staticModel, materialShininess_staticModel);
//		glUniform4fv(LightPositionUniform_staticModel, 1, (GLfloat*)lightPosition_staticModel);
//
//
//		for (unsigned int i = 0; i < TREE_MODEL_ACTUAL_DRAW; i++)
//		{
//			glUniform3f(m_Tree_location[i], modelTreePos[i].x, modelTreePos[i].y, modelTreePos[i].z);
//			//glUniform3fv(m_Tree_location[i], 1, glm::value_ptr(modelTreePos[i]));
//		}
//
//		glUniform1i(diffuseSampler_Uniform_staticModel, 0);
//		glUniform1i(normalSampler_Uniform_staticModel, 1);
//		glUniform1i(specularSampler_Uniform_staticModel, 2);
//		glUniform1i(heightSampler_Uniform_staticModel, 3);
//
//		//Render Model
//		Tree2.RenderModel(TREE_MODEL_ACTUAL_DRAW, gShaderProgramObject_staticModel);
//
//		glUseProgram(0);
//	}
//
//
//	void Tree3_Display(glm::mat4 modelMatrix1, glm::mat4 viewMatrix1, glm::mat4 ProjMatrix)
//	{
//		glUseProgram(gShaderProgramObject_staticModel);
//
//		glm::mat4 modelMatrix;
//		glm::mat4 viewMatrix;
//		glm::mat4 translationMatrix;
//		glm::mat4 scaleMatrix;
//		glm::mat4 rotationMatrix;
//		modelMatrix = glm::mat4();
//		viewMatrix = viewMatrix1;
//		translationMatrix = glm::mat4();
//		scaleMatrix = glm::mat4();
//		rotationMatrix = glm::mat4();
//		//For sunflower
//		/*	translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 500.0f,-500.0f));
//		modelMatrix1 = modelMatrix1*translationMatrix;*/
//
//		//For tree
//		//translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 10.0f, 0.0f));
//		translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));
//		modelMatrix1 = modelMatrix1*translationMatrix;
//
//		/*rotationMatrix = glm::rotate(rotationMatrix, glm::radians(100.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//		modelMatrix1 = modelMatrix1*rotationMatrix;*/
//		//For sunflower
//		scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(20.0f, 20.0f, 20.0f));
//		modelMatrix1 = modelMatrix1*scaleMatrix;
//		/*
//		//For tree
//		scaleMatrix = glm::scale(glm::mat4(1.0),glm::vec3(9.0f, 9.0f, 9.0f));
//		modelMatrix1 = modelMatrix1*scaleMatrix;
//		*/
//
//		glUniformMatrix4fv(mUniform_staticModel,
//			1,
//			GL_FALSE,
//			glm::value_ptr(modelMatrix1));
//		glUniformMatrix4fv(vUniform_staticModel,
//			1,
//			GL_FALSE,
//			glm::value_ptr(viewMatrix));
//		glUniformMatrix4fv(pUniform_staticModel,
//			1,
//			GL_FALSE,
//			glm::value_ptr(ProjMatrix));
//
//
//		glUniform3fv(LDUniform_staticModel, 1, (GLfloat*)lightDiffused_staticModel);
//		glUniform3fv(KDUniform_staticModel, 1, (GLfloat*)materialDiffused_staticModel);
//		glUniform3fv(LAUniform_staticModel, 1, (GLfloat*)lightAmbient_staticModel);
//		glUniform3fv(KAUniform_staticModel, 1, (GLfloat*)materialAmbient_staticModel);
//		glUniform3fv(LSUniform_staticModel, 1, (GLfloat*)lightSpecular_staticModel);
//		glUniform3fv(KSUniform_staticModel, 1, (GLfloat*)materialSpecular_staticModel);
//
//		glUniform1f(MaterialShininessUniform_staticModel, materialShininess_staticModel);
//		glUniform4fv(LightPositionUniform_staticModel, 1, (GLfloat*)lightPosition_staticModel);
//
//
//		for (unsigned int i = 0; i < TREE_MODEL_ACTUAL_DRAW; i++)
//		{
//			glUniform3f(m_Tree_location[i], modelTreePos[i].x, modelTreePos[i].y, modelTreePos[i].z);
//			//glUniform3fv(m_Tree_location[i], 1, glm::value_ptr(modelTreePos[i]));
//		}
//
//		glUniform1i(diffuseSampler_Uniform_staticModel, 0);
//		glUniform1i(normalSampler_Uniform_staticModel, 1);
//		glUniform1i(specularSampler_Uniform_staticModel, 2);
//		glUniform1i(heightSampler_Uniform_staticModel, 3);
//
//		//Render Model
//		Tree3.RenderModel(TREE_MODEL_ACTUAL_DRAW,gShaderProgramObject_staticModel);
//
//		glUseProgram(0);
//	}
//	
//
//	void Tree4_Display(glm::mat4 modelMatrix1, glm::mat4 viewMatrix1, glm::mat4 ProjMatrix)
//	{
//		glUseProgram(gShaderProgramObject_staticModel);
//
//		glm::mat4 modelMatrix;
//		glm::mat4 viewMatrix;
//		glm::mat4 translationMatrix;
//		glm::mat4 scaleMatrix;
//		glm::mat4 rotationMatrix;
//		modelMatrix = glm::mat4();
//		viewMatrix = viewMatrix1;
//		translationMatrix = glm::mat4();
//		scaleMatrix = glm::mat4();
//		rotationMatrix = glm::mat4();
//		//For sunflower
//		/*	translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 500.0f,-500.0f));
//		modelMatrix1 = modelMatrix1*translationMatrix;*/
//
//		//For tree
//		translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));
//		//translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, -3.0f));
//		modelMatrix = modelMatrix*translationMatrix;
///*
//		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(100.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//		modelMatrix1 = modelMatrix1*rotationMatrix;
//	*/	//For sunflower
//		//scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(0.0f, 20.0f, 20.0f));
//		//modelMatrix = modelMatrix*scaleMatrix;
//		///*
//		//For tree
//		//scaleMatrix = glm::scale(glm::mat4(1.0),glm::vec3(9.0f, 9.0f, 9.0f));
//		//modelMatrix1 = modelMatrix1*scaleMatrix;
//		
//
//		glUniformMatrix4fv(mUniform_staticModel,
//			1,
//			GL_FALSE,
//			glm::value_ptr(modelMatrix));
//		glUniformMatrix4fv(vUniform_staticModel,
//			1,
//			GL_FALSE,
//			glm::value_ptr(glm::mat4()));
//		glUniformMatrix4fv(pUniform_staticModel,
//			1,
//			GL_FALSE,
//			glm::value_ptr(ProjMatrix));
//
//
//
//		for (unsigned int i = 0; i < TREE_MODEL_ACTUAL_DRAW; i++)
//		{
//			glUniform3f(m_Tree_location[i], modelTreePos[i].x, modelTreePos[i].y, modelTreePos[i].z);
//			//glUniform3fv(m_Tree_location[i], 1, glm::value_ptr(modelTreePos[i]));
//		}
//
//		glUniform1i(diffuseSampler_Uniform_staticModel, 0);
//		glUniform1i(normalSampler_Uniform_staticModel, 1);
//		glUniform1i(specularSampler_Uniform_staticModel, 2);
//		glUniform1i(heightSampler_Uniform_staticModel, 3);
//
//		//Render Model
//		Tree4.RenderModel(TREE_MODEL_ACTUAL_DRAW,gShaderProgramObject_staticModel);
//
//		glUseProgram(0);
//	}
	
};

