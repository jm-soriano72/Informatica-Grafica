#version 330 core

out vec4 outColor;

in vec3 color; //Recibe el color calculado en el shader de v�rtices, tras haberlo recalculado aplic�ndole iluminaci�n

void main()
{
	outColor = vec4(color, 1.0);   
}
