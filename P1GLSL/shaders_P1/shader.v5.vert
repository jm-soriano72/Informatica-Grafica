#version 330 core

in vec3 inPos;	

//Pasar una �nica matriz, para ahorrarse las tres multiplicaciones por v�rtice, la variable ya estar� multiplicada. As� se mejora la eficiencia.

uniform mat4 modelViewProj;
out vec3 color;

void main()
{
	//El color de los v�rtices depende del id de estos. Despu�s, se pasan los datos a las primitivas y su color depender� de ello
	
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




