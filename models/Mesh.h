#pragma once

#include <windows.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Data.h"

using namespace std;
typedef unsigned int uint;
#define NUM_BONES_PER_VEREX 4

namespace ModelLoad 
{
	struct Vertex
	{
		
		glm::vec3 Position;// Position
		glm::vec3 Normal; // Normal
		glm::vec2 TexCoords;// TexCoords
	};

	struct Texture
	{
		GLuint id;
		string type;
		aiString path;
	};

	
	struct BoneMatrix
	{
		aiMatrix4x4 offset_matrix;
		aiMatrix4x4 final_world_transform;

	};
	struct VertexBoneData
	{
		uint ids[NUM_BONES_PER_VEREX];   // we have 4 bone ids for EACH vertex & 4 weights for EACH vertex
		float weights[NUM_BONES_PER_VEREX];

		VertexBoneData()
		{
			memset(ids, 0, sizeof(ids));    // init all values in array = 0
			memset(weights, 0, sizeof(weights));
		}

		void addBoneData(uint bone_id, float weight)
		{
			for (uint i = 0; i < NUM_BONES_PER_VEREX; i++)
			{
				if (weights[i] == 0.0)
				{
					ids[i] = bone_id;
					weights[i] = weight;
					return;
				}
			}
		}
	};

	class Mesh
	{
	public:
		/*  Mesh Data  */
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;
		vector<VertexBoneData> bones_id_weights_for_each_vertex;

		/*  Functions  */
		// Constructor0
		Mesh() {};
		// Constructor1
		Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
		{
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;

			// Now that we have all the required data, set the vertex buffers and its attribute pointers.
			this->setupMesh();
		}

		// Constructor2
		// New contstructor with last argument bones
		Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, vector<VertexBoneData> bone_id_weights)
		{
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;
			this->bones_id_weights_for_each_vertex = bone_id_weights;

			// Now that we have all the required data, set the vertex buffers and its attribute pointers.
			this->setupMesh();
		}

		// Render the mesh
		void Draw(GLuint shaderProgram,GLint numOfInstances)
		{
			// Bind appropriate textures
			GLuint diffuseNr = 1;
			GLuint specularNr = 1;

			for (GLuint i = 0; i < this->textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
												  // Retrieve texture number (the N in diffuse_textureN)
				stringstream ss;
				string number;
				string name = this->textures[i].type;

				if (name == "texture_diffuse")
				{
					ss << diffuseNr++; // Transfer GLuint to stream
				}
				else if (name == "texture_specular")
				{
					ss << specularNr++; // Transfer GLuint to stream
				}

				number = ss.str();
				// Now set the sampler to the correct texture unit
				GLuint samplerId = glGetUniformLocation(shaderProgram, (name + number).c_str());
				
				//FILE *pFile;
				//fopen_s(&pFile, "Logs/ModelTexture.txt", "a+");
				/*if (samplerId == -1)
					fprintf(pFile, "");*/
				//fclose(pFile);
				//CCommon error;
				//error._check_gl_error(pFile, "Logs/ModelTexture.txt", __FILE__, __LINE__);

				glUniform1i(samplerId, i);
				// And finally bind the texture
				glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
			}

			// Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
			//glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 16.0f);

			// Draw mesh
			glBindVertexArray(this->VAO);
			//glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
			//glDrawElementsInstanced(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0); GLint 
			glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, NULL, numOfInstances);
			glBindVertexArray(0);

			// Always good practice to set everything back to defaults once configured.
			for (GLuint i = 0; i < this->textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}

		// Render The Mesh For Shadow
		void DrawShadow(GLuint shaderProgram, GLint numOfInstances)
		{
			glBindVertexArray(this->VAO);
			glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, NULL, numOfInstances);
			glBindVertexArray(0);
		}

	private:
		/*  Render data  */
		GLuint VAO;
		GLuint VBO;			//vertices
		GLuint VBO_bones;	//bones
		GLuint EBO;			//indices

		/*  Functions    */
		// Initializes all the buffer objects/arrays
		void setupMesh()
		{
			// Create buffers/arrays
			glGenVertexArrays(1, &this->VAO);
			glBindVertexArray(this->VAO);

			//Vertices data
			// Load data into vertex buffers
			glGenBuffers(1, &this->VBO);
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
			

			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
			//glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//Bones data
			glGenBuffers(1, &this->VBO_bones);
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO_bones);
			glBufferData(GL_ARRAY_BUFFER, this->bones_id_weights_for_each_vertex.size() * sizeof(VertexBoneData), &this->bones_id_weights_for_each_vertex[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//Indices data
			glGenBuffers(1, &this->EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);


			/**** Set the vertex attribute pointers, interleaved arrays--> Position, Normal, TexCoords **/
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
			// Vertex Positions
			glEnableVertexAttribArray(OGL_ATTRIBUTE_VERTEX);
			glVertexAttribPointer(OGL_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);

			// Vertex Normals
			glEnableVertexAttribArray(OGL_ATTRIBUTE_NORMAL);// offsetof(Vertex, normal) = returns the byte offset of that variable from the start of the struct
			glVertexAttribPointer(OGL_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, Normal));

			// Vertex Texture Coords
			glEnableVertexAttribArray(OGL_ATTRIBUTE_TEXTURE0);
			glVertexAttribPointer(OGL_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, TexCoords));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//Vertex bones id
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO_bones);
			glEnableVertexAttribArray(OGL_ATTRIBUTE_BONEID);
			glVertexAttribIPointer(OGL_ATTRIBUTE_BONEID, 4, GL_INT, sizeof(VertexBoneData), (GLvoid*)0); // for INT Ipointer
			//weights
			glEnableVertexAttribArray(OGL_ATTRIBUTE_BONEWEIGHT);
			glVertexAttribPointer(OGL_ATTRIBUTE_BONEWEIGHT, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (GLvoid*)offsetof(VertexBoneData, weights));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
			glBindVertexArray(0);
		}
	};
}//namespace ModelLoad