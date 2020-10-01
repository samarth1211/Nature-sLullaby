#pragma once

#include"CommonHeader.h"

class CTerrain
{
private:
	// Shaders
	GLuint iShaderProgramObject_Terrain;
	// All Vertex Buffers
	GLuint iVertexArrayObject_Terrain;
	GLuint iVertexBufferObject_Position_Terrain;
	GLuint iVertexBufferObject_Color_Terrain;
	GLuint iVertexBufferObject_Normal_Terrain;
	GLuint iVertexBufferObject_Texture_Terrain;
	GLuint iVertexBufferObject_Tangent_Terrain;
	GLuint iVertexBufferObject_Idices_Terrain;
	// Uniforms
	GLint iUniform_Model_Matrix_Terrain;
	GLint iUniform_View_Matrix_Terrain;
	GLint iUniform_Projection_Matrix_Terrain;

	GLint iUniform_ScaleBias_Matrix_Terrain;
	GLint iUniform_Light_View_Matrix_Terrain;
	GLint iUniform_Light_Projection_Matrix_Terrain;

	GLint iUniform_xPixelOffset;
	GLint iUniform_yPixelOffset;
	// sampler
	GLuint iUniform_Texture0_BG_Sampler;
	GLuint iUniform_Texture1_R_Sampler;
	GLuint iUniform_Texture2_G_Sampler;
	GLuint iUniform_Texture3_B_Sampler;
	GLuint iUniform_Texture4_BlendMap_Sampler;
	GLuint iUniform_Texture4_NormalMap_Sampler;
	GLuint iUniform_Texture5_DepthMap_Sampler;

	GLfloat BlockScale;
	GLfloat HeightScale;

	LoadTGAFile tgaLoader;

	std::vector<glm::vec3> vTerrainPositionBuffer;
	std::vector<glm::vec4> vTerrainColorBuffer;
	std::vector<glm::vec3> vTerrainNormalBuffer;
	std::vector<glm::vec3> vTerrainTangentBuffer;
	std::vector<glm::vec2> vTerrainTexCoordBuffer;
	std::vector<GLuint> vTerrainIndexBuffer;

	FILE *terrainLogFIle;

	glm::uvec2	HeightMapDimensions;

	/*All Textures*/
	GLuint iTexture_Stone;
	GLuint iTexture_BG;
	GLuint iTexture_R;
	GLuint iTexture_G;
	GLuint iTexture_B;
	GLuint iTexture_BlendMap;
	GLuint iTexture_Normal;


	void GenerateIndexBuffer();
	void GenerateNormals();
	int GetFileLength(std::istream& file);
	float GetHeightValue(const unsigned char*, unsigned char);
	unsigned short GetHeightValueShort(const unsigned char * data, unsigned char numBytes);
	float GetPercentage(float, const float, const float);
	int LoadBMPTexture(TCHAR imageResourceId[], GLuint *Texture);
	

public:

	glm::mat4 m4TerrainPerspectiveProjectionMatrix;
	CTerrain();
	int SetUpTerrain();// Initilaize terrain
	~CTerrain();

	void SetProjection(glm::mat4 newprojection);

	int LoadHeightmap(const std::string& filename, unsigned char bitsPerPixel, unsigned int width, unsigned int height);
	float GetHeightAt(const glm::vec3& position);

	void DeleteTerrain();
	
	void RenderTerrain(glm::mat4 Model, glm::mat4 View, glm::mat4 Projection,GLuint depthTexture);
	void RenderTerrainShadow();


	GLuint GetShaderProgram();


};


