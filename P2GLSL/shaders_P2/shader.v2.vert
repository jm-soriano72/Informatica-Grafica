#version 330 core

in vec3 inPos;	
in vec3 inColor;
in vec3 inNormal; //Recibe la normal 

//Se pasan las matrices para ahorrar c�lculos
uniform mat4 normal;
uniform mat4 modelView;
uniform mat4 modelViewProj;


out vec3 color;
out vec3 pos; //Tambi�n se pasa la posici�n en espacio de la c�mara a los fragments
out vec3 norm; //La normal se va a seguir calculando en cada v�rtice con Phong, por lo que se va a pasar a los fragments


void main()
{
	color = inColor;
	norm = (normal*vec4(inNormal,0.0)).xyz; //Transformar las normales a espacio de vista
	//Se crea una variable local posici�n en coordenadas del espacio de vista (multiplicando por modelView)
	pos = (modelView*vec4(inPos,1.0)).xyz;
	

	gl_Position =  modelViewProj * vec4 (inPos,1.0);

}


