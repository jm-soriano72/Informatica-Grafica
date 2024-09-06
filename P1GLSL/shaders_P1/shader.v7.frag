#version 330 core

out vec4 outColor;
in vec2 texCoord;

uniform sampler2D colorTex; //Manera de acceder a la textura (muestreo en 2D). El nombre es fundamental, ya que está definido en el main

void main()
{
		outColor = texture (colorTex,texCoord); //La textura por lo tanto se forma de las coordenadas de textura y del sampler 2D
}