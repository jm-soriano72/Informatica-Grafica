#version 330 core

out vec4 outColor;
in vec3 color; //Par�metro que viene dado por los v�rtices que determina el color de las primitivas

void main()
{

	outColor = vec4(color, 1.0);
	
}
