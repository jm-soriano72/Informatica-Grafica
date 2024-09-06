#version 330 core

in vec3 inPos;	
#define M_PI 3.14592

void main()
{
	mat4 modelView = mat4 (1.0);  //Definir matriz modelView
	modelView[3].z = -3; 
	mat4 proj = mat4 (1.0);
	float f = 1.0/ tan(M_PI/60);  //Utilizando la apertura de 60� que dan como dato, se ahorra el c�lculo de top, bottom... a trav�s de la tangente del �ngulo sim�trico
	float far = 15.0;
	float near = 0.1;

	//Se define la matriz de proyecci�n en OpenGL

	proj[0].x = f;
	proj[1].y = f;
	proj[2].z = (far + near) / (near-far);
	proj[2].w = -1.0;
	proj[3].z = (2.0*far*near) / (near-far);
	proj[3].w = 0.0;


	gl_Position = proj*modelView*vec4(inPos, 1); //Para calcular la posici�n de un v�rtice, se multiplica la matriz de proyecci�n por la matriz de modelo (transformaciones afines), por el vector de posici�n
}




