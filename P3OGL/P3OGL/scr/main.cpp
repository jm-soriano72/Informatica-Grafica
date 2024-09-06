#include "BOX.h"
#include "auxiliar.h"


#include <gl/glew.h>
#define SOLVE_FGLUT_WARNING
#include <gl/freeglut.h> 

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

using namespace std;
using namespace glm;


//////////////////////////////////////////////////////////////
// Datos que se almacenan en la memoria de la CPU
//////////////////////////////////////////////////////////////

//Matrices
glm::mat4	proj = glm::mat4(1.0f);
glm::mat4	view = glm::mat4(1.0f);
glm::mat4	model = glm::mat4(1.0f);
glm::mat4	model2 = glm::mat4(1.0f);


//Vectores que almacenan la posición y la intensidad de la luz
glm::vec3 lightPos = glm::vec3(0.0, 0.0, 4.0);
glm::vec3 lightInt = glm::vec3(1.0);


//////////////////////////////////////////////////////////////
// Variables que nos dan acceso a Objetos OpenGL
//////////////////////////////////////////////////////////////

// A través de estas variables se accede a los distintos elementos de los shaders

// Shaders 1

unsigned int vshader;
unsigned int fshader;
unsigned int program;

// Shaders 2

unsigned int vshader2;
unsigned int fshader2;
unsigned int program2;


// Variables uniformes de los shaders (matrices)

int uModelViewMat;
int uModelViewProjMat;
int uNormalMat;
int uView;

// Variables para modificar la posición y la intensidad de la luz

int uLightPosition;
int uLightIntensity;

// Variables uniformes de la segunda pareja de shaders

int uModelViewMat2;
int uModelViewProjMat2;
int uNormalMat2;
int uView2;
int uColorTex2;
int uSpecularTex2;
int uEmissiveTex2;


// Variables para modificar la posición y la intensidad de la luz
	
int uLightPosition2;
int uLightIntensity2;

// Texturas shaders 1

int uColorTex;
int uEmiTex;
int uSpecularTex;

// Atributos shaders 1

int inPos;
int inColor;
int inNormal;
int inTexCoord;

// Atributos shaders 2

int inPos2;
int inColor2;
int inNormal2;
int inTexCoord2;

// Se crean dos variables que permitan configurar 2 texturas

unsigned int colorTexId;
unsigned int emiTexId;
unsigned int specularTexId;

// Variables de textura de los shaders 2

unsigned int colorTexId2;
unsigned int specularTexId2;
unsigned int emissiveTexId2;


//VAOs

unsigned int vao;
unsigned int vao2;
  
//VBOs que forman parte del objeto
unsigned int posVBO;
unsigned int colorVBO;
unsigned int normalVBO;
unsigned int texCoordVBO;
unsigned int triangleIndexVBO;




//////////////////////////////////////////////////////////////
// Funciones auxiliares
//////////////////////////////////////////////////////////////


//Declaración de CB
void renderFunc();
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);

//Funciones de inicialización y destrucción
void initContext(int argc, char** argv);
void initOGL();
void initShader(const char *vname, const char *fname);
void initShader2(const char* vname, const char* fname); // Función para inicializar la segunda pareja de shaders
void initObj();
void destroy();


//Carga el shader indicado, devuele el ID del shader
GLuint loadShader(const char *fileName, GLenum type);

//Crea una textura, la configura, la sube a OpenGL, 
//y devuelve el identificador de la textura 
unsigned int loadTex(const char *fileName);


int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)

	initContext(argc, argv);
	initOGL();

	// Se inicializan los shaders, llamando a sus respectivas funciones por parejas

	initShader("../shaders_P3/shader.v2.vert", "../shaders_P3/shader.v2.frag");
	initShader2("../shaders_P3/shader.v3.vert", "../shaders_P3/shader.v3.frag");
	initObj();

	glutMainLoop();

	destroy();

	return 0;
}
	
//////////////////////////////////////////
// Funciones auxiliares 

void initContext(int argc, char** argv){ 
	//Crea el contexto e inicializa la librería Glut
	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);

	//glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	
	//Definición del FrameBuffer y creación de la ventana
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Prácticas OGL");
	glewExperimental = GL_TRUE;

	//Inicialización de las extensiones
	GLenum err = glewInit();
	if (GLEW_OK != err) //Código de error en la función Glew
	{
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}
	const GLubyte* oglVersion = glGetString(GL_VERSION);
	std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;

	//Definir funciones callback según el tipo de evento
	glutReshapeFunc(resizeFunc);
	glutDisplayFunc(renderFunc);
	glutIdleFunc(idleFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
}

void initOGL(){

	//Se activa el test de profundidad y se indica el color del escenario
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Indica la orientación de la cara front, configura la etapa de rasterizado y activa el culling
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	//Define la matriz de proyección
	proj = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 50.0f);

	//Define la matriz de vista
	view = glm::mat4(1.0f);
	view[3].z = -6;

}
void destroy(){
	// Se liberan los recursos utilizados
	glDetachShader(program, vshader);
	glDetachShader(program, fshader);
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	glDeleteProgram(program);

	// Libera los recursos utilizados
	if (inPos != -1) glDeleteBuffers(1, &posVBO);
	if (inColor != -1) glDeleteBuffers(1, &colorVBO);
	if (inNormal != -1) glDeleteBuffers(1, &normalVBO);
	if (inTexCoord != -1) glDeleteBuffers(1, &texCoordVBO);
	glDeleteBuffers(1, &triangleIndexVBO);
	glDeleteVertexArrays(1, &vao);

	glDeleteTextures(1, &colorTexId);
	glDeleteTextures(1, &emiTexId);


}

void initShader(const char *vname, const char *fname){

	// Creación de un shader de vértices y un shader de fragmentos
	vshader = loadShader(vname, GL_VERTEX_SHADER);
	fshader = loadShader(fname, GL_FRAGMENT_SHADER);
	// Enlaza los dos shaders en un mismo programa (program está definido para el objeto)
	program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);

	// Se asigna un índice a cada uno de los atributos del shader
	glBindAttribLocation(program, 0, "inPos");
	glBindAttribLocation(program, 1, "inColor");
	glBindAttribLocation(program, 2, "inNormal");
	glBindAttribLocation(program, 3, "inTexCoord");
	
	// Comprueba los errores de la fase de enlazado
	int linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		char* logString = new char[logLen];
		glGetProgramInfoLog(program, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete[] logString;
		glDeleteProgram(program);
		program = 0;
		exit(-1);
	}

	// Creas los identificadores de las variables uniformes
	uNormalMat = glGetUniformLocation(program, "normal");
	uModelViewMat = glGetUniformLocation(program, "modelView");
	uModelViewProjMat = glGetUniformLocation(program, "modelViewProj");
	uView = glGetUniformLocation(program, "view");

	// Crea los identificadores de las variables uniformes de la luz, para poder acceder a ellas desde los shaders
	uLightPosition = glGetUniformLocation(program, "lightPosition");
	uLightIntensity = glGetUniformLocation(program, "lightIntensity");

	// Creas las variables uniformes de las texturas
	uColorTex = glGetUniformLocation(program, "colorTex");
	uEmiTex = glGetUniformLocation(program, "emiTex");
	uSpecularTex = glGetUniformLocation(program, "specTex"); // Identificador de la textura especular

	//Crea los identificadores de los atributos
	inPos = glGetAttribLocation(program, "inPos");
	inColor = glGetAttribLocation(program, "inColor");
	inNormal = glGetAttribLocation(program, "inNormal");
	inTexCoord = glGetAttribLocation(program, "inTexCoord");

}

// Función para cargar la segunda pareja de shaders. Es exactamente igual que initShader(), pero añadiendo un 2 a las variables definidas

void initShader2(const char* vname, const char* fname) {

	// Se crea la segunda pareja de shaders

	vshader2 = loadShader(vname, GL_VERTEX_SHADER);
	fshader2 = loadShader(fname, GL_FRAGMENT_SHADER);

	// Creación de un segundo programa, que enlaza la segunda pareja de shaders

	program2 = glCreateProgram();
	glAttachShader(program2, vshader2);
	glAttachShader(program2, fshader2);


	glBindAttribLocation(program2, 0, "inPos");
	glBindAttribLocation(program2, 1, "inColor");
	glBindAttribLocation(program2, 2, "inNormal");
	glBindAttribLocation(program2, 3, "inTexCoord");


	glLinkProgram(program2);


	GLint linked;
	glGetProgramiv(program2, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(program2, GL_INFO_LOG_LENGTH, &logLen);
		char* logString = new char[logLen];
		glGetProgramInfoLog(program2, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete logString;
		glDeleteProgram(program2);
		program2 = 0;
		exit(-1);
	}


	// Se crean los identificadores de todas las variables 

	uNormalMat2 = glGetUniformLocation(program2, "normal");
	uModelViewMat2 = glGetUniformLocation(program2, "modelView");
	uModelViewProjMat2 = glGetUniformLocation(program2, "modelViewProj");
	uView2 = glGetUniformLocation(program2, "view");

	uColorTex2 = glGetUniformLocation(program2, "colorTex");
	uSpecularTex2 = glGetUniformLocation(program2, "specTex");
	uEmissiveTex2 = glGetUniformLocation(program2, "emiTex");

	uLightPosition2 = glGetUniformLocation(program2, "lightPosition");
	uLightIntensity2 = glGetUniformLocation(program2, "lightIntensity");

	inPos2 = glGetAttribLocation(program2, "inPos");
	inColor2 = glGetAttribLocation(program2, "inColor");
	inNormal2 = glGetAttribLocation(program2, "inNormal");
	inTexCoord2 = glGetAttribLocation(program2, "inTexCoord");

}


void initObj(){

	// Crea y activa el VAO en el que se almacenará la configuración del objeto
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Atributos de la malla del objeto 1 //

	if (inPos != -1)
	{
		glGenBuffers(1, &posVBO);
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
			cubeVertexPos, GL_STATIC_DRAW);
		glVertexAttribPointer(inPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inPos);
	}
	if (inColor != -1)
	{
		glGenBuffers(1, &colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
			cubeVertexColor, GL_STATIC_DRAW);
		glVertexAttribPointer(inColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inColor);
	}
	if (inNormal != -1)
	{
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
			cubeVertexNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(inNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inNormal);
	}
	if (inTexCoord != -1)
	{
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 2,
			cubeVertexTexCoord, GL_STATIC_DRAW);
		glVertexAttribPointer(inTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inTexCoord);
	}


	// Se crea la lista de índices que apunten a los elementos definidos arriba

	glGenBuffers(1, &triangleIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		cubeNTriangleIndex * sizeof(unsigned int) * 3, cubeTriangleIndex,
		GL_STATIC_DRAW);

	//Se cargan las texturas
	colorTexId = loadTex("../img/color2.png");
	emiTexId = loadTex("../img/emissive.png");
	specularTexId = loadTex("../img/specMap.png");

	// Inicializamos la matriz de modelado del objeto
	model = glm::mat4(1.0f);


	// INICIALIZACIÓN DEL OBJETO 2 // 

	glGenVertexArrays(2, &vao2);
	glBindVertexArray(vao2);

	if (inPos2 != -1)
	{
		glGenBuffers(1, &posVBO);
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
			cubeVertexPos, GL_STATIC_DRAW);
		glVertexAttribPointer(inPos2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inPos2);
	}
	if (inColor2 != -1)
	{
		glGenBuffers(1, &colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
			cubeVertexColor, GL_STATIC_DRAW);
		glVertexAttribPointer(inColor2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inColor2);
	}
	if (inNormal2 != -1)
	{
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
			cubeVertexNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(inNormal2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inNormal2);
	}
	if (inTexCoord2 != -1)
	{
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 2,
			cubeVertexTexCoord, GL_STATIC_DRAW);
		glVertexAttribPointer(inTexCoord2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inTexCoord2);
	}

	glGenBuffers(1, &triangleIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		cubeNTriangleIndex * sizeof(unsigned int) * 3, cubeTriangleIndex,
		GL_STATIC_DRAW);

	model2 = glm::mat4(1.0f);

	colorTexId2 = loadTex("../img/color4.jpg");
	specularTexId2 = loadTex("../img/specMap3.png");

}

GLuint loadShader(const char *fileName, GLenum type){ 
	
	unsigned int fileLen;
	char* source = loadStringFromFile(fileName, fileLen);
	//////////////////////////////////////////////
	//Creación y compilación del Shader
	GLuint shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1,
		(const GLchar**)&source, (const GLint*)&fileLen);
	glCompileShader(shader);
	delete[] source; //Delete array new

	//Comprobamos que se compiló bien
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		char* logString = new char[logLen];
		glGetShaderInfoLog(shader, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete[] logString;

		glDeleteShader(shader);
		exit(-1);
	}

	return shader; //Se devuelve el ID del shader creado
}


unsigned int loadTex(const char *fileName){ 

	unsigned char* map;
	unsigned int w, h;
	map = loadTexture(fileName, w, h);

	if (!map)
	{
		std::cout << "Error cargando el fichero: "
			<< fileName << std::endl;
		exit(-1);
	}

	unsigned int texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, (GLvoid*)map);

	// Se genera el mipmap
	glGenerateMipmap(GL_TEXTURE_2D);
	// Configura el método de acceso
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);

	delete[] map;

	// Filtro anisotrópico

	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);



	// Devuelve el ID de la textura
	return texId;

}

void renderFunc() { 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Limpia el buffer de color y el buffer de profundidad antes de cada renderizado

	model2 = glm::translate(model, glm::vec3(2.5f));

	glUseProgram(program); //Se activa el uso del programa

	// Se pasan las variables de la luz al shader. Se relacionan los vectores declarados arriba con las variables accesibles desde los shaders

	if (uLightIntensity != -1) glUniform3fv(uLightIntensity, 1, &(lightInt.x));
	if (uLightPosition != -1) glUniform3fv(uLightPosition, 1, &(lightPos.x));

	/// OBJETO 1 ///

	// Se calculan las matrices necesarias para la práctica (en CPU)
	glm::mat4 modelView = view * model;
	glm::mat4 modelViewProj = proj * view * model;
	glm::mat4 normal = glm::transpose(glm::inverse(modelView));

	// De esta forma se pasan las matrices como variables uniformes a los shaders
	if (uModelViewMat != -1)
		glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE,
			&(modelView[0][0]));
	if (uModelViewProjMat != -1)
		glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE,
			&(modelViewProj[0][0]));
	if (uNormalMat != -1)
		glUniformMatrix4fv(uNormalMat, 1, GL_FALSE,
			&(normal[0][0]));
	if (uView != -1)
		glUniformMatrix4fv(uView, 1, GL_FALSE,
			&(view[0][0]));

	// Se activan las texturas y se enlazan con el programa

	if (uColorTex != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorTexId);
		glUniform1i(uColorTex, 0);
	}

	if (uEmiTex != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, emiTexId);
		glUniform1i(uEmiTex, 1);
	}

	if (uSpecularTex != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, specularTexId);
		glUniform1i(uSpecularTex, 2);
	}




	// Se activa el VAO con la configuración del objeto y pinta la lista de triángulos

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3, GL_UNSIGNED_INT, (void*)0);


	/// OBJETO 2 ///

	glUseProgram(program2);
	modelView = view * model2;
	modelViewProj = proj * view * model2;
	normal = glm::transpose(glm::inverse(modelView));

	//Las subimos al shader
	if (uModelViewMat2 != -1) glUniformMatrix4fv(uModelViewMat2, 1, GL_FALSE, &(modelView[0][0]));//(Matriz,id,no trasponer, dirección del primer elemento)
	if (uModelViewProjMat2 != -1) glUniformMatrix4fv(uModelViewProjMat2, 1, GL_FALSE, &(modelViewProj[0][0]));//(Matriz,id,no trasponer, dirección del primer elemento)
	if (uNormalMat2 != -1) glUniformMatrix4fv(uNormalMat2, 1, GL_FALSE, &(normal[0][0]));
	if (uView2 != -1)
		glUniformMatrix4fv(uView2, 1, GL_FALSE,
			&(view[0][0]));

	if (uLightIntensity2 != -1) glUniform3fv(uLightIntensity2, 1, &(lightInt.x));
	if (uLightPosition2 != -1) glUniform3fv(uLightPosition2, 1, &(lightPos.x));

	if (uColorTex2 != -1) {
		glActiveTexture(GL_TEXTURE0+3);//Activamos la textura
		glBindTexture(GL_TEXTURE_2D, colorTexId2);//le asociamos la textura
		glUniform1i(uColorTex2, 3);
	}


	if (uSpecularTex2 != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, specularTexId2);
		glUniform1i(uSpecularTex2, 4);
	}

	if (uEmissiveTex2 != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_2D, emissiveTexId2);
		glUniform1i(uEmissiveTex2, 5);
	}


	glBindVertexArray(vao2); // Activar el VAO de la configuración de los atributos
	glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3, GL_UNSIGNED_INT, (void*)0);

	glutSwapBuffers();
}


void resizeFunc(int width, int height){

	// Se ajusta el viewport
	glViewport(0, 0, width, height);

	// Se calcula el aspect ratio, para conservarlo
	float aspectratio = (float(width) / float(height));

	// Se ajusta la matriz de proyección al aspect ratio
	proj[0].x = 1.0 / (aspectratio * (tan(3.1415 / 6)));
	proj[1].y = 1.0 / (tan(3.1415 / 6));

	//Se planifica un evento de renderizado
	glutPostRedisplay();


}
void idleFunc(){
	// Se añade la animación del cubo
	model = glm::mat4(1.0f);
	static float angle = 0.0f;
	angle = (angle > 3.141592f * 2.0f) ? 0 : angle + 0.01f;
	model = glm::rotate(model, angle, glm::vec3(1.0f, 1.0f, 0.0f));

	glutPostRedisplay();
}
void keyboardFunc(unsigned char key, int x, int y){

	// Se crea una matriz auxiliar que almacene las transformaciones de entrada por el teclado

	glm::mat4 viewAux = glm::mat4(1.0f);

	std::cout << "Se ha pulsado la tecla " << key << std::endl << std::endl;

	switch (key) {
	case 'w':
		viewAux[3].z = viewAux[3].z + 2.0f;
		break;
	case 'a':
		viewAux[3].x = viewAux[3].x + 2.0f;
		break;
	case 's':
		viewAux[3].z = viewAux[3].z - 2.0f;
		break;
	case 'd':
		viewAux[3].x = viewAux[3].x - 2.0f;
		break;
	case 'q':
		viewAux = glm::rotate(viewAux, 0.1f, glm::vec3(0.0, -1.0, 0.0));
		break;
	case 'e':
		viewAux = glm::rotate(viewAux, 0.1f, glm::vec3(0.0, 1.0, 0.0));
		break;
	case 'j':
		//Mueve la luz hacia la izquierda
		lightPos = lightPos + glm::vec3(-1.0, 0, 0);
		break;
	case 'k':
		//Mueve la luz hacia abajo
		lightPos = lightPos + glm::vec3(0, -1.0, 0);
		break;
	case 'l':
		//Mueve la luz hacia la derecha
		lightPos = lightPos + glm::vec3(1.0, 0, 0);
		break;
	case 'i':
		//Mueve la luz hacia arriba
		lightPos = lightPos + glm::vec3(0, 1.0, 0);
		break;
	case 'u':
		//Mueve la luz hacia atras
		lightPos = lightPos + glm::vec3(0, 0, 1.0);
		break;
	case 'o':
		//Mueve la luz hacia delante
		lightPos = lightPos + glm::vec3(0, 0, -1.0);
		break;
	case 'y':
		if (lightInt.x < 1.5)lightInt += glm::vec3(0.1, 0.1, 0.1);
		break;
	case 'h':
		if (lightInt.x > 0)lightInt -= glm::vec3(0.1, 0.1, 0.1);
		break;
	}

	view = viewAux * view;

	glutPostRedisplay();



}

// Función que regula la cámara en función del ratón

void mouseFunc(int button, int state, int x, int y)
{

	if (state == 0)
		cout << "Se ha pulsado el botón ";
	else
		cout << "Se ha soltado el botón ";

	// Creación de una matriz auxiliar, que recibe la transformación dependiendo de la tecla del ratón pulsada

	mat4 auxiliar = mat4(1.0);

	if (button == 0) {
		cout << "de la izquierda del ratón " << endl;
		auxiliar = rotate(auxiliar, 0.01f, vec3(0.0f, -3.0f, 0.0f));   // La cámara gira en el eje Y, en sentido negativo
		view = auxiliar * view;
	}

	if (button == 1) {
		auxiliar = translate(auxiliar, vec3(0.0f, 0.0f, 0.5f));   // La cámara se mueve hacia el interior en el eje Z
		view = auxiliar * view;
		cout << "central del ratón " << endl;
	}

	if (button == 2) {
		cout << "de la derecha del ratón " << endl;
		auxiliar = rotate(auxiliar, 0.01f, vec3(0.0f, 3.0f, 0.0f));   // La cámara gira en el eje Y, en sentido positivo
		view = auxiliar * view;
	}

	cout << "en la posición " << x << " " << y << endl << endl;

	glutPostRedisplay();
}











