#version 330 core

in vec3 inPos;	

//Pasar una única matriz, para ahorrarse las tres multiplicaciones por vértice, la variable ya estará multiplicada. Así se mejora la eficiencia.

uniform mat4 modelViewProj;
out vec3 color;

void main()
{
	//El color de los vértices depende del id de estos. Después, se pasan los datos a las primitivas y su color dependerá de ello
	
	if (mod (gl_VertexID,4.0) == 0.0)
		color = vec3(1.0,0.0,0.0);
		
	else if (mod (gl_VertexID, 4.0) == 1.0)
		color = vec3(0.0,1.0,0.0);

	else if (mod (gl_VertexID, 4.0) == 2.0)
		color = vec3(0.0,0.0,1.0);

	else if (mod (gl_VertexID, 4.0) == 3.0)
		color = vec3(1.0);
	
	
	gl_Position = modelViewProj*vec4(inPos, 1); 
}




