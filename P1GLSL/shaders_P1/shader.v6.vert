#version 330 core

in vec3 inPos;	

//Pasar una única matriz, para ahorrarse las tres multiplicaciones por vértice, la variable ya estará multiplicada. Así se mejora la eficiencia.

uniform mat4 modelViewProj;

in vec3 inColor; //Este color lo coge del fichero del cubo BOX.h
out vec3 color;

void main()
{
	color = inColor;
	gl_Position = modelViewProj*vec4(inPos, 1); 
}




