#pragma once


#include"CommonHeader.h"

//#include <map>

#include<gl\glew.h>
#include<gl\GL.h>

#include"AnimMesh.h"
//#include<assimp\Importer.hpp> //for Assimp::Importer

#include<assimp\scene.h>
#include<assimp\postprocess.h>
#include<glm\gtc\quaternion.hpp>

#define MAX_BONES	100

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);

class AnimModel
{
private:
	Assimp::Importer import;
	const aiScene *scene;
	std::vector<AnimMesh> meshes; //one mesh in one object
	std::string directory; // directory to the resources

	std::map<std::string, unsigned int>m_bone_mapping;
	unsigned int m_num_bones = 0;
	std::vector<smBoneMatrix> m_bone_matrices;
	aiMatrix4x4 m_global_inverse_transform;

	GLuint m_bone_location[MAX_BONES];
	float ticks_per_second = 0.0f;

	void processNode(aiNode *node, const aiScene *scene);
	AnimMesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<smTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string type_name);

	std::vector<smMaterial> loadMaterialColor(aiMaterial* mat, const char* type, int one, int two, std::string typeName);

	int findPosition(float p_animation_time, const aiNodeAnim* p_node_anim);
	int findRotation(float p_animation_time, const aiNodeAnim* p_node_anim);
	int findScaling(float p_animation_time, const aiNodeAnim* p_node_anim);

	const aiNodeAnim* findNodeAnim(const aiAnimation* p_animation, const std::string p_node_name);

	// calculate transform matrix
	aiVector3D calcInterpolatedPosition(float p_animation_time, const aiNodeAnim* p_node_anim);
	aiQuaternion calcInterpolatedRotation(float p_animation_time, const aiNodeAnim* p_node_anim);
	aiVector3D calcInterpolatedScaling(float p_animation_time, const aiNodeAnim* p_node_anim);

	void readNodeHierarchy(float p_animation_time, const aiNode* p_node, const aiMatrix4x4 parent_transform);
	void boneTransform(double time_in_sec, std::vector<aiMatrix4x4>& transforms);

public:
	AnimModel();
	~AnimModel();


	void ModelCleanUp();

	bool IsModelHavingAnimation; // for animation Models
	void initShaders(GLuint shader_program);
	void loadModel(const std::string& path);

	//void update();
	void draw(GLuint shaders_program, bool isAnimated, double dTimeOfAnimation);
	void drawInstanced(GLuint shaders_program, bool isAnimated, double dTimeOfAnimation, GLint numOfInstances);

	void showNodeName(aiNode* node);

	glm::mat4 aiToGlm(aiMatrix4x4 ai_matr);
	aiQuaternion nlerp(aiQuaternion a, aiQuaternion b, float blend);

	// added
	// rotate Head
	glm::quat rotate_head_xz = glm::quat(cos(glm::radians(0.0f)), sin(glm::radians(0.0f)) * glm::vec3(1.0f, 0.0f, 0.0f)); // this quad do nothingggggg!!!!!


};
