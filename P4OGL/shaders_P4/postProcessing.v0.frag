#version 330 core

//Color de salida
out vec4 outColor;

//Variables Variantes
in vec2 texCoord;

//Textura
uniform sampler2D colorTex;

void main()
{
	outColor = vec4(textureLod(colorTex,texCoord,0).xyz,1.0); //Pantalla del mismo n�mero de p�xeles que la textura, as� se evita .......
}