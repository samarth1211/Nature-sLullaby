#version 450 core

layout (location = 0) in vec3 aPosition;

out vec3 TexCoords;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

void main()
{
	TexCoords = aPosition;
	vec4 Position = uProjectionMatrix * uViewMatrix * vec4(aPosition, 1.0);

	gl_Position = Position.xyww;
}