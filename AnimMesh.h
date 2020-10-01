#pragma once


#include"CommonHeader.h"

#include<glm\glm.hpp>
#include<assimp\Importer.hpp>

#include<gl\glew.h>
#include<gl\GL.h>

#define NUM_BONES_PER_VEREX 4

struct smVertex
{
	glm::vec3 v3Position;
	glm::vec3 v3Normal;
	glm::vec2 v2TexCoords;
	glm::vec3 v3Tangent;
	glm::vec3 v3Bitangent;
};


struct smBoneMatrix
{
	//Comment :  here is glm::vec4
	aiMatrix4x4 aimat44_Offset_Matrix;//offset_matrix
	aiMatrix4x4 aimat44_Final_World_Transform;//final_world_transform
};

struct smTexture
{
	GLuint iTextureID;
	std::string strTextureType;
	//Comment :  code here string
	aiString aistrPathToTexture;
};


struct smMaterial
{
	std::string material_name;
	//float Ka[4];
	float Value[3];
	float Kd[3];
	float Ks[3];
	float Ni;

	std::string type;
};


struct VertexBoneData
{
	//4 bone ids for each vertex and 4 weights for each vertex
	UINT ids[NUM_BONES_PER_VEREX];
	float weights[NUM_BONES_PER_VEREX];

	VertexBoneData()
	{
		memset(ids, 0, sizeof(ids));
		memset(weights, 0, sizeof(weights));
	}

	void addBoneData(UINT bone_id, float weight);
};

class AnimMesh
{
private:
	//mesh data
	std::vector<smVertex> vertices;
	std::vector<GLuint> indices;
	std::vector<smTexture> textures;
	std::vector<smMaterial> materials;
	std::vector<VertexBoneData> bones_id_weights_for_each_vertex;

	//buffers
	GLuint VAO;
	GLuint VBO_Vertices;
	GLuint VBO_bones;
	GLuint EBO_indices;

	//init buffers
	void SetupMesh();
public:
	AnimMesh(std::vector<smVertex> vertex, std::vector<GLuint> ind, std::vector<smTexture> texture, std::vector<smMaterial> materials, std::vector<VertexBoneData> bone_id_weights);

	AnimMesh() {};

	~AnimMesh();

	void MeshCleanUP();

	//Render Mesh
	void Draw(GLuint shaders_program);
	void DrawInsctanced(GLuint shaders_program, GLint numOfInstances);
};



