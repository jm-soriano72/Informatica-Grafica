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

//Declaración de variables globales

int posicionPreMov_x = 0; //Variable utilizada en la cámara orbital implementada en el ratón, que almacena la posición en X antes del movimiento
int posicionPreMov_y = 0; //Variable utilizada en la cámara orbital implementada en el ratón, que almacena la posición en Y antes del movimiento

//Declaración de CB

void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);
void mouseMotionFunc(int x, int y);

//Declaración de la matriz de vista y la matriz de proyección de manera global, para poder ser utilizadas en las funciones

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
   
	//Se ajusta la cámara

	view[3].z = -6;

	//Definiendo la matriz de proyección, calculando todas las variables que intervienen (proyección perspectiva). Es mejor definirla en el main, porque así se pasa la matriz directamente a cada uno de los vértices y se ahorra cálculo

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
		

	//Incluir texturas aquí.
	IGlib::addColorTex(objId, "../img/color.png");
	

	//Mainloop
	IGlib::mainLoop();
	IGlib::destroy();
	return 0;
}

void resizeFunc(int width, int height)
{
	//Ajusta el aspect ratio al tamaño de la ventana

	float aspect_ratio = ((float) width / (float) height); // Se define el aspect ratio dividiendo el ancho entre el alto de la pantalla
	proj[0].x = 1.0 / (aspect_ratio * (tan(3.1416 / 6))); // Se actualizan las coordenadas de la matriz en función del aspect ratio
	proj[1].y = 1.0 / (tan(3.1416 / 6));

	IGlib::setProjMat(proj);
}

void idleFunc()
{
	static float angle = 0.0f; //El valor del ángulo se va acumulando por ser un tipo de variable estática
	angle = (angle < 3.141599f * 2.0f) ? angle + 0.01f : 0.0f; //Si el ángulo es menor a 2PI se suma 0,0001 y si no se inicializa a 0 de nuevo

	// Creación de las matrices de transformación de cada uno de los objetos

	mat4 modelMat = mat4(1.0f);
	mat4 modelMat2 = mat4(1.0f);
	mat4 modelMat3 = mat4(1.0f);

	// Transformaciones afines de los objetos en escena

	// CUBO 1 //

	modelMat = rotate(modelMat, angle, vec3(0.0f, 1.0f, 0.0f)); //
	modelMat = translate(modelMat, vec3(0.0f, 1.0f, 1.0f));

	//Actualización de la matriz de modelo del cubo 1

	IGlib::setModelMat(objId, modelMat);

	// CUBO 2 //

	modelMat2 = translate(modelMat2, vec3(modelMat[0].x, modelMat[1].y, modelMat[2].z)); // Traslación alrededor del primer cubo, con las coordenadas de su matriz de modelo
	modelMat2 = rotate(modelMat2, angle, vec3(0.0f, 1.0f, 1.0f)); // Rotación sobre sí mismo y alrededor del cubo 1
	modelMat2 = translate(modelMat2, vec3(3.0f, 0.0f, 0.0f)); // Traslación alrededor del primer cubo a una distancia de 3

	//Actualización de la matriz de modelo del cubo 2

	IGlib::setModelMat(objId2, modelMat2);


	// PIRÁMIDE //

	modelMat3 = translate(modelMat3, vec3(modelMat2[0].x, modelMat2[1].y, modelMat2[2].z));  //Traslación alrededor del segundo cubo, a través de sus coordenadas
	modelMat3 = rotate(modelMat3, angle, vec3(0.0f, 1.0f, 1.0f));
	modelMat3 = translate(modelMat3, vec3(0.0f, 0.0f, -2.0f));

	//Actualización de la matriz de modelo de la pirámide

	IGlib::setModelMat(objId3, modelMat3);

}


void keyboardFunc(unsigned char key, int x, int y)
{
	std::cout << "Se ha pulsado la tecla " << key << endl << endl;

	//Creación de una matriz auxiliar que recibirá la transformación que se le indique según la tecla pulsada

	mat4 auxiliar = mat4(1.0);

	switch (key) {

	case 'w':

		auxiliar = translate(auxiliar, vec3(0.0f, 0.0f, -0.2f));   // La cámara se mueve hacia el interior en el eje Z
		break;
	
	case 'a':

		auxiliar = translate(auxiliar, vec3(-0.2f, 0.0f, 0.0f));   // La cámara se mueve hacia la izquierda, en el sentido negativo del eje X
		break;

	case 's':

		auxiliar = translate(auxiliar, vec3(0.0f, 0.0f, 0.2f));   // La cámara se mueve hacia el exterior en el eje Z
		break;

	case 'd':

		auxiliar = translate(auxiliar, vec3(0.2f, 0.0f, 0.0f));   // La cámara se mueve hacia la derecha, en el sentido positivo del eje X
		break;

	case 'q':

		auxiliar = rotate(auxiliar, 0.01f, vec3(0.0f, -0.1f, 0.0f));   // La cámara gira en el eje Y, en sentido negativo
		break;

	case 'e':

		auxiliar = rotate(auxiliar, 0.01f, vec3(0.0f, 0.1f, 0.0f));   // La cámara gira en el eje Y, en sentido positivo
		break;
	}

	// Actualización de la matriz de vista con la transformación aplicada en la matriz auxiliar

	view = auxiliar*view;
	IGlib::setViewMat(view);

}

void mouseFunc(int button, int state, int x, int y)
{

	if (state==0)
		cout << "Se ha pulsado el botón ";
	else
		cout << "Se ha soltado el botón ";

	// Creación de una matriz auxiliar, que recibe la transformación dependiendo de la tecla del ratón pulsada

	mat4 auxiliar = mat4(1.0);
	
	if (button == 0) {
		cout << "de la izquierda del ratón " << endl;
		auxiliar = rotate(auxiliar, 0.01f, vec3(0.0f, -1.0f, 0.0f));   // La cámara gira en el eje Y, en sentido negativo
	}

	if (button == 1) {
		auxiliar = translate(auxiliar, vec3(0.0f, 0.0f, 0.5f));   // La cámara se mueve hacia el interior en el eje Z
		cout << "central del ratón " << endl;
	}
	if (button == 2) {
		cout << "de la derecha del ratón " << endl;
		auxiliar = rotate(auxiliar, 0.01f, vec3(0.0f, 1.0f, 0.0f));   // La cámara gira en el eje Y, en sentido positivo
	}

	cout << "en la posición " << x << " " << y << endl << endl;

	// Actualización de la matriz de vista con la transformación aplicada en la matriz auxiliar

	view = auxiliar * view;
	IGlib::setViewMat(view);
}

void mouseMotionFunc(int x, int y)
{
	mat4 auxiliar = mat4(1.0);

	if (x < posicionPreMov_x) {
		auxiliar = rotate(auxiliar, 0.01f, vec3(0.0, -1.0, 0.0));    // Si se pulsa el ratón a la izquierda, hacia el lado negativo de las X, la cámara rota hacia la izquierda
	}
	else if (x > posicionPreMov_x) {
		auxiliar = rotate(auxiliar, 0.01f, vec3(0.0, 1.0, 0.0));     // Si se pulsa el ratón a la derecha, hacia el lado positivo de las X, la cámara rota hacia la derecha
	}

	// Se actualiza la posición en X
	posicionPreMov_x = x;

	if (y < posicionPreMov_y) {
		auxiliar = rotate(auxiliar, 0.01f, vec3(-1.0, 0.0, 0.0));    // Si se pulsa el ratón abajo, hacia el lado negativo de las y, la cámara rota hacia abajo
	}
	else if (y > posicionPreMov_y) {
		auxiliar = rotate(auxiliar, 0.01f, vec3(1.0, 0.0, 0.0));     // Si se pulsa el ratón arriba, hacia el lado positivo de las y, la cámara rota hacia arriba
	}

	// Se actualiza la posición en Y
	posicionPreMov_y = y;

	// Se actualiza la cámara
	view = auxiliar * view;
	IGlib::setViewMat(view);
}
