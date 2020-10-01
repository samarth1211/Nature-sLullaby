#pragma once

#include <windows.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SOIL2/SOIL2.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "LoadTGATexture.h"
#include "LoadShaders.h"
//#include "Texture/Tga.h"

#include "Data.h"

using namespace std;


#define MAX_BONES 100


namespace ModelLoad
{
	LoadTGAFile *tga = new LoadTGAFile();
	

	class Model
	{
	public:


		 
		/*  Functions   */
		// Constructor, expects a filepath to a 3D model.
		Model(const GLchar *path)
		{
			//log file
			/*if (fopen_s(&m_pFile, "Logs/ModelLoaderLog.txt", "w") != 0)
			{
				MessageBox(NULL, TEXT("MyModelLoaderLog File can not be created"), TEXT("Error"), MB_OK);
				ExitProcess(EXIT_FAILURE);
			}
			else
				fprintf(m_pFile, "ModelLoaderLog file created successfully\n");*/

			this->m_glProgramShadow = -1;
			this->scene = nullptr;
			this->m_glProgram = 0;
			this->m_glProgramNight = 0;
			this->m_bAnimationPresent = FALSE;
			this->rotate_head_xz = glm::quat(cos(glm::radians(0.0f)), sin(glm::radians(0.0f)) * glm::vec3(1.0f, 0.0f, 0.0f)); // this quad do nothingggggg!!!!!

			this->loadModel(path);
			this->InitShaders();
		}
		~Model()
		{
			/*if (m_pFile)
			{
				fprintf(m_pFile, "ModelLoaderLog file closed successfully\n");
				fclose(m_pFile);
				m_pFile = NULL;
			}*/
		}

		
		
		void InitShaders(std::string vs, std::string fs)
		{//only used of Letter Model Loading, dont use for other Medels
			string vertShaderPath = vs;
			string fragShaderPath = fs;

			ShaderInfo ModelLoad[] =
			{
				{ GL_VERTEX_SHADER, vertShaderPath.c_str(), 0 },
				{ GL_FRAGMENT_SHADER, fragShaderPath.c_str(), 0 },
				{ GL_NONE, NULL, 0 }
			};

			m_glProgram = LoadShaders(ModelLoad);
		}

		void InitShaders()
		{
			string vertShaderPath;
			string fragShaderPath;

			string vertShaderDepthPath;
			string fragShaderDepthPath;

			string vertShaderPathNight;
			string fragShaderPathNight;

			if (m_bAnimationPresent)
			{
				/*vertShaderPath = "Resources/Shaders/ModelLoadingWithAnimation.vert.glsl";
				fragShaderPath = "Resources/Shaders/ModelLoadingWithAnimation.frag.glsl";*/
			}
			else
			{
				//vertShaderPath = "models/Shaders/ModelLoadingWithoutAnimation.vert.glsl";
				vertShaderPath = "models/Shaders/ModelLoadingWithoutAnimationInstancing.vert.glsl";
				fragShaderPath = "models/Shaders/ModelLoadingWithoutAnimation.frag.glsl";

				vertShaderDepthPath = "models/Shaders/ModelLoadingWithoutAnimationInstancingShadow.vert.glsl";
				fragShaderDepthPath = "models/Shaders/ModelLoadingWithoutAnimationShadow.frag.glsl";

				vertShaderPathNight = "models/Shaders/ModelLoadingWithoutAnimationNight.vert.glsl";
				fragShaderPathNight = "models/Shaders/ModelLoadingWithoutAnimationNight.frag.glsl";
			}

			ShaderInfo ModelLoad[] =
			{
			{ GL_VERTEX_SHADER, vertShaderPath.c_str(), 0 },
			{ GL_FRAGMENT_SHADER, fragShaderPath.c_str(), 0 },
			{ GL_NONE, NULL, 0 }
			};

			ShaderInfo ModelLoadShadow[] = 
			{
				{ GL_VERTEX_SHADER, vertShaderDepthPath.c_str(), 0 },
				{ GL_FRAGMENT_SHADER, fragShaderDepthPath.c_str(), 0 },
				{ GL_NONE, NULL, 0 }
			};

			ShaderInfo ModelLoadNight[] =
			{
				{ GL_VERTEX_SHADER, vertShaderPathNight.c_str(), 0 },
				{ GL_FRAGMENT_SHADER, fragShaderPathNight.c_str(), 0 },
				{ GL_NONE, NULL, 0 }
			};

			m_glProgram = LoadShaders(ModelLoad);
			m_glProgramNight = LoadShaders(ModelLoadNight);

			m_glProgramShadow = LoadShaders(ModelLoadShadow);

			if (m_bAnimationPresent)
			{
				for (uint i = 0; i < MAX_BONES; i++) // get location all matrices of bones
				{
					string name = "bones[" + to_string(i) + "]";// name like in shader
					m_bone_location[i] = glGetUniformLocation(m_glProgram, name.c_str());
				}
			}
		}

		// Draws the model, and thus all its meshes
		void Draw(double tDelta,GLint numOfInstances)
		{
			if (m_bAnimationPresent)
			{
				/*static int writeCnt = 0;
				bool bWrite = false;
				if (writeCnt++ % 30 == 0)
					bWrite = true;
				//tDelta = 5.0f;*/
				vector<aiMatrix4x4> transforms;
				boneTransform(tDelta, transforms);
				/*if (bWrite)
					fprintf(m_pFile, "mat4x4 bones[]: tDelta=%lf\n", tDelta);*/
				for (uint i = 0; i < transforms.size(); i++) // move all matrices for actual model position to shader
				{
					glUniformMatrix4fv(m_bone_location[i], 1, GL_TRUE, (const GLfloat*)&transforms[i]);
					/*if (bWrite)
					{
						fprintf(m_pFile, "%f\t%f\t%f\t%f\n", transforms[i].a1, transforms[i].a2, transforms[i].a3, transforms[i].a4);
						fprintf(m_pFile, "%f\t%f\t%f\t%f\n", transforms[i].b1, transforms[i].b2, transforms[i].b3, transforms[i].b4);
						fprintf(m_pFile, "%f\t%f\t%f\t%f\n", transforms[i].c1, transforms[i].c2, transforms[i].c3, transforms[i].c4);
						fprintf(m_pFile,"%f\t%f\t%f\t%f\n\n",transforms[i].d1, transforms[i].d2, transforms[i].d3, transforms[i].d4);
					}*/
				}
			}

			for (GLuint i = 0; i < this->meshes.size(); i++)
			{
				this->meshes[i].Draw(m_glProgram, numOfInstances);
			}
		}


		void DrawForShadow(GLuint shaderProgram, double tDelta, GLint numOfInstances)
		{
			for (GLuint i = 0; i < this->meshes.size(); i++)
			{
				this->meshes[i].DrawShadow(shaderProgram, numOfInstances);
			}
		}

		void DrawNight(double tDelta, GLint numOfInstances)
		{
			if (m_bAnimationPresent)
			{
				/*static int writeCnt = 0;
				bool bWrite = false;
				if (writeCnt++ % 30 == 0)
				bWrite = true;
				//tDelta = 5.0f;*/
				vector<aiMatrix4x4> transforms;
				boneTransform(tDelta, transforms);
				/*if (bWrite)
				fprintf(m_pFile, "mat4x4 bones[]: tDelta=%lf\n", tDelta);*/
				for (uint i = 0; i < transforms.size(); i++) // move all matrices for actual model position to shader
				{
					glUniformMatrix4fv(m_bone_location[i], 1, GL_TRUE, (const GLfloat*)&transforms[i]);
					/*if (bWrite)
					{
					fprintf(m_pFile, "%f\t%f\t%f\t%f\n", transforms[i].a1, transforms[i].a2, transforms[i].a3, transforms[i].a4);
					fprintf(m_pFile, "%f\t%f\t%f\t%f\n", transforms[i].b1, transforms[i].b2, transforms[i].b3, transforms[i].b4);
					fprintf(m_pFile, "%f\t%f\t%f\t%f\n", transforms[i].c1, transforms[i].c2, transforms[i].c3, transforms[i].c4);
					fprintf(m_pFile,"%f\t%f\t%f\t%f\n\n",transforms[i].d1, transforms[i].d2, transforms[i].d3, transforms[i].d4);
					}*/
				}
			}

			for (GLuint i = 0; i < this->meshes.size(); i++)
			{
				this->meshes[i].Draw(m_glProgramNight, numOfInstances);
			}
		}

		// User Shader Program
		void UseProgram(void)
		{
			glUseProgram(this->m_glProgram);
		}

		// Shadow
		void UseProgramShadow(void)
		{
			glUseProgram(this->m_glProgramShadow);
		}

		void UseProgramNight(void)
		{
			glUseProgram(this->m_glProgramNight);
		}

		GLuint GetProgram()
		{
			return(this->m_glProgram);
		}

		// Shadow
		GLuint GetProgramShadow()
		{
			return (this->m_glProgramShadow);
		}

		GLuint GetProgramNight()
		{
			return(this->m_glProgramNight);
		}

	private:
		/*  Shader Data  */
		GLuint m_glProgram;
		GLuint m_glProgramNight; // for night scene
		GLuint m_glProgramShadow;

		/*  Model Data  */
		vector<Mesh> meshes;
		string directory;
		vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

		//FILE *m_pFile;
		float ticks_per_second = 0.0f;

		//Animation Data
		BOOL m_bAnimationPresent;
		map<string, uint> m_bone_mapping; // maps a bone name and their index
		uint m_num_bones = 0;
		vector<BoneMatrix> m_bone_matrices;
		aiMatrix4x4 m_global_inverse_transform;

		GLint m_bone_location[MAX_BONES];

		Assimp::Importer importer;
		const aiScene* scene;

		// rotate Head
		glm::quat rotate_head_xz = glm::quat(cos(glm::radians(0.0f)), sin(glm::radians(0.0f)) * glm::vec3(1.0f, 0.0f, 0.0f)); // this quad do nothingggggg!!!!!


		/*  Functions   */
		void boneTransform(double time_in_sec, vector<aiMatrix4x4>& transforms)
		{
			aiMatrix4x4 identity_matrix; //mat4(1.0f);

			if (this->scene == nullptr)
				return;
			if (this->scene->mAnimations == nullptr)
			{
				return;
			}

			double time_in_ticks = time_in_sec * ticks_per_second;
			float animation_time = (float)fmod(time_in_ticks, this->scene->mAnimations[0]->mDuration);

			readNodeHierarchy(animation_time, this->scene->mRootNode, identity_matrix);

			transforms.resize(m_num_bones);

			for (uint i = 0; i < m_num_bones; i++)
			{
				transforms[i] = m_bone_matrices[i].final_world_transform;
			}
		}

		// start from RootNode
		void readNodeHierarchy(float p_animation_time, const aiNode* p_node, const aiMatrix4x4 parent_transform)
		{
			string node_name(p_node->mName.data);

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

				if (string(node_anim->mNodeName.data) == string("Head"))
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
				uint bone_index = m_bone_mapping[node_name];
				m_bone_matrices[bone_index].final_world_transform = m_global_inverse_transform * global_transform * m_bone_matrices[bone_index].offset_matrix;
			}

			for (uint i = 0; i < p_node->mNumChildren; i++)
			{
				readNodeHierarchy(p_animation_time, p_node->mChildren[i], global_transform);
			}

		}

		const aiNodeAnim * findNodeAnim(const aiAnimation * p_animation, const string p_node_name)
		{
			// channel in animation contains aiNodeAnim (aiNodeAnim its transformation for bones)
			// numChannels == numBones
			for (uint i = 0; i < p_animation->mNumChannels; i++)
			{
				const aiNodeAnim* node_anim = p_animation->mChannels[i];
				if (string(node_anim->mNodeName.data) == p_node_name)
				{
					return node_anim;
				}
			}

			return nullptr;
		}

		aiVector3D calcInterpolatedPosition(float p_animation_time, const aiNodeAnim* p_node_anim)
		{
			if (p_node_anim->mNumPositionKeys == 1)
			{
				return p_node_anim->mPositionKeys[0].mValue;
			}

			uint position_index = findPosition(p_animation_time, p_node_anim);
			uint next_position_index = position_index + 1;
			assert(next_position_index < p_node_anim->mNumPositionKeys);

			float delta_time = (float)(p_node_anim->mPositionKeys[next_position_index].mTime - p_node_anim->mPositionKeys[position_index].mTime);

			float factor = (p_animation_time - (float)p_node_anim->mPositionKeys[position_index].mTime) / delta_time;
			assert(factor >= 0.0f && factor <= 1.0f);
			aiVector3D start = p_node_anim->mPositionKeys[position_index].mValue;
			aiVector3D end = p_node_anim->mPositionKeys[next_position_index].mValue;
			aiVector3D delta = end - start;

			return start + factor * delta;
		}

		aiQuaternion calcInterpolatedRotation(float p_animation_time, const aiNodeAnim* p_node_anim)
		{
			if (p_node_anim->mNumRotationKeys == 1)
			{
				return p_node_anim->mRotationKeys[0].mValue;
			}

			uint rotation_index = findRotation(p_animation_time, p_node_anim);
			uint next_rotation_index = rotation_index + 1;
			assert(next_rotation_index < p_node_anim->mNumRotationKeys);

			float delta_time = (float)(p_node_anim->mRotationKeys[next_rotation_index].mTime - p_node_anim->mRotationKeys[rotation_index].mTime);

			float factor = (p_animation_time - (float)p_node_anim->mRotationKeys[rotation_index].mTime) / delta_time;

			//cout << "p_node_anim->mRotationKeys[rotation_index].mTime: " << p_node_anim->mRotationKeys[rotation_index].mTime << endl;
			//cout << "p_node_anim->mRotationKeys[next_rotaion_index].mTime: " << p_node_anim->mRotationKeys[next_rotation_index].mTime << endl;
			//cout << "delta_time: " << delta_time << endl;
			//cout << "animation_time: " << p_animation_time << endl;
			//cout << "animation_time - mRotationKeys[rotation_index].mTime: " << (p_animation_time - (float)p_node_anim->mRotationKeys[rotation_index].mTime) << endl;
			//cout << "factor: " << factor << endl << endl << endl;

			assert(factor >= 0.0f && factor <= 1.0f);
			aiQuaternion start_quat = p_node_anim->mRotationKeys[rotation_index].mValue;
			aiQuaternion end_quat = p_node_anim->mRotationKeys[next_rotation_index].mValue;

			return nlerp(start_quat, end_quat, factor);
		}

		aiVector3D calcInterpolatedScaling(float p_animation_time, const aiNodeAnim* p_node_anim)
		{
			if (p_node_anim->mNumScalingKeys == 1)
			{
				return p_node_anim->mScalingKeys[0].mValue;
			}

			uint scaling_index = findScaling(p_animation_time, p_node_anim);
			uint next_scaling_index = scaling_index + 1;
			assert(next_scaling_index < p_node_anim->mNumScalingKeys);

			float delta_time = (float)(p_node_anim->mScalingKeys[next_scaling_index].mTime - p_node_anim->mScalingKeys[scaling_index].mTime);

			float  factor = (p_animation_time - (float)p_node_anim->mScalingKeys[scaling_index].mTime) / delta_time;
			assert(factor >= 0.0f && factor <= 1.0f);
			aiVector3D start = p_node_anim->mScalingKeys[scaling_index].mValue;
			aiVector3D end = p_node_anim->mScalingKeys[next_scaling_index].mValue;
			aiVector3D delta = end - start;

			return start + factor * delta;
		}

		uint findPosition(float p_animation_time, const aiNodeAnim* p_node_anim)
		{
			for (uint i = 0; i < p_node_anim->mNumPositionKeys - 1; i++)
			{
				if (p_animation_time < (float)p_node_anim->mPositionKeys[i + 1].mTime)
				{
					return i;
				}
			}

			assert(0);
			return 0;
		}

		uint findRotation(float p_animation_time, const aiNodeAnim* p_node_anim)
		{
			for (uint i = 0; i < p_node_anim->mNumRotationKeys - 1; i++)
			{
				if (p_animation_time < (float)p_node_anim->mRotationKeys[i + 1].mTime)
				{
					return i;
				}
			}

			assert(0);
			return 0;
		}

		uint findScaling(float p_animation_time, const aiNodeAnim* p_node_anim)
		{
			for (uint i = 0; i < p_node_anim->mNumScalingKeys - 1; i++)
			{
				if (p_animation_time < (float)p_node_anim->mScalingKeys[i + 1].mTime)
				{
					return i;
				}
			}

			assert(0);
			return 0;
		}

		aiQuaternion nlerp(aiQuaternion a, aiQuaternion b, float blend)
		{
			//cout << a.w + a.x + a.y + a.z << endl;
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


		// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
		void loadModel(string path)
		{
			// Read file via ASSIMP
			//Assimp::Importer importer;
			//const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
			this->scene = importer.ReadFile(path, aiProcess_Triangulate);

			// Check for errors
			if (!this->scene || this->scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !this->scene->mRootNode) // if is Not Zero
			{
				cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
				//fprintf(m_pFile, "ERROR::ASSIMP:: %s\n", importer.GetErrorString());
				return;
			}

			// Retrieve the directory path of the filepath, container for model.obj and textures and other files
			this->directory = path.substr(0, path.find_last_of('/'));

			//Animation handling: 
			m_global_inverse_transform = this->scene->mRootNode->mTransformation;
			m_global_inverse_transform.Inverse();

			if (this->scene->mAnimations)
			{
				m_bAnimationPresent = TRUE;
				//fprintf(m_pFile, "-->%s Has Animation Present\n", path.c_str());

				if (scene->mAnimations[0]->mTicksPerSecond != 0.0)
				{
					ticks_per_second = (float)scene->mAnimations[0]->mTicksPerSecond;
				}
				else
				{
					ticks_per_second = 25.0f;
				}

				//fprintf(m_pFile, "scene->HasAnimations() 1: %d\n", scene->HasAnimations());
				//fprintf(m_pFile, "scene->mNumMeshes 1: %d\n", scene->mNumMeshes);
				//fprintf(m_pFile, "scene->mAnimations[0]->mNumChannels 1: %d\n", scene->mAnimations[0]->mNumChannels);
				//fprintf(m_pFile, "scene->mAnimations[0]->mDuration 1: %lf\n", scene->mAnimations[0]->mDuration);
				//fprintf(m_pFile, "scene->mAnimations[0]->mTicksPerSecond 1: %lf\n", scene->mAnimations[0]->mTicksPerSecond);


			}
			else
			{
				m_bAnimationPresent = FALSE;
				//fprintf(m_pFile, "%s Has No Animation\n", path.c_str());
			}

			//fprintf(m_pFile, "\nNodes Names : \n");
			this->showNodeName(this->scene->mRootNode);

			// Process ASSIMP's root node recursively
			this->processNode(this->scene->mRootNode, this->scene);
		}

		//
		void showNodeName(aiNode* node)
		{
			//fprintf(m_pFile, "Node Name: %s\n", node->mName.data);
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				showNodeName(node->mChildren[i]);
			}
		}

		// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		/*void processNode(aiNode* node, const aiScene* scene)
		{
			// Process each mesh located at the current node
			for (GLuint i = 0; i < node->mNumMeshes; i++)
			{
				// The node object only contains indices to index the actual objects in the scene.
				// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

				this->meshes.push_back(this->processMesh(mesh, scene));
			}

			// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
			for (GLuint i = 0; i < node->mNumChildren; i++)
			{
				this->processNode(node->mChildren[i], scene);
			}
		}*/

		void processNode(aiNode* node, const aiScene* scene)
		{
			Mesh mesh;
			for (uint i = 0; i < scene->mNumMeshes; i++)
			{
				aiMesh* ai_mesh = scene->mMeshes[i];
				mesh = processMesh(ai_mesh, scene);
				meshes.push_back(mesh); //accumulate all meshes in one vector
			}

		}

		Mesh processMesh(aiMesh *mesh, const aiScene *scene)
		{
			// Data to fill
			vector<Vertex> vertices;
			vector<GLuint> indices;
			vector<Texture> textures;
			vector<VertexBoneData> bones_id_weights_for_each_vertex;
			bones_id_weights_for_each_vertex.resize(mesh->mNumVertices);

			//Vertices
			vertices.reserve(mesh->mNumVertices);
			indices.reserve(mesh->mNumVertices);
			// Walk through each of the mesh's vertices
			for (GLuint i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex;
				glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

				// Positions
				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.Position = vector;

				// Normals
				if (mesh->mNormals != NULL)
				{
					vector.x = mesh->mNormals[i].x;
					vector.y = mesh->mNormals[i].y;
					vector.z = mesh->mNormals[i].z;
					vertex.Normal = vector;
				}
				else
				{
					vertex.Normal = glm::vec3();
				}

				// Texture Coordinates
				if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
				{
					glm::vec2 vec;
					// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
					// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.TexCoords = vec;
				}
				else
				{
					vertex.TexCoords = glm::vec2(0.0f, 0.0f);
				}

				vertices.push_back(vertex);
			}

			//Indices
			// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			for (GLuint i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				// Retrieve all indices of the face and store them in the indices vector
				for (GLuint j = 0; j < face.mNumIndices; j++)
				{
					indices.push_back(face.mIndices[j]);
				}
			}

			// Process materials
			if (mesh->mMaterialIndex >= 0)
			{
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
				// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
				// Same applies to other texture as the following list summarizes:
				// Diffuse: texture_diffuseN
				// Specular: texture_specularN
				// Normal: texture_normalN

				// 1. Diffuse maps
				vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
				textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

				// 2. Specular maps
				vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
				textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

				// 3. Opacity maps
				vector<Texture> opacityMaps = this->loadMaterialTextures(material, aiTextureType_OPACITY, "texture_opacity");
				textures.insert(textures.end(), opacityMaps.begin(), opacityMaps.end());
			}

			// load bones
			for (uint i = 0; i < mesh->mNumBones; i++)
			{
				uint bone_index = 0;
				string bone_name(mesh->mBones[i]->mName.data);
				//fprintf(m_pFile, "Bone[%d]: %s\n", i, mesh->mBones[i]->mName.data);

				if (m_bone_mapping.find(bone_name) == m_bone_mapping.end())
				{
					// Allocate an index for a new bone
					bone_index = m_num_bones;
					m_num_bones++;
					BoneMatrix bi;
					m_bone_matrices.push_back(bi);
					m_bone_matrices[bone_index].offset_matrix = mesh->mBones[i]->mOffsetMatrix;
					m_bone_mapping[bone_name] = bone_index;

					//fprintf(m_pFile, "bone_name: %s \tbone_index: %d\n", bone_name.c_str(), bone_index);
				}
				else
				{
					bone_index = m_bone_mapping[bone_name];
				}

				for (uint j = 0; j < mesh->mBones[i]->mNumWeights; j++)
				{
					uint vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
					float weight = mesh->mBones[i]->mWeights[j].mWeight;
					bones_id_weights_for_each_vertex[vertex_id].addBoneData(bone_index, weight);

					//fprintf(m_pFile, " vertex_id: %d \t	bone_index:%d \tweight: %f\n", vertex_id, bone_index, weight);
				}
			}

			// Return a mesh object created from the extracted mesh data
			//return Mesh(vertices, indices, textures);
			return Mesh(vertices, indices, textures, bones_id_weights_for_each_vertex);
		}

		// Checks all material textures of a given type and loads the textures if they're not loaded yet.
		// The required info is returned as a Texture struct.
		vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
		{
			vector<Texture> textures;

			for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);

				// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
				GLboolean skip = false;

				for (GLuint j = 0; j < textures_loaded.size(); j++)
				{
					if (textures_loaded[j].path == str)
					{
						textures.push_back(textures_loaded[j]);
						skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)

						break;
					}
				}

				if (!skip)
				{   // If texture hasn't been loaded already, load it
					Texture texture;
					//texture.id = TextureFromFile(str.C_Str(), this->directory);
					string filename = this->directory + '/' + string(str.C_Str());
					texture.id = tga->LoadTGATexture(filename.c_str());
					texture.type = typeName;
					texture.path = str;
					textures.push_back(texture);

					this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
				}
			}

			return textures;
		}
	};

	
}
