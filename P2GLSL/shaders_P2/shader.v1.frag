#version 330 core

out vec4 outColor;

in vec3 color; //Recibe el color calculado en el shader de vértices, tras haberlo recalculado aplicándole iluminación

void main()
{
	outColor = vec4(color, 1.0);   
}
