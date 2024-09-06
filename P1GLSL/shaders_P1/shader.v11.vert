#version 330 core

in vec3 inPos;	
in vec2 inTexCoord; // Se pasan como parámetros las coordenadas de textura
uniform mat4 modelViewProj;
uniform mat4 modelView;


out vec2 texCoord;  // Se devuelven las coordenadas de textura

void main()
{
	
	texCoord = inTexCoord; 
	gl_Position =  modelViewProj * vec4(inPos, 1.0);

}
