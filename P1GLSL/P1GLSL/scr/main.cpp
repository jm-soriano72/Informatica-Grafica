#include "BOX.h"
#include <IGL/IGlib.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;
using namespace std;

//Idenficadores de los objetos de la escena

int objId = -1;
int objId2 = -2;
int objId3 = -3;

//Declaraci�n de variables globales

int posicionPreMov_x = 0; //Variable utilizada en la c�mara orbital implementada en el rat�n, que almacena la posici�n en X antes del movimiento
int posicionPreMov_y = 0; //Variable utilizada en la c�mara orbital implementada en el rat�n, que almacena la posici�n en Y antes del movimiento

//Declaraci�n de CB

void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);
void mouseMotionFunc(int x, int y);

//Declaraci�n de la matriz de vista y la matriz de proyecci�n de manera global, para poder ser utilizadas en las funciones

mat4 proj = mat4(1.0f);
mat4 view = mat4(1.0f);


int main(int argc, char** argv)
{
	locale::global(locale("spanish"));// acentos ;)
	if (!IGlib::init("../shaders_P1/shader.v11.vert", "../shaders_P1/shader.v11.frag"))
		return -1;

	//CBs
	IGlib::setResizeCB(resizeFunc);
	IGlib::setIdleCB(idleFunc);
	IGlib::setKeyboardCB(keyboardFunc);
	IGlib::setMouseCB(mouseFunc);
	IGlib::setMouseMoveCB(mouseMotionFunc);
   
	//Se ajusta la c�mara

	view[3].z = -6;

	//Definiendo la matriz de proyecci�n, calculando todas las variables que intervienen (proyecci�n perspectiva). Es mejor definirla en el main, porque as� se pasa la matriz directamente a cada uno de los v�rtices y se ahorra c�lculo

	float f = 1.0f / tan(3.141592 / 6);
	float far = 20.0;
	float near = 0.1;

	proj[0].x = f;
	proj[1].y = f;
	proj[2].z = (far + near) / (near - far);
	proj[2].w = -1.0f;
	proj[3].z = (2.0f * far * near)/(near - far);
	proj[3].w = 0.0;


	IGlib::setProjMat(proj);
	IGlib::setViewMat(view);



	//Creamos los distintos objetos de la escena

	objId = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex, 
			cubeVertexPos, cubeVertexColor, cubeVertexNormal,cubeVertexTexCoord, cubeVertexTangent);

	objId2 = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex,
		cubeVertexPos, cubeVertexColor, cubeVertexNormal, cubeVertexTexCoord, cubeVertexTangent);

	objId3 = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, pyramidTriangleIndex,
		pyramidVertexPos, pyramidVertexColor, pyramidVertexNormal, pyramidVertexTexCoord, pyramidVertexTangent);
		

	//Incluir texturas aqu�.
	IGlib::addColorTex(objId, "../img/color.png");
	

	//Mainloop
	IGlib::mainLoop();
	IGlib::destroy();
	return 0;
}

void resizeFunc(int width, int height)
{
	//Ajusta el aspect ratio al tama�o de la ventana

	float aspect_ratio = ((float) width / (float) height); // Se define el aspect ratio dividiendo el ancho entre el alto de la pantalla
	proj[0].x = 1.0 / (aspect_ratio * (tan(3.1416 / 6))); // Se actualizan las coordenadas de la matriz en funci�n del aspect ratio
	proj[1].y = 1.0 / (tan(3.1416 / 6));

	IGlib::setProjMat(proj);
}

void idleFunc()
{
	static float angle = 0.0f; //El valor del �ngulo se va acumulando por ser un tipo de variable est�tica
	angle = (angle < 3.141599f * 2.0f) ? angle + 0.01f : 0.0f; //Si el �ngulo es menor a 2PI se suma 0,0001 y si no se inicializa a 0 de nuevo

	// Creaci�n de las matrices de transformaci�n de cada uno de los objetos

	mat4 modelMat = mat4(1.0f);
	mat4 modelMat2 = mat4(1.0f);
	mat4 modelMat3 = mat4(1.0f);

	// Transformaciones afines de los objetos en escena

	// CUBO 1 //

	modelMat = rotate(modelMat, angle, vec3(0.0f, 1.0f, 0.0f)); //
	modelMat = translate(modelMat, vec3(0.0f, 1.0f, 1.0f));

	//Actualizaci�n de la matriz de modelo del cubo 1

	IGlib::setModelMat(objId, modelMat);

	// CUBO 2 //

	modelMat2 = translate(modelMat2, vec3(modelMat[0].x, modelMat[1].y, modelMat[2].z)); // Traslaci�n alrededor del primer cubo, con las coordenadas de su matriz de modelo
	modelMat2 = rotate(modelMat2, angle, vec3(0.0f, 1.0f, 1.0f)); // Rotaci�n sobre s� mismo y alrededor del cubo 1
	modelMat2 = translate(modelMat2, vec3(3.0f, 0.0f, 0.0f)); // Traslaci�n alrededor del primer cubo a una distancia de 3

	//Actualizaci�n de la matriz de modelo del cubo 2

	IGlib::setModelMat(objId2, modelMat2);


	// PIR�MIDE //

	modelMat3 = translate(modelMat3, vec3(modelMat2[0].x, modelMat2[1].y, modelMat2[2].z));  //Traslaci�n alrededor del segundo cubo, a trav�s de sus coordenadas
	modelMat3 = rotate(modelMat3, angle, vec3(0.0f, 1.0f, 1.0f));
	modelMat3 = translate(modelMat3, vec3(0.0f, 0.0f, -2.0f));

	//Actualizaci�n de la matriz de modelo de la pir�mide

	IGlib::setModelMat(objId3, modelMat3);

}


void keyboardFunc(unsigned char key, int x, int y)
{
	std::cout << "Se ha pulsado la tecla " << key << endl << endl;

	//Creaci�n de una matriz auxiliar que recibir� la transformaci�n que se le indique seg�n la tecla pulsada

	mat4 auxiliar = mat4(1.0);

	switch (key) {

	case 'w':

		auxiliar = translate(auxiliar, vec3(0.0f, 0.0f, -0.2f));   // La c�mara se mueve hacia el interior en el eje Z
		break;
	
	case 'a':

		auxiliar = translate(auxiliar, vec3(-0.2f, 0.0f, 0.0f));   // La c�mara se mueve hacia la izquierda, en el sentido negativo del eje X
		break;

	case 's':

		auxiliar = translate(auxiliar, vec3(0.0f, 0.0f, 0.2f));   // La c�mara se mueve hacia el exterior en el eje Z
		break;

	case 'd':

		auxiliar = translate(auxiliar, vec3(0.2f, 0.0f, 0.0f));   // La c�mara se mueve hacia la derecha, en el sentido positivo del eje X
		break;

	case 'q':

		auxiliar = rotate(auxiliar, 0.01f, vec3(0.0f, -0.1f, 0.0f));   // La c�mara gira en el eje Y, en sentido negativo
		break;

	case 'e':

		auxiliar = rotate(auxiliar, 0.01f, vec3(0.0f, 0.1f, 0.0f));   // La c�mara gira en el eje Y, en sentido positivo
		break;
	}

	// Actualizaci�n de la matriz de vista con la transformaci�n aplicada en la matriz auxiliar

	view = auxiliar*view;
	IGlib::setViewMat(view);

}

void mouseFunc(int button, int state, int x, int y)
{

	if (state==0)
		cout << "Se ha pulsado el bot�n ";
	else
		cout << "Se ha soltado el bot�n ";

	// Creaci�n de una matriz auxiliar, que recibe la transformaci�n dependiendo de la tecla del rat�n pulsada

	mat4 auxiliar = mat4(1.0);
	
	if (button == 0) {
		cout << "de la izquierda del rat�n " << endl;
		auxiliar = rotate(auxiliar, 0.01f, vec3(0.0f, -1.0f, 0.0f));   // La c�mara gira en el eje Y, en sentido negativo
	}

	if (button == 1) {
		auxiliar = translate(auxiliar, vec3(0.0f, 0.0f, 0.5f));   // La c�mara se mueve hacia el interior en el eje Z
		cout << "central del rat�n " << endl;
	}
	if (button == 2) {
		cout << "de la derecha del rat�n " << endl;
		auxiliar = rotate(auxiliar, 0.01f, vec3(0.0f, 1.0f, 0.0f));   // La c�mara gira en el eje Y, en sentido positivo
	}

	cout << "en la posici�n " << x << " " << y << endl << endl;

	// Actualizaci�n de la matriz de vista con la transformaci�n aplicada en la matriz auxiliar

	view = auxiliar * view;
	IGlib::setViewMat(view);
}

void mouseMotionFunc(int x, int y)
{
	mat4 auxiliar = mat4(1.0);

	if (x < posicionPreMov_x) {
		auxiliar = rotate(auxiliar, 0.01f, vec3(0.0, -1.0, 0.0));    // Si se pulsa el rat�n a la izquierda, hacia el lado negativo de las X, la c�mara rota hacia la izquierda
	}
	else if (x > posicionPreMov_x) {
		auxiliar = rotate(auxiliar, 0.01f, vec3(0.0, 1.0, 0.0));     // Si se pulsa el rat�n a la derecha, hacia el lado positivo de las X, la c�mara rota hacia la derecha
	}

	// Se actualiza la posici�n en X
	posicionPreMov_x = x;

	if (y < posicionPreMov_y) {
		auxiliar = rotate(auxiliar, 0.01f, vec3(-1.0, 0.0, 0.0));    // Si se pulsa el rat�n abajo, hacia el lado negativo de las y, la c�mara rota hacia abajo
	}
	else if (y > posicionPreMov_y) {
		auxiliar = rotate(auxiliar, 0.01f, vec3(1.0, 0.0, 0.0));     // Si se pulsa el rat�n arriba, hacia el lado positivo de las y, la c�mara rota hacia arriba
	}

	// Se actualiza la posici�n en Y
	posicionPreMov_y = y;

	// Se actualiza la c�mara
	view = auxiliar * view;
	IGlib::setViewMat(view);
}
