#include "BOX.h"
#include "auxiliar.h"
#include "PLANE.h"

#include <gl/glew.h>
#define SOLVE_FGLUT_WARNING
#include <gl/freeglut.h> 

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cstdlib>

#define RAND_SEED 31415926
#define SCREEN_SIZE 500,500
#define MASK_SIZE 25

//////////////////////////////////////////////////////////////
// Datos que se almacenan en la memoria de la CPU
//////////////////////////////////////////////////////////////

//Matrices
glm::mat4	proj = glm::mat4(1.0f);
glm::mat4	view = glm::mat4(1.0f);
glm::mat4	model = glm::mat4(1.0f);


//////////////////////////////////////////////////////////////
// Variables que nos dan acceso a Objetos OpenGL
//////////////////////////////////////////////////////////////

float angle = 0.0f;

//VAO
unsigned int vao;

//VBOs que forman parte del objeto
unsigned int posVBO;
unsigned int colorVBO;
unsigned int normalVBO;
unsigned int texCoordVBO;
unsigned int triangleIndexVBO;

unsigned int colorTexId;
unsigned int emiTexId;

//Por definir
unsigned int vshader;
unsigned int fshader;
unsigned int program;

//Variables Uniform 
int uModelViewMat;
int uModelViewProjMat;
int uNormalMat;

//Texturas Uniform
int uColorTex;
int uEmiTex;

//Variables que configuran la textura del forward rendering
unsigned int uVertexTexPP;
unsigned int vertexBuffTexId;
unsigned int uDepthTexPP;

//Atributos
int inPos;
int inColor;
int inNormal;
int inTexCoord;

// Variables uniformes que controlan los parámetros de Motion Blur

float alpha = 0.6;

// Variables uniformes del DOF (Depth of Field)

float distanciaFocal = -25.0;
float distanciaMax = 0.2;

// Uso del buffer de profundidad

float pnear = 1;
float pfar = 50;

// Índices de las variables uniformes DOF

int uDistanciaFocal;
int uDistanciaMax;
int unear;
int ufar;

// Máscara de convolución

int umaskfactor;
int umask;

float maskFactor = float(1.0 / 200);
float* mask = new float[MASK_SIZE] {
		1.0f * maskFactor, 2.0f * maskFactor, 3.0f * maskFactor, 2.0f * maskFactor, 1.0f * maskFactor,
		2.0f * maskFactor, 3.0f * maskFactor, 4.0f * maskFactor, 3.0f * maskFactor, 2.0f * maskFactor,
		3.0f * maskFactor, 4.0f * maskFactor, 5.0f * maskFactor, 4.0f * maskFactor, 3.0f * maskFactor,
		2.0f * maskFactor, 3.0f * maskFactor, 4.0f * maskFactor, 3.0f * maskFactor, 2.0f * maskFactor,
		1.0f * maskFactor, 2.0f * maskFactor, 3.0f * maskFactor, 2.0f * maskFactor, 1.0f * maskFactor
};


//////////////////////////////////////////////////////////////
// Funciones auxiliares
//////////////////////////////////////////////////////////////

//Declaración de CB
void renderFunc();
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);

void renderCube();

//Funciones de inicialización y destrucción
void initContext(int argc, char** argv);
void initOGL();
void initShaderFw(const char *vname, const char *fname);
void initObj();
void destroy();


//Carga el shader indicado, devuele el ID del shader
//!Por implementar
GLuint loadShader(const char *fileName, GLenum type);

//Crea una textura, la configura, la sube a OpenGL, 
//y devuelve el identificador de la textura 
//!!Por implementar
unsigned int loadTex(const char *fileName);

//////////////////////////////////////////////////////////////
// Nuevas variables auxiliares
//////////////////////////////////////////////////////////////

unsigned int postProccesVShader;
unsigned int postProccesFShader;
unsigned int postProccesProgram;

//Uniform
unsigned int uColorTexPP;

//Atributos
int inPosPP;

// Variables que nos darán acceso al objeto cuadrado
unsigned int planeVAO;
unsigned int planeVertexVBO;

// Variables que darán acceso al objeto cuadrado
unsigned int fbo;
unsigned int colorBuffTexId;
unsigned int depthBuffTexId;


//////////////////////////////////////////////////////////////
// Nuevas funciones auxiliares
//////////////////////////////////////////////////////////////

void initShaderPP(const char* vname, const char* fname);
void initPlane();
void initFBO();
void resizeFBO(unsigned int w, unsigned int h);


int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)

	initContext(argc, argv);
	initOGL();
	initShaderFw("../shaders_P4/fwRendering.v1.vert", "../shaders_P4/fwRendering.v1.frag");
	initObj();

	// Se llama a la función creada previamente, en la cual se cargan los nuevos shaders de post proceso

	initShaderPP("../shaders_P4/postProcessing.v4.vert",
		"../shaders_P4/postProcessing.v4.frag");

	// Se inicializa la geometría

	initPlane();

	// Se inicializa el FBO y se invoca a la función resize de manera dinámica con las dimensiones de pantalla, para ajustar el tamaño de las texturas dependiendo de esta

	initFBO();
	resizeFBO(SCREEN_SIZE);
	
	glutMainLoop();

	destroy();

	return 0;
}

//////////////////////////////////////////
// Funciones auxiliares 
void initContext(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	//glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_SIZE);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Prácticas GLSL");

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}

	const GLubyte *oglVersion = glGetString(GL_VERSION);
	std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;

	glutReshapeFunc(resizeFunc);
	glutDisplayFunc(renderFunc);
	glutIdleFunc(idleFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
}

void initOGL()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	proj = glm::perspective(glm::radians(60.0f), 1.0f, 1.0f, 50.0f);
	view = glm::mat4(1.0f);
	view[3].z = -25.0f;
}


void destroy()
{
	glDetachShader(program, vshader);
	glDetachShader(program, fshader);
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	glDeleteProgram(program);

	if (inPos != -1) glDeleteBuffers(1, &posVBO);
	if (inColor != -1) glDeleteBuffers(1, &colorVBO);
	if (inNormal != -1) glDeleteBuffers(1, &normalVBO);
	if (inTexCoord != -1) glDeleteBuffers(1, &texCoordVBO);
	glDeleteBuffers(1, &triangleIndexVBO);

	glDeleteVertexArrays(1, &vao);

	glDeleteTextures(1, &colorTexId);
	glDeleteTextures(1, &emiTexId);

	// Se añaden las destrucciones de los nuevos shaders creados

	glDetachShader(postProccesProgram, postProccesVShader);
	glDetachShader(postProccesProgram, postProccesFShader);
	glDeleteShader(postProccesVShader);
	glDeleteShader(postProccesFShader);
	glDeleteProgram(postProccesProgram);

	glDeleteBuffers(1, &planeVertexVBO);
	glDeleteVertexArrays(1, &planeVAO);

	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &colorBuffTexId);
	glDeleteTextures(1, &depthBuffTexId);
	glDeleteTextures(1, &vertexBuffTexId);


}

void initShaderFw(const char *vname, const char *fname)
{
	vshader = loadShader(vname, GL_VERTEX_SHADER);
	fshader = loadShader(fname, GL_FRAGMENT_SHADER);

	program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);

	glBindAttribLocation(program, 0, "inPos");
	glBindAttribLocation(program, 1, "inColor");
	glBindAttribLocation(program, 2, "inNormal");
	glBindAttribLocation(program, 3, "inTexCoord");


	glLinkProgram(program);

	int linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);

		char *logString = new char[logLen];
		glGetProgramInfoLog(program, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete[] logString;

		glDeleteProgram(program);
		program = 0;
		exit(-1);
	}

	uNormalMat = glGetUniformLocation(program, "normal");
	uModelViewMat = glGetUniformLocation(program, "modelView");
	uModelViewProjMat = glGetUniformLocation(program, "modelViewProj");

	uColorTex = glGetUniformLocation(program, "colorTex");
	uEmiTex = glGetUniformLocation(program, "emiTex");

	inPos = glGetAttribLocation(program, "inPos");
	inColor = glGetAttribLocation(program, "inColor");
	inNormal = glGetAttribLocation(program, "inNormal");
	inTexCoord = glGetAttribLocation(program, "inTexCoord");
}

void initObj()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	if (inPos != -1)
	{
		glGenBuffers(1, &posVBO);
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex*sizeof(float) * 3,
			cubeVertexPos, GL_STATIC_DRAW);
		glVertexAttribPointer(inPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inPos);
	}

	if (inColor != -1)
	{
		glGenBuffers(1, &colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex*sizeof(float) * 3,
			cubeVertexColor, GL_STATIC_DRAW);
		glVertexAttribPointer(inColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inColor);
	}

	if (inNormal != -1)
	{
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex*sizeof(float) * 3,
			cubeVertexNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(inNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inNormal);
	}


	if (inTexCoord != -1)
	{
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex*sizeof(float) * 2,
			cubeVertexTexCoord, GL_STATIC_DRAW);
		glVertexAttribPointer(inTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inTexCoord);
	}

	glGenBuffers(1, &triangleIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		cubeNTriangleIndex*sizeof(unsigned int) * 3, cubeTriangleIndex,
		GL_STATIC_DRAW);

	model = glm::mat4(1.0f);

	colorTexId = loadTex("../img/color2.png");
	emiTexId = loadTex("../img/emissive.png");
}

GLuint loadShader(const char *fileName, GLenum type)
{
	unsigned int fileLen;
	char *source = loadStringFromFile(fileName, fileLen);

	//////////////////////////////////////////////
	//Creación y compilación del Shader
	GLuint shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1,
		(const GLchar **)&source, (const GLint *)&fileLen);
	glCompileShader(shader);
	delete[] source;

	//Comprobamos que se compilo bien
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

		char *logString = new char[logLen];
		glGetShaderInfoLog(shader, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete[] logString;

		glDeleteShader(shader);
		exit(-1);
	}

	return shader;
}

unsigned int loadTex(const char *fileName)
{
	unsigned char *map;
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
	delete[] map;
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);

	return texId;
}

void renderFunc()
{

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// El renderizado del cubo queda dentro del fbo
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/**/
	glUseProgram(program);

	//Texturas
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


	model = glm::mat4(2.0f);
	model[3].w = 1.0f;
	model = glm::rotate(model, angle, glm::vec3(1.0f, 1.0f, 0.0f));
	renderCube();

	std::srand(RAND_SEED);
	for (unsigned int i = 0; i < 10; i++)
	{
		float size = float(std::rand() % 3 + 1);

		glm::vec3 axis(glm::vec3(float(std::rand() % 2),
			float(std::rand() % 2), float(std::rand() % 2)));
		if (glm::all(glm::equal(axis, glm::vec3(0.0f))))
			axis = glm::vec3(1.0f);

		float trans = float(std::rand() % 7 + 3) * 1.00f + 0.5f;
		glm::vec3 transVec = axis * trans;
		transVec.x *= (std::rand() % 2) ? 1.0f : -1.0f;
		transVec.y *= (std::rand() % 2) ? 1.0f : -1.0f;
		transVec.z *= (std::rand() % 2) ? 1.0f : -1.0f;

		model = glm::rotate(glm::mat4(1.0f), angle*2.0f*size, axis);
		model = glm::translate(model, transVec);
		model = glm::rotate(model, angle*2.0f*size, axis);
		model = glm::scale(model, glm::vec3(1.0f / (size*0.7f)));
		renderCube();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Renderizado del plano

	glUseProgram(postProccesProgram);

	// Variables uniformes 
	if (uDistanciaMax != -1) glUniform1fv(uDistanciaMax, 1, &distanciaMax);
	if (uDistanciaFocal != -1) glUniform1fv(uDistanciaFocal, 1, &distanciaFocal);
	if (unear != -1) glUniform1fv(unear, 1, &pnear);
	if (ufar != -1) glUniform1fv(ufar, 1, &pfar);
	if (umaskfactor != -1) glUniform1fv(umaskfactor, 1, &maskFactor);
	if (umask != -1) glUniform1fv(umask, 25, mask);

	// CONTROL DEL MOTION BLUR 

	glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA);
	glBlendColor(0.5f, 0.5f, 0.5f, alpha);
	glBlendEquation(GL_FUNC_ADD);


	// Se sube la textura al shader

	if (uColorTexPP != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffTexId);
		glUniform1i(uColorTexPP, 0);
	}

	if (uVertexTexPP != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, vertexBuffTexId);
		glUniform1i(uVertexTexPP, 1);
	}

	if (uDepthTexPP != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, depthBuffTexId);
		glUniform1i(uDepthTexPP, 2);
	}

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glutSwapBuffers();
}

void renderCube()
{
	glm::mat4 modelView = view * model;
	glm::mat4 modelViewProj = proj * view * model;
	glm::mat4 normal = glm::transpose(glm::inverse(modelView));

	if (uModelViewMat != -1)
		glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE,
		&(modelView[0][0]));
	if (uModelViewProjMat != -1)
		glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE,
		&(modelViewProj[0][0]));
	if (uNormalMat != -1)
		glUniformMatrix4fv(uNormalMat, 1, GL_FALSE,
		&(normal[0][0]));
	
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3,
		GL_UNSIGNED_INT, (void*)0);
}



void resizeFunc(int width, int height)
{
	glViewport(0, 0, width, height);
	proj = glm::perspective(glm::radians(60.0f), float(width) /float(height), 1.0f, 50.0f);

	// Se modifica el tamaño de las texturas dependiendo de esta
	resizeFBO(width, height);

	glutPostRedisplay();
}

void idleFunc()
{
	angle = (angle > 3.141592f * 2.0f) ? 0 : angle + 0.02f;
	
	glutPostRedisplay();
}

void keyboardFunc(unsigned char key, int x, int y){

	switch (key) {
	// Control del alfa, para el Motion Blur
	case 'w':
		alpha = alpha + 0.1;
		break;

	case 's':
		alpha = alpha - 0.1;
		break;

	// Control del Depth of Field
	case 'a':
		distanciaMax *= 0.5;

		break;
	
	case 'z':
		distanciaMax *= 2;

		break;

	case 'd':
		distanciaFocal--;

		break;

	case 'c':
		distanciaFocal++;

		break;

	case 'j':
		maskFactor = float(0.03);				
		mask = new float[MASK_SIZE] {
			1.0f * maskFactor, 2.0f * maskFactor, 3.0f * maskFactor, 2.0f * maskFactor, 1.0f * maskFactor,
				2.0f * maskFactor, 3.0f * maskFactor, 4.0f * maskFactor, 3.0f * maskFactor, 2.0f * maskFactor,
				3.0f * maskFactor, 4.0f * maskFactor, 5.0f * maskFactor, 4.0f * maskFactor, 3.0f * maskFactor,
				2.0f * maskFactor, 3.0f * maskFactor, 4.0f * maskFactor, 3.0f * maskFactor, 2.0f * maskFactor,
				1.0f * maskFactor, 2.0f * maskFactor, 3.0f * maskFactor, 2.0f * maskFactor, 1.0f * maskFactor
		};
		break;

	case 'k':
		maskFactor = float(0.1);
		mask = new float[MASK_SIZE] {
			0.0f * maskFactor, 1.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor,
				1.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor,
				0.0f * maskFactor, -1.0f * maskFactor, 1.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor,
				0.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor, 1.0f * maskFactor,
				0.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor, 1.0f * maskFactor, 0.0f * maskFactor };
		break;

	case 'l':
		maskFactor = float(0.05);
		mask = new float[MASK_SIZE] {
			0.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor,
				0.0f * maskFactor, -2.0f * maskFactor, -1.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor,
				0.0f * maskFactor, -1.0f * maskFactor, 1.0f * maskFactor, +1.0f * maskFactor, 0.0f * maskFactor,
				0.0f * maskFactor, 0.0f * maskFactor, +1.0f * maskFactor, 2.0f * maskFactor, 0.0f * maskFactor,
				0.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor, 0.0f * maskFactor };
		break;
	}








}
void mouseFunc(int button, int state, int x, int y){}

// Función que inicializa y enlaza los shaders de post proceso

void initShaderPP(const char* vname, const char* fname) {

	postProccesVShader = loadShader(vname, GL_VERTEX_SHADER);
	postProccesFShader = loadShader(fname, GL_FRAGMENT_SHADER);
	postProccesProgram = glCreateProgram();
	glAttachShader(postProccesProgram, postProccesVShader);
	glAttachShader(postProccesProgram, postProccesFShader);
	glBindAttribLocation(postProccesProgram, 0, "inPos");
	glLinkProgram(postProccesProgram);
	int linked;
	glGetProgramiv(postProccesProgram, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(postProccesProgram, GL_INFO_LOG_LENGTH, &logLen);
		char* logString = new char[logLen];
		glGetProgramInfoLog(postProccesProgram, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete logString;
		glDeleteProgram(postProccesProgram);
		postProccesProgram = 0;
		exit(-1);
	}

	uColorTexPP = glGetUniformLocation(postProccesProgram, "colorTex");
	uVertexTexPP = glGetUniformLocation(postProccesProgram, "vertexTex");
	inPosPP = glGetAttribLocation(postProccesProgram, "inPos");

	uDistanciaFocal = glGetUniformLocation(postProccesProgram, "focDist");
	uDistanciaMax = glGetUniformLocation(postProccesProgram, "maxDist");

	unear = glGetUniformLocation(postProccesProgram, "near");
	ufar = glGetUniformLocation(postProccesProgram, "far");
	uDepthTexPP = glGetUniformLocation(postProccesProgram, "depthTex");

	umaskfactor = glGetUniformLocation(postProccesProgram, "maskFactor");
	umask = glGetUniformLocation(postProccesProgram, "mask");

}

// Función que inicializa el plano

void initPlane() {

	//Generar VAO y activarlo
	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);
	//Generamos VBO de posiciones
	glGenBuffers(1, &planeVertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVertexVBO);
	glBufferData(GL_ARRAY_BUFFER, planeNVertex * sizeof(float) * 3,
		planeVertexPos, GL_STATIC_DRAW);
	glVertexAttribPointer(inPosPP, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(inPosPP);
}

// Inicializa el FBO

void initFBO() {
	glGenFramebuffers(1, &fbo);
	glGenTextures(1, &colorBuffTexId);
	glGenTextures(1, &depthBuffTexId);
	glGenTextures(1, &vertexBuffTexId);

}

// Función que redefine las imágenes de textura cada vez que el usuario toca el viewport, en función del ancho y el alto // PREGUNTA DE EXAMEN

void resizeFBO(unsigned int w, unsigned int h) {
	glBindTexture(GL_TEXTURE_2D, colorBuffTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0,
		GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, vertexBuffTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, depthBuffTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0,
		GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, colorBuffTexId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		depthBuffTexId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, vertexBuffTexId, 0);
	const GLenum buffs[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffs);
	//const GLenum buffs[1] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, buffs);
	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		std::cerr << "Error configurando el FBO" << std::endl;
		exit(-1);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

