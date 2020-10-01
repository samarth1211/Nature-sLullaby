#version 450 core

layout ( location = 0 ) in vec3 in_position;
layout ( location = 2 ) in vec3 in_normal;
layout ( location = 3 ) in vec2 in_texCoords;

out VS_OUT
{
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main( )
{	
	vs_out.FragPos = vec3(model * vec4(in_position, 1.0));
	vs_out.TexCoords = in_texCoords;
	vs_out.Normal = mat3(transpose(inverse(model))) * in_normal;
    gl_Position = projection * view * model * vec4(in_position, 1.0);
}
