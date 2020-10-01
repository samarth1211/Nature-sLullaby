#version 450 core

layout ( location = 0 ) in vec4 in_position;

uniform mat4 u_Light_view_matrix;
uniform mat4 u_Light_projection_matrix;

layout (std140, binding = 0) uniform perInstanceData
{
	mat4 u_instanceModelMatrix[615];
};

void main( )
{	
	mat4 u_scene_model_matrix = u_instanceModelMatrix[gl_InstanceID];
	mat4 shadowMatrix = u_Light_projection_matrix * u_Light_view_matrix * u_scene_model_matrix;
    gl_Position = shadowMatrix * in_position;
}
