#version 330 core

in vec3 inPos;	

//Pasar las matrices definidas en el main para no definirlas una vez en cada v�rtice, utilizarlas directamente en cada uno
uniform mat4 proj; //3� Aplicaci�n de la perspectiva a la escena y transformaci�n de sus coordenadas
uniform mat4 model; //1� Transformaciones afines dentro del mundo
uniform mat4 view; //2� Transformaci�n a coodenadas de la c�mara

void main()
{

	gl_Position = proj*view*model*vec4(inPos, 1); 
}




