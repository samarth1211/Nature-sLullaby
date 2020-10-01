
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <glm\ext.hpp>


#include "CommonHeader.h"
#include "Data.h"
#include "LoadShaderFromFile.h"
#include "LoadShaders.h"




class CubeMap
{
private:
	
	GLuint gShaderProgramObjectCubemap;
	GLuint gVaoCubeMap;
	GLuint gVboCubeMap;
	GLuint CubemapTexture;
public:
	CubeMap();
	~CubeMap();

	void InitializeLightCube(GLuint ShaderProgObject);

	void DrawCubemap(glm::mat4 ViewMatrix, glm::mat4 gPerspectiveProjectionMatrix);
};
