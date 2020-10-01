#pragma once


#include"CommonHeader.h"

#include <GL/glew.h>
#include <GL/GL.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <glm\ext.hpp>

//#include <windows.h>

class CSkyDome
{
private:
	GLuint ShaderProgramObjectSky;
	GLuint FrameBufferObjectSky;
	GLuint VAOSkyDome;
	GLuint VBOSkyDomePosition;
	
	float SunRadius;
	glm::vec3 SunCPos, SunRotVec;
	glm::vec3 SunCDir, SunWPos;
	glm::vec3 LightColor;
	glm::vec3 SunColor;
	glm::vec3 CamPos;

	GLfloat AmbientIntensity;
	GLfloat DiffuseIntensity;

	float RefractionFactor;

	GLint Samples;	// Number of sample rays to use in the integral equation
	GLfloat Kr; // Rayleight scattering constant
	GLfloat Km; // Mie scattering constant
	GLfloat ESun; // Sun brightness constant
	GLfloat g; // The Mie phase asymmentry factor
	GLfloat Exposure;
	GLfloat InnerRadius;
	GLfloat OuterRadius;
	GLfloat Scale;
	GLfloat ScaleDepth;
	GLfloat ScaleOverScaleDepth;

	GLfloat WavelengthRed;
	GLfloat WavelengthGreen;
	GLfloat WavelengthBlue;

	GLfloat WavelengthRed4;
	GLfloat WavelengthGreen4;
	GLfloat WavelengthBlue4;

	glm::vec3 InvWaveLength;
	glm::vec3 CameraPos;

	void ComputeSkyDome(void);

	GLfloat alpha;
	GLboolean DecreaseAlpha;
	GLboolean IncreaseAlpha;
	GLboolean IsSunriseComplete;
	GLboolean IsSunsetComplete;

	bool m_bItNight;
	bool m_bItDay;

public:
	GLfloat TransitionSpeed = 0.5f;

	FILE *pSkyFile = NULL;
	CSkyDome();
	~CSkyDome();

	void SetShaderProgramObject(GLuint SPO);
	GLuint GetShaderProgramObject(void);

	void InitializeSky(GLuint ShaderObject);
	void RenderSky(glm::vec3 CamPos, glm::mat4 MVP, GLboolean Value);
	void UpdateSky(GLfloat fDelta);

	GLfloat GetRefractionFactor(void);
	GLfloat GetSunCDirY(void);
	GLfloat GetSunRadius(void);
	glm::vec3 GetSunColor(void);
	GLfloat GetInnerRadius(void);

	glm::vec3 GetLightColor(void);
	glm::vec3 GetSunWPos(void);
	glm::vec3 GetSunRotVec(void);
	GLfloat GetAmbientIntensity(void);
	GLfloat GetDiffuseIntensity(void);

	glm::vec3 GetSunCPos(void);
	glm::vec3 GetSunCDir(void);

	void SetSunCDirY(GLfloat SunCDirY);
	void SetSunWPos(glm::vec3 SunWPos);
	void SetSunCPos(glm::vec3 SunCPos);
	void SetSunCDir(glm::vec3 SunCDir);

	bool IsDay();
	bool IsNight();
	
};
