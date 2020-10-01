#include "SunLight.h"

CSunLight::CSunLight()
{
}

CSunLight::~CSunLight()
{
}

glm::vec3 CSunLight::getAmbient()
{
	return glm::vec3(this->Ambient);
}

glm::vec3 CSunLight::getDiffuse()
{
	return this->Diffuse;
}

glm::vec3 CSunLight::getSpecular()
{
	return glm::vec3(this->Specular);
}

glm::vec3 CSunLight::getDirection()
{
	return glm::vec3(this->Direction);
}

void CSunLight::setAmbient(glm::vec3 & newAmbient)
{
	this->Ambient = newAmbient;
}

void CSunLight::setDiffuse(glm::vec3 & newDiffuse)
{
	this->Diffuse = newDiffuse;
}

void CSunLight::setSpecular(glm::vec3 & newSpecular)
{
	this->Specular = newSpecular;
}

void CSunLight::setDirection(glm::vec3 & newDirection)
{
	this->Direction = newDirection;
}
