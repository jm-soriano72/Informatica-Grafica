#version 330 core

in vec3 inPos;	
in vec3 inNormal; // Se pasa como parámetro la normal de cada vértice
in vec2 inTexCoord; // Se pasan como parámetros las coordenadas de textura
uniform mat4 modelViewProj;
uniform mat4 modelView;


out vec3 norm;  // Se devuelve la normal en coordenadas de la cámara
out vec2 texCoord;  // Se devuelven las coordenadas de textura

void main()
{
	norm = (transpose(inverse (modelView)) * vec4(inNormal,0)).xyz; //Para expresar las normales en coordenadas de la cámara, se multiplica por la inversa de la matriz de vista, traspuesta por que se trata de las normales
	texCoord = inTexCoord; 
	gl_Position =  modelViewProj * vec4(inPos, 1.0);

}
