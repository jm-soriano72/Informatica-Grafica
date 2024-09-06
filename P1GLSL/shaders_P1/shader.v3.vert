#version 330 core

in vec3 inPos;	

//Pasar una �nica matriz, para ahorrarse las tres multiplicaciones por v�rtice, la variable ya estar� multiplicada. As� se mejora la eficiencia.

uniform mat4 modelViewProj;

void main()
{

	gl_Position = modelViewProj*vec4(inPos, 1); 
}




