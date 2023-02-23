#version 330

/*
	The vertex shader runs once for every vertex in the primitive
	Its main job is to convert the vertex position to a screen position by writing to gl_Position

	Note: I set the extension to .vert to avoid a clash with NSight but still allow the highlighting
	plugin to work (GLSL Langauage integration - highly recommended).
*/
uniform mat4 combined_xform;
uniform mat4 model_xform;
uniform sampler2D sampler_tex;

out vec3 varying_normal;
out vec3 varying_position;
out vec2 varying_texcoords;

layout (location=0) in vec3 vertex_position;
layout (location=1) in vec3 vertex_normal;
layout (location=2) in vec2 vertex_texcoords;

out vec3 varying_gouraud;
out float intensityVaried;

void main(void)
{
	varying_position =(model_xform * vec4(vertex_position, 1.0)).xyz;

	//varying_normal = (model_xform * vec4(vertex_normal, 0.0)).xyz;
	
	varying_texcoords=vertex_texcoords;
	varying_normal= mat3(model_xform) * vertex_normal;
	
	vec3 P = varying_position;
	vec3 N = varying_normal;

	vec3 light_direction=vec3(-0.5,-0.5,0.0);
	vec3 L=normalize(-light_direction);

	intensityVaried=max(0,dot(L,N));
	gl_Position= combined_xform * model_xform * vec4(vertex_position, 1.0);

	
}