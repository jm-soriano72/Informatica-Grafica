#version 330 core

in vec3 inPos;	

//Pasar una única matriz, para ahorrarse las tres multiplicaciones por vértice, la variable ya estará multiplicada. Así se mejora la eficiencia.

uniform mat4 modelViewProj;

void main()
{

	gl_Position = modelViewProj*vec4(inPos, 1); 
}




