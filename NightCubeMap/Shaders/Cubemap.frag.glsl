#version 450 core

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube Cubemap;
uniform vec3 fogColor;
const float lowerLimit = 0.0;
const float upperLimit = 0.4;

void main()
{
	vec4 finalColor = texture(Cubemap, TexCoords);
	float factor=(TexCoords.y -lowerLimit)/(upperLimit- lowerLimit);
	factor = clamp(factor,0.0,1.0);
	FragColor = mix(vec4(fogColor,1.0), finalColor, factor);
}
