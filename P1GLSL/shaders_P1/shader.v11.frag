#version 330 core

out vec4 outColor; // Vector que va a almacenar el color
in vec2 texCoord; // Recibe las coordenadas de textura

void main()
{
	if((((texCoord[0]-0.5)*(texCoord[0]-0.5))+((texCoord[1]-0.5)*(texCoord[1]-0.5)))<0.25) {

		outColor = vec4(texCoord, 0.0, 1.0);
	}

	else {
		
		discard;
	}
}