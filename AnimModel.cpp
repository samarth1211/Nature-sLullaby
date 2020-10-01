//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include"AnimModel.h"

#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Log File Name "SamModelLogFile.txt"

AnimModel::AnimModel()
{
	scene = nullptr;
}


AnimModel::~AnimModel()
{
	import.FreeScene();
}


void AnimModel::ModelCleanUp()
{
	if (meshes.size() > 0)
	{
		for (size_t i = 0; i < meshes.size(); i++)
		{
			meshes[i].MeshCleanUP();
		}
	}
}

// setting up bone data here
void AnimModel::initShaders(GLuint shader_program)
{
	for (int i = 0; i < MAX_BONES; i++) // get location all matrices of bones
	{
		std::string name = "gBones[" + std::to_string(i) + "]";// name like in shader
		m_bone_location[i] = glGetUniformLocation(shader_program, name.c_str());
	}
}


//double t = 0.1f;
//static float count1 = 0.1f;
void AnimModel::draw(GLuint shaders_program, bool isAnimated, double dTimeOfAnimation)
{
	if (IsModelHavingAnimation)
	{
		std::vector<aiMatrix4x4> transforms;


		static bool firstTime = true;
		//if(firstTime == true)

		boneTransform(dTimeOfAnimation, transforms);

		//boneTransform((double)GetTickCount() / 1000.0f, transforms);

		//boneTransform(1.0f, transforms);
		//boneTransform(0.1f, transforms);
		firstTime = false;
		//if(count1 < 47.0f)
		//boneTransform((double)timer.getTime() / 1000.0f, transforms);
		/*if (count1 < 470.0f)
		boneTransform(count1, transforms);*/
		//boneTransform(t , transforms);

		/*t += 0.1f;
		if (isAnimated)
		count1 = count1 + 1.0f;
		else
		count1 = 0.0f;*/

		// move all matrices for actual model position to shader
		for (unsigned int i = 0; i < transforms.size(); i++)
		{
			glUniformMatrix4fv(m_bone_location[i], 1, GL_TRUE, (const GLfloat*)&transforms[i]);
		}
	}

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shaders_program);
	}
}

void AnimModel::drawInstanced(GLuint shaders_program, bool isAnimated, double dTimeOfAnimation, GLint numOfInstances)
{
	if (IsModelHavingAnimation)
	{
		std::vector<aiMatrix4x4> transforms;
		static bool firstTime = true;
		boneTransform(dTimeOfAnimation, transforms);
		firstTime = false;
		// move all matrices for actual model position to shader
		for (unsigned int i = 0; i < transforms.size(); i++)
		{
			glUniformMatrix4fv(m_bone_location[i], 1, GL_TRUE, (const GLfloat*)&transforms[i]);
		}
	}

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].DrawInsctanced(shaders_program, numOfInstances);
	}
}

void AnimModel::loadModel(const std::string& path)
{
	FILE *pFIle = NULL;

	fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
	fprintf(pFIle, "Enter LoadMyModel\n");
	fclose(pFIle);
	scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
		fprintf(pFIle, "ERROR::ASSIMP::MODEL::ReadFile failed = %s\n", import.GetErrorString());
		fclose(pFIle);
		return;
	}

	fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
	fprintf(pFIle, "Success::ASSIMP::MODEL::ReadFile success\n");
	fclose(pFIle);

	directory = path.substr(0, path.find_last_of('/'));
	if (IsModelHavingAnimation == true)
	{
		m_global_inverse_transform = scene->mRootNode->mTransformation;
		m_global_inverse_transform.Inverse();

		if (scene->mAnimations[0]->mTicksPerSecond != 0.0)
		{
			ticks_per_second = (float)scene->mAnimations[0]->mTicksPerSecond;
		}
		else
		{
			ticks_per_second = 25.0f;
		}

		fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
		fprintf(pFIle, "Before Directory Access\n");
		fclose(pFIle);


		fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
		fprintf(pFIle, "ASSIMP::MODEL:: Directory-> %s\n", directory.c_str());
		fprintf(pFIle, "scene->HasAnimations() 1: %d\n", scene->HasAnimations());
		fprintf(pFIle, "scene->mNumMeshes 1: %d\n", scene->mNumMeshes);
		fprintf(pFIle, "scene->mAnimations[0]->mNumChannels 1: %d\n", scene->mAnimations[0]->mNumChannels);
		fprintf(pFIle, "scene->mAnimations[0]->mDuration 1: %f\n", scene->mAnimations[0]->mDuration);
		fprintf(pFIle, "scene->mAnimations[0]->mTicksPerSecond 1:  %f\n", scene->mAnimations[0]->mTicksPerSecond);

		fprintf(pFIle, "		name nodes : \n");
		fclose(pFIle);
	}
	showNodeName(scene->mRootNode);

	processNode(scene->mRootNode, scene);

	if (IsModelHavingAnimation == true)
	{
		for (unsigned int i = 0; i < scene->mAnimations[0]->mNumChannels; i++)
		{
			fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
			fprintf(pFIle, "nodes animation name : %s \n", scene->mAnimations[0]->mChannels[i]->mNodeName.C_Str());
			fclose(pFIle);
		}
	}
	fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
	fprintf(pFIle, "Exit LoadMyModel\n");
	fclose(pFIle);
}

void AnimModel::showNodeName(aiNode* node)
{
	FILE *pFIle = NULL;
	fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
	fprintf(pFIle, "Data : %s\n", node->mName.data);
	fclose(pFIle);
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		showNodeName(node->mChildren[i]);
	}
}

void AnimModel::processNode(aiNode* node, const aiScene* scene)
{
	AnimMesh mesh;
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* aiMesh = scene->mMeshes[i];
		mesh = processMesh(aiMesh, scene);
		meshes.push_back(mesh);
	}


}

AnimMesh AnimModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	FILE *pFIle = NULL;

	fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
	fprintf(pFIle, "bones: %d , vertices: %d\n", mesh->mNumBones, mesh->mNumVertices);
	fclose(pFIle);

	std::vector<smVertex> vertices;
	std::vector<GLuint>indices;
	std::vector<smTexture> textures;
	std::vector<smMaterial>materials;

	std::vector<VertexBoneData> bones_id_weights_for_each_vertex;

	vertices.reserve(mesh->mNumVertices);
	indices.reserve(mesh->mNumVertices);

	bones_id_weights_for_each_vertex.resize(mesh->mNumVertices);



	//vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		smVertex vertex;
		glm::vec3 vector1;
		vector1.x = mesh->mVertices[i].x;
		vector1.y = mesh->mVertices[i].y;
		vector1.z = mesh->mVertices[i].z;
		vertex.v3Position = vector1;


		if (mesh->mNormals != NULL)
		{
			vector1.x = mesh->mNormals[i].x;
			vector1.y = mesh->mNormals[i].y;
			vector1.z = mesh->mNormals[i].z;
			vertex.v3Normal = vector1;
		}
		else
		{
			vertex.v3Normal = glm::vec3();
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec[0] = mesh->mTextureCoords[0][i].x;
			vec[1] = mesh->mTextureCoords[0][i].y;
			vertex.v2TexCoords = vec;
		}
		else
		{
			vertex.v2TexCoords = glm::vec2(0.0f, 0.0f);
		}


		vector1.x = mesh->mTangents[i].x;
		vector1.y = mesh->mTangents[i].y;
		vector1.z = mesh->mTangents[i].z;
		vertex.v3Tangent = vector1;

		vector1.x = mesh->mBitangents[i].x;
		vector1.y = mesh->mBitangents[i].y;
		vector1.z = mesh->mBitangents[i].z;
		vertex.v3Bitangent = vector1;

		vertices.push_back(vertex);

	}

	//indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}



	//material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		smMaterial materialInfo;
		aiString matName;
		material->Get(AI_MATKEY_NAME, matName);
		materialInfo.material_name = std::string(matName.C_Str());
		fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
		fprintf(pFIle, "Name: %s \n", matName.C_Str());
		fclose(pFIle);

		// 1. diffuse maps
		std::vector<smTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<smTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<smTexture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<smTexture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		/*std::vector<Texture> opacityMaps = loadMaterialTextures(material, aiTextureType_OPACITY, "texture_opacity");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		std::vector<Texture> opacityMaps1 = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		std::vector<Texture> opacityMaps2 = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normals");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		std::vector<Texture> opacityMaps3 = loadMaterialTextures(material, aiTextureType_SHININESS, "texture_shininess");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		std::vector<Texture> opacityMaps4 = loadMaterialTextures(material, aiTextureType_DISPLACEMENT, "texture_displacement");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		std::vector<Texture> opacityMaps5 = loadMaterialTextures(material, aiTextureType_LIGHTMAP, "texture_lightMap");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		std::vector<Texture> opacityMaps6 = loadMaterialTextures(material, aiTextureType_REFLECTION, "texture_reflection");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		std::vector<Texture> opacityMaps7 = loadMaterialTextures(material, aiTextureType_NONE, "texture_none");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		std::vector<Texture> opacityMaps8 = loadMaterialTextures(material, aiTextureType_UNKNOWN, "texture_unknown");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());*/



		//ambient
		std::vector<smMaterial> MaterialAmbientColorMaps = loadMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, "material_ambient_animModel");
		materials.insert(materials.end(), MaterialAmbientColorMaps.begin(), MaterialAmbientColorMaps.end());
		//diffuse
		std::vector<smMaterial> MaterialDiffuseColorMaps = loadMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, "material_diffuse_animModel");
		materials.insert(materials.end(), MaterialDiffuseColorMaps.begin(), MaterialDiffuseColorMaps.end());
		//specular
		std::vector<smMaterial> MaterialSpecularColorMaps = loadMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, "material_specular_animModel");
		materials.insert(materials.end(), MaterialSpecularColorMaps.begin(), MaterialSpecularColorMaps.end());

	}
	if (pFIle)
	{
		fclose(pFIle);
		pFIle = NULL;
	}

	//load bones
	if (IsModelHavingAnimation == true)
	{
		for (unsigned int i = 0; i < mesh->mNumBones; i++)
		{
			int bone_index = 0;
			std::string bone_name(mesh->mBones[i]->mName.data);

			fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
			fprintf(pFIle, "MODEL::%s \n", mesh->mBones[i]->mName.data);
			fclose(pFIle);

			if (m_bone_mapping.find(bone_name) == m_bone_mapping.end())
			{
				//allocate an index for a new bone
				bone_index = m_num_bones;
				m_num_bones++;
				smBoneMatrix bi;
				m_bone_matrices.push_back(bi);
				m_bone_matrices[bone_index].aimat44_Offset_Matrix = mesh->mBones[i]->mOffsetMatrix;
				m_bone_mapping[bone_name] = bone_index;
			}
			else
			{
				bone_index = m_bone_mapping[bone_name];
			}

			for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
			{
				int vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
				float weight = mesh->mBones[i]->mWeights[j].mWeight;
				bones_id_weights_for_each_vertex[vertex_id].addBoneData(bone_index, weight);
			}
		}
	}
	if (pFIle)
	{
		fclose(pFIle);
		pFIle = NULL;
	}
	return AnimMesh(vertices, indices, textures, materials, bones_id_weights_for_each_vertex);
}

std::vector<smTexture> AnimModel::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string type_name)
{
	FILE *pFIle = NULL;
	std::vector<smTexture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString ai_str;
		mat->GetTexture(type, i, &ai_str);

		std::string filename = std::string(ai_str.C_Str());
		filename = directory + '/' + filename;

		fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
		fprintf(pFIle, "Texture::%s of type_name:%s\n", ai_str.C_Str(), type_name.c_str());
		fclose(pFIle);

		smTexture texture;

		texture.iTextureID = TextureFromFile(ai_str.C_Str(), this->directory, true); // return prepaired openGL texture
		texture.strTextureType = type_name;
		texture.aistrPathToTexture = ai_str;
		textures.push_back(texture);

	}

	return (textures);
}

std::vector<smMaterial> AnimModel::loadMaterialColor(aiMaterial * mat, const char * type, int one, int two, std::string typeName)
{
	FILE *pFIle = NULL;

	std::vector<smMaterial> materials;

	aiColor3D color;

	mat->Get(type, one, two, color);

	/*aiGetMaterialColor(mat, type, one, two, &color);*/

	smMaterial materialInfo;
	materialInfo.type = typeName;
	materialInfo.Value[0] = color[0];
	materialInfo.Value[1] = color[1];
	materialInfo.Value[2] = color[2];

	fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
	fprintf(pFIle, "Material %s:Value = %f, %f, %f \n", materialInfo.type.c_str(), materialInfo.Value[0], materialInfo.Value[1], materialInfo.Value[2]);
	fclose(pFIle);

	materials.push_back(materialInfo);

	return (materials);
}

int AnimModel::findPosition(float p_animation_time, const aiNodeAnim * p_node_anim)
{
	for (unsigned int i = 0; i < p_node_anim->mNumPositionKeys - 1; i++)
	{
		if (p_animation_time < (float)p_node_anim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);
	return 0;
}

int AnimModel::findRotation(float p_animation_time, const aiNodeAnim * p_node_anim)
{
	for (unsigned int i = 0; i < p_node_anim->mNumRotationKeys - 1; i++)
	{
		if (p_animation_time < (float)p_node_anim->mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);
	return 0;
}

int AnimModel::findScaling(float p_animation_time, const aiNodeAnim * p_node_anim)
{
	for (unsigned int i = 0; i < p_node_anim->mNumScalingKeys - 1; i++)
	{
		if (p_animation_time < (float)p_node_anim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);
	return 0;
}

const aiNodeAnim * AnimModel::findNodeAnim(const aiAnimation * p_animation, const std::string p_node_name)
{
	// channel in animation contains aiNodeAnim, aiNodeAnim its transformation for bones
	for (unsigned int i = 0; i < p_animation->mNumChannels; i++)
	{
		const aiNodeAnim* node_anim = p_animation->mChannels[i];
		if (std::string(node_anim->mNodeName.data) == p_node_name)
		{
			return node_anim;
		}
	}
	return nullptr;
}

aiVector3D AnimModel::calcInterpolatedPosition(float p_animation_time, const aiNodeAnim * p_node_anim)
{

	if (p_node_anim->mNumPositionKeys == 1)
	{
		return p_node_anim->mPositionKeys[0].mValue;
	}

	unsigned int position_index = findPosition(p_animation_time, p_node_anim);
	unsigned int next_position_index = position_index + 1;

	assert(next_position_index < p_node_anim->mNumPositionKeys);

	float delta_time = (float)(p_node_anim->mPositionKeys[next_position_index].mTime - p_node_anim->mPositionKeys[position_index].mTime);

	float factor = (p_animation_time - (float)p_node_anim->mPositionKeys[position_index].mTime) / delta_time;

	assert(factor >= 0.0f && factor <= 1.0f);
	aiVector3D start = p_node_anim->mPositionKeys[position_index].mValue;
	aiVector3D end = p_node_anim->mPositionKeys[next_position_index].mValue;
	aiVector3D delta = end - start;

	return start + factor * delta;
}

aiQuaternion AnimModel::calcInterpolatedRotation(float p_animation_time, const aiNodeAnim * p_node_anim)
{
	if (p_node_anim->mNumRotationKeys == 1)
	{
		return p_node_anim->mRotationKeys[0].mValue;
	}

	unsigned int rotation_index = findRotation(p_animation_time, p_node_anim);
	unsigned int next_rotation_index = rotation_index + 1;
	assert(next_rotation_index < p_node_anim->mNumRotationKeys);

	float delta_time = (float)(p_node_anim->mRotationKeys[next_rotation_index].mTime - p_node_anim->mRotationKeys[rotation_index].mTime);

	float factor = (p_animation_time - (float)p_node_anim->mRotationKeys[rotation_index].mTime) / delta_time;

	assert(factor >= 0.0f && factor <= 1.0f);
	aiQuaternion start_quat = p_node_anim->mRotationKeys[rotation_index].mValue;
	aiQuaternion end_quat = p_node_anim->mRotationKeys[next_rotation_index].mValue;

	return nlerp(start_quat, end_quat, factor);
}

aiVector3D AnimModel::calcInterpolatedScaling(float p_animation_time, const aiNodeAnim * p_node_anim)
{
	if (p_node_anim->mNumScalingKeys == 1)
	{
		return p_node_anim->mScalingKeys[0].mValue;
	}

	unsigned int scaling_index = findScaling(p_animation_time, p_node_anim);
	unsigned int next_scaling_index = scaling_index + 1;
	assert(next_scaling_index < p_node_anim->mNumScalingKeys);

	float delta_time = (float)(p_node_anim->mScalingKeys[next_scaling_index].mTime - p_node_anim->mScalingKeys[scaling_index].mTime);

	float  factor = (p_animation_time - (float)p_node_anim->mScalingKeys[scaling_index].mTime) / delta_time;
	assert(factor >= 0.0f && factor <= 1.0f);
	aiVector3D start = p_node_anim->mScalingKeys[scaling_index].mValue;
	aiVector3D end = p_node_anim->mScalingKeys[next_scaling_index].mValue;
	aiVector3D delta = end - start;

	return start + factor * delta;
}

void AnimModel::readNodeHierarchy(float p_animation_time, const aiNode * p_node, const aiMatrix4x4 parent_transform)
{
	std::string node_name(p_node->mName.data);


	const aiAnimation* animation = scene->mAnimations[0];
	aiMatrix4x4 node_transform = p_node->mTransformation;

	const aiNodeAnim* node_anim = findNodeAnim(animation, node_name);

	if (node_anim)
	{

		//scaling
		//aiVector3D scaling_vector = node_anim->mScalingKeys[2].mValue;
		aiVector3D scaling_vector = calcInterpolatedScaling(p_animation_time, node_anim);
		aiMatrix4x4 scaling_matr;
		aiMatrix4x4::Scaling(scaling_vector, scaling_matr);

		//rotation
		//aiQuaternion rotate_quat = node_anim->mRotationKeys[2].mValue;
		aiQuaternion rotate_quat = calcInterpolatedRotation(p_animation_time, node_anim);
		aiMatrix4x4 rotate_matr = aiMatrix4x4(rotate_quat.GetMatrix());

		//translation
		//aiVector3D translate_vector = node_anim->mPositionKeys[2].mValue;
		aiVector3D translate_vector = calcInterpolatedPosition(p_animation_time, node_anim);
		aiMatrix4x4 translate_matr;
		aiMatrix4x4::Translation(translate_vector, translate_matr);

		if (std::string(node_anim->mNodeName.data) == std::string("Head") || std::string(node_anim->mNodeName.data) == std::string("Hand_L"))
		{
			aiQuaternion rotate_head = aiQuaternion(rotate_head_xz.w, rotate_head_xz.x, rotate_head_xz.y, rotate_head_xz.z);

			node_transform = translate_matr * (rotate_matr * aiMatrix4x4(rotate_head.GetMatrix())) * scaling_matr;
		}
		else
		{
			node_transform = translate_matr * rotate_matr * scaling_matr;
		}

	}

	aiMatrix4x4 global_transform = parent_transform * node_transform;


	if (m_bone_mapping.find(node_name) != m_bone_mapping.end()) // true if node_name exist in bone_mapping
	{
		int bone_index = m_bone_mapping[node_name];
		m_bone_matrices[bone_index].aimat44_Final_World_Transform = m_global_inverse_transform * global_transform * m_bone_matrices[bone_index].aimat44_Offset_Matrix;
	}

	for (unsigned int i = 0; i < p_node->mNumChildren; i++)
	{
		readNodeHierarchy(p_animation_time, p_node->mChildren[i], global_transform);
	}

}

void AnimModel::boneTransform(double time_in_sec, std::vector<aiMatrix4x4>& transforms)
{
	aiMatrix4x4 identity_matrix; // = mat4(1.0f);

	double time_in_ticks = time_in_sec * ticks_per_second;
	float animation_time = (float)fmod(time_in_ticks, scene->mAnimations[0]->mDuration);

	readNodeHierarchy(animation_time, scene->mRootNode, identity_matrix);

	transforms.resize(m_num_bones);

	for (unsigned int i = 0; i < m_num_bones; i++)
	{
		transforms[i] = m_bone_matrices[i].aimat44_Final_World_Transform;
	}
}


glm::mat4 AnimModel::aiToGlm(aiMatrix4x4 ai_matr)
{
	glm::mat4 result;
	result[0].x = ai_matr.a1; result[0].y = ai_matr.b1; result[0].z = ai_matr.c1; result[0].w = ai_matr.d1;
	result[1].x = ai_matr.a2; result[1].y = ai_matr.b2; result[1].z = ai_matr.c2; result[1].w = ai_matr.d2;
	result[2].x = ai_matr.a3; result[2].y = ai_matr.b3; result[2].z = ai_matr.c3; result[2].w = ai_matr.d3;
	result[3].x = ai_matr.a4; result[3].y = ai_matr.b4; result[3].z = ai_matr.c4; result[3].w = ai_matr.d4;

	return result;
}

aiQuaternion AnimModel::nlerp(aiQuaternion a, aiQuaternion b, float blend)
{

	a.Normalize();
	b.Normalize();

	aiQuaternion result;
	float dot_product = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	float one_minus_blend = 1.0f - blend;

	if (dot_product < 0.0f)
	{
		result.x = a.x * one_minus_blend + blend * -b.x;
		result.y = a.y * one_minus_blend + blend * -b.y;
		result.z = a.z * one_minus_blend + blend * -b.z;
		result.w = a.w * one_minus_blend + blend * -b.w;
	}
	else
	{
		result.x = a.x * one_minus_blend + blend * b.x;
		result.y = a.y * one_minus_blend + blend * b.y;
		result.z = a.z * one_minus_blend + blend * b.z;
		result.w = a.w * one_minus_blend + blend * b.w;
	}

	return result.Normalize();
}

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	FILE *pFIle = NULL;

	unsigned int textureID;
	GLfloat Anisotropy = 4.0f;

	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);



		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Anisotropy);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Anisotropy);

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);

		fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
		fprintf(pFIle, "Texture load at path : %s \n", path);
		fclose(pFIle);
	}
	else
	{
		//	std::cout << "Texture failed to load at path: " << path << std::endl;
		fopen_s(&pFIle, "SamModelLogFile.txt", "a+");
		fprintf(pFIle, "Texture failed to load at path : %s \n", path);
		fclose(pFIle);

		//std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

