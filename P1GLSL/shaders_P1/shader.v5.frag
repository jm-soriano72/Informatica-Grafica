#version 330 core

out vec4 outColor;
in vec3 color; //Parámetro que viene dado por los vértices que determina el color de las primitivas

void main()
{

	outColor = vec4(color, 1.0);
	
}
