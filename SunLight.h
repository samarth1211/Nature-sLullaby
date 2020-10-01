#pragma once
#include"CommonHeader.h"

// This file is for using SunLight values which is shared by all
// objects in Land.
// Values must be in sync with atmosphear


class CSunLight
{
private:
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	glm::vec3 Direction;

public:

	CSunLight();
	~CSunLight();

	glm::vec3 getAmbient();
	glm::vec3 getDiffuse();
	glm::vec3 getSpecular();
	glm::vec3 getDirection();

	void setAmbient(glm::vec3 &newAmbient);
	void setDiffuse(glm::vec3 &newDiffuse);
	void setSpecular(glm::vec3 &newSpecular);
	void setDirection(glm::vec3 &newDirection);
};


