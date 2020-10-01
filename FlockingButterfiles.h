#pragma once


#include "CommonHeader.h"
#include	<IL\il.h>

#define WORKGROUP_SIZE	256
#define NUM_WORKGROUPS  16
#define FLOCK_SIZE		NUM_WORKGROUPS * WORKGROUP_SIZE



class CFlockingButterflies
{

	


private:
	FILE *buffterFliesLogFIle;

	GLuint g_ShaderProgramObject_Particle = 0;
	GLuint g_ComputeProgramObject_Particle = 0;

	GLuint  flock_buffer[2];
	GLuint  flock_render_vao[2];
	GLuint  geometry_buffer;

	// Uniforms
	GLuint g_Uniform_Model_Matrix = 0;
	GLuint g_Uniform_View_Matrix = 0;
	GLuint g_Uniform_Projection_Matrix = 0;

	GLint g_uniform_NumberofRows = 0;
	GLint g_uniform_Offset1 = 0;
	GLint g_uniform_Offset2 = 0;
	GLint g_uniform_blendFactor = 0;

	GLint g_Uniform_CS_closest_allowed_dist = 0;
	GLint g_Uniform_CS_rule1_weight = 0;
	GLint g_Uniform_CS_rule2_weight = 0;
	GLint g_Uniform_CS_rule3_weight = 0;
	GLint g_Uniform_CS_rule4_weight = 0;
	GLint g_Uniform_CS_goal = 0;
	GLint g_Uniform_CS_timeStep = 0;

	GLuint  swap_index = 0;
	// sampler
	GLuint g_uniform_ButterFlySampler = 0;
	GLuint g_TextureButterFly = 0;
	
	// Projection
	glm::mat4 g_PersPectiveProjectionMatrix;

	GLfloat g_AnimTime = 0.0;
	GLfloat g_FlockTime = 0.0;
	bool g_bFullScreen = false;

	// Image information to calculate offsets

	int iNumberOfRows = 5;// change it later on

	int iTextureIndex = 0;
	// size of texture atlas
	int g_iImgRows = 0;
	int g_iImgCols = 0;
	// x and y offset => local at Render()

	//GLfloat g_fTimeValue = 0.0f;

	
public:

	CFlockingButterflies();

	~CFlockingButterflies();


	struct flock_member
	{
		glm::vec3 position;
		unsigned int : 32;// padding
		glm::vec3 velocity;
		unsigned int : 32;// padding
	};

	int InitializeFlockingButterflies();

	glm::vec3 randomValues();

	float random();

	int LoadPNGTexture(GLuint *texture, const char *filename, int *imgRows, int *imgCols);



	void DeleteFlockingButterflies();

	void RenderFlockingButterflies(glm::mat4 Model, glm::mat4 View, glm::mat4 Projection);

	void CFlockingButterflies::Update(void);
	



};


