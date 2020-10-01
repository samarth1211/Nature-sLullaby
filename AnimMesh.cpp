#include"AnimMesh.h"

void VertexBoneData::addBoneData(UINT bone_id, float weight)
{
	for (UINT i = 0; i < NUM_BONES_PER_VEREX; i++)
	{
		if (weights[i] == 0.0)
		{
			ids[i] = bone_id;
			weights[i] = weight;
			return;
		}
	}
}


AnimMesh::AnimMesh(std::vector<smVertex> vertex, std::vector<GLuint> ind, std::vector<smTexture> texture, std::vector<smMaterial> material, std::vector<VertexBoneData> bone_id_weights)
{
	this->vertices = vertex;
	this->indices = ind;
	this->textures = texture;
	this->materials = material;

	this->bones_id_weights_for_each_vertex = bone_id_weights;

	this->SetupMesh();
}

AnimMesh::~AnimMesh()
{

}

void AnimMesh::MeshCleanUP()
{
	/*
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
	*/

	if (EBO_indices)
	{
		glDeleteBuffers(1, &EBO_indices);
		EBO_indices = NULL;
	}

	if (VBO_bones)
	{
		glDeleteBuffers(1, &VBO_bones);
		VBO_bones = NULL;
	}

	if (VBO_Vertices)
	{
		glDeleteBuffers(1, &VBO_Vertices);
		VBO_Vertices = NULL;
	}

	if (VAO)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = NULL;
	}


	if (bones_id_weights_for_each_vertex.size() >0)
	{
		bones_id_weights_for_each_vertex.clear();
		bones_id_weights_for_each_vertex.shrink_to_fit();
	}

	if (materials.size() >0)
	{
		materials.clear();
		materials.shrink_to_fit();
	}


	if (indices.size() >0)
	{
		indices.clear();
		indices.shrink_to_fit();
	}


	if (vertices.size() >0)
	{
		vertices.clear();
		vertices.shrink_to_fit();
	}

	if (textures.size() > 0)
	{
		for (size_t i = 0; i < textures.size(); i++)
		{
			glDeleteTextures(1, &textures[i].iTextureID);
			textures[i].iTextureID = NULL;
		}

		textures.clear();
		textures.shrink_to_fit();
	}
}

void AnimMesh::SetupMesh()
{

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &this->VBO_Vertices);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO_Vertices);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size()*sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(smVertex), (GLvoid*)0);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(smVertex), (GLvoid*)offsetof(smVertex, v3Normal));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(smVertex), (GLvoid*)offsetof(smVertex, v2TexCoords));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TANGENT);
	glVertexAttribPointer(AMC_ATTRIBUTE_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(smVertex), (GLvoid*)offsetof(smVertex, v3Tangent));
	//// vertex bitangent
	glEnableVertexAttribArray(AMC_ATTRIBUTE_BITANGENT);
	glVertexAttribPointer(AMC_ATTRIBUTE_BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(smVertex), (GLvoid*)offsetof(smVertex, v3Bitangent));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &this->VBO_bones);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO_bones);
	glBufferData(GL_ARRAY_BUFFER, this->bones_id_weights_for_each_vertex.size() * sizeof(bones_id_weights_for_each_vertex[0]), &bones_id_weights_for_each_vertex[0], GL_STATIC_DRAW);

	glVertexAttribIPointer(AMC_ATTRIBUTE_ID, 4, GL_INT, sizeof(VertexBoneData), (GLvoid*)0);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_ID);
	glVertexAttribPointer(AMC_ATTRIBUTE_WEIGHTS, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (GLvoid*)offsetof(VertexBoneData, weights));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_WEIGHTS);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &this->EBO_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void AnimMesh::Draw(GLuint shaders_program)
{
	unsigned int diffuse_nr = 1;
	unsigned int specular_nr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	bool isTexturePresent = false;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		isTexturePresent = true;
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = textures[i].strTextureType;

		if (name == "texture_diffuse")
		{
			number = std::to_string(diffuse_nr++);
		}
		else if (name == "texture_specular")
		{
			number = std::to_string(specular_nr++);
		}
		else if (name == "texture_normal")
		{
			number = std::to_string(normalNr++);
		}
		else if (name == "texture_height")
		{
			number = std::to_string(heightNr++);
		}

		glBindTexture(GL_TEXTURE_2D, textures[i].iTextureID);

		glUniform1i(glGetUniformLocation(shaders_program, (name + number).c_str()), i);
	}

	unsigned int diffuse_mr = 1;
	for (unsigned int i = 0; i < materials.size(); i++)
	{
		std::string number;
		std::string name = materials[i].type;

		if ((name == "material_ambient_animModel") || (name == "material_diffuse_animModel") || (name == "material_specular_animModel"))
		{
			GLint retval = glGetUniformLocation(shaders_program, (name).c_str());
			glUniform3fv(glGetUniformLocation(shaders_program, (name).c_str()), 1, materials[i].Value);
		}
	}

	if (isTexturePresent == true)
	{
		glUniform1i(glGetUniformLocation(shaders_program, "isTexture"), 1);
	}
	else
	{
		glUniform1i(glGetUniformLocation(shaders_program, "isTexture"), 0);
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertices);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Why texture bind call here ?
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void AnimMesh::DrawInsctanced(GLuint shaders_program, GLint numOfInstances)
{
	unsigned int diffuse_nr = 1;
	unsigned int specular_nr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	bool isTexturePresent = false;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		isTexturePresent = true;
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = textures[i].strTextureType;

		if (name == "texture_diffuse")
		{
			number = std::to_string(diffuse_nr++);
		}
		else if (name == "texture_specular")
		{
			number = std::to_string(specular_nr++);
		}
		else if (name == "texture_normal")
		{
			number = std::to_string(normalNr++);
		}
		else if (name == "texture_height")
		{
			number = std::to_string(heightNr++);
		}

		glBindTexture(GL_TEXTURE_2D, textures[i].iTextureID);

		glUniform1i(glGetUniformLocation(shaders_program, (name + number).c_str()), i);
	}

	unsigned int diffuse_mr = 1;
	for (unsigned int i = 0; i < materials.size(); i++)
	{
		std::string number;
		std::string name = materials[i].type;

		if ((name == "material_ambient_animModel") || (name == "material_diffuse_animModel") || (name == "material_specular_animModel"))
		{
			GLint retval = glGetUniformLocation(shaders_program, (name).c_str());
			glUniform3fv(glGetUniformLocation(shaders_program, (name).c_str()), 1, materials[i].Value);
		}
	}

	if (isTexturePresent == true)
	{
		glUniform1i(glGetUniformLocation(shaders_program, "isTexture"), 1);
	}
	else
	{
		glUniform1i(glGetUniformLocation(shaders_program, "isTexture"), 0);
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertices);
	//glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT,0, numOfInstances);
	glBindVertexArray(0);

	// Why texture bind call here ?
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

