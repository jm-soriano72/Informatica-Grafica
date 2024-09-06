#version 330 core

in vec3 inPos;	
in vec2 inTexCoord;
uniform mat4 modelViewProj;

out vec2 texCoord; //Coordenadas de textura para cada v�rtice

void main()
{
	texCoord = inTexCoord;
	gl_Position =  modelViewProj * vec4(inPos, 1.0);
}
