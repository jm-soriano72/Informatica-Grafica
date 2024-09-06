#version 330 core

out vec4 outColor; // Vector que va a almacenar el color

in vec3 norm; // Recibe la normal en coordenadas de la cámara
in vec2 texCoord; // Recibe las coordenadas de textura

void main()
{
	if(gl_PrimitiveID%2==0) {

		outColor = vec4(norm,1.0);
	}

	else {
		
		outColor = vec4(texCoord, 0.0, 1.0);
	}
}