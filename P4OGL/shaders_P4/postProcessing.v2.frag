#version 330 core

//#define MASK9
#define MASK25A

//Color de salida
out vec4 outColor;


//Variables Variantes
in vec2 texCoord;

// Máscara de convolución

#ifdef MASK9
#define MASK_SIZE 9u
const float maskFactor = float (1.0/14.0);
const vec2 texIdx[MASK_SIZE] = vec2[](
vec2(-1.0,1.0), vec2(0.0,1.0), vec2(1.0,1.0),
vec2(-1.0,0.0), vec2(0.0,0.0), vec2(1.0,0.0),
vec2(-1.0,-1.0), vec2(0.0,-1.0), vec2(1.0,-1.0));
const float mask[MASK_SIZE] = float[](
float (1.0*maskFactor), float (2.0*maskFactor), float (1.0*maskFactor),
float (2.0*maskFactor), float (2.0*maskFactor), float (2.0*maskFactor),
float (1.0*maskFactor), float (2.0*maskFactor), float (1.0*maskFactor));
#endif

#ifdef MASK25A
#define MASK_SIZE 25u
const vec2 texIdx[MASK_SIZE] = vec2[](
vec2(-2.0,2.0), vec2(-1.0,2.0), vec2(0.0,2.0), vec2(1.0,2.0), vec2(2.0,2.0),
vec2(-2.0,1.0), vec2(-1.0,1.0), vec2(0.0,1.0), vec2(1.0,1.0), vec2(2.0,1.0),
vec2(-2.0,0.0), vec2(-1.0,0.0), vec2(0.0,0.0), vec2(1.0,0.0), vec2(2.0,0.0),
vec2(-2.0,-1.0), vec2(-1.0,-1.0), vec2(0.0,-1.0), vec2(1.0,-1.0), vec2(2.0,-1.0),
vec2(-2.0,-2.0), vec2(-1.0,-2.0), vec2(0.0,-2.0), vec2(1.0,-2.0), vec2(2.0,-2.0));
const float maskFactor = float (1.0/200);
const float mask[MASK_SIZE] = float[](
1.0*maskFactor, 2.0*maskFactor, 3.0*maskFactor,2.0*maskFactor, 1.0*maskFactor,
2.0*maskFactor, 3.0*maskFactor, 4.0*maskFactor,3.0*maskFactor, 2.0*maskFactor,
3.0*maskFactor, 4.0*maskFactor, 5.0*maskFactor,4.0*maskFactor, 3.0*maskFactor,
2.0*maskFactor, 3.0*maskFactor, 4.0*maskFactor,3.0*maskFactor, 2.0*maskFactor,
1.0*maskFactor, 2.0*maskFactor, 3.0*maskFactor,2.0*maskFactor, 1.0*maskFactor);
#endif


//Textura
uniform sampler2D colorTex;

//Textura con la posición del fragmento en coordenadas de la cámara
uniform sampler2D vertexTex;

//Distancia de enfoque y desenfoque máximo

uniform float focDist;
uniform float maxDist;



void main()

{
	//Sería más rápido utilizar una variable uniform el tamaño de la textura.

	vec2 ts = vec2(1.0) / vec2 (textureSize (colorTex,0)); // Se calcula cuánto se tiene que avanzar en la textura para llegar al siguiente píxel.

	//Profundidad de campo

	float dof = abs(texture(vertexTex,texCoord).z -focDist)
		* maxDist;
	dof = clamp (dof, 0.0, 1.0);
	dof *= dof;

	vec4 color = vec4 (0.0);

	// Se crea un bucle para recorrer todos los elementos de la máscara 

	for (uint i = 0u; i < MASK_SIZE; i++)
	{
		vec2 iidx = texCoord + ts * texIdx[i]*dof; //Se suma la coordenada de textura, más el tamaño de cada paso a avanzar (ts) 
		color += texture(colorTex, iidx,0.0) * mask[i]; //Se acumula el valor ponderado, en función del valor indicado en la máscara
	}

	outColor = color;
}
