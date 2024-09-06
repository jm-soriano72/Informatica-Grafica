#version 330 core

in vec3 inPos;	

//Pasar las matrices definidas en el main para no definirlas una vez en cada vértice, utilizarlas directamente en cada uno
uniform mat4 proj; //3º Aplicación de la perspectiva a la escena y transformación de sus coordenadas
uniform mat4 model; //1º Transformaciones afines dentro del mundo
uniform mat4 view; //2º Transformación a coodenadas de la cámara

void main()
{

	gl_Position = proj*view*model*vec4(inPos, 1); 
}




