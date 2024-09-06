#version 330 core

in vec3 inPos;	
in vec3 inColor;
in vec3 inNormal; //Recibe la normal 

//Se pasan las matrices para ahorrar c�lculos
uniform mat4 normal;
uniform mat4 modelView;
uniform mat4 modelViewProj;


//Propiedades Luz

vec3 Ia = vec3(0.4); //Intensidad de la luz ambiental
vec3 Id = vec3(1.0); //Intensidad de la luz difusa
vec3 Is = vec3(1.0); //Intensidad de la luz especular
vec3 lpos = vec3(2.0); //Posici�n de la luz 

//Propiedades Material-Objeto

vec3 Ka; //Color ambiental del material
vec3 Kd; //Componente difusa del objeto o material
vec3 Ks = vec3(1.0); //Componente especular del material, que no tiene que ser el mismo que el ambiental, por lo que se puede inicializar aqu� directamente de manera independiente
vec3 N; //Normal en espacio de vista
vec3 pos; //Posici�n
float alpha = 3.0;

out vec3 color;

//Funci�n auxiliar para calcular el sombreado
vec3 shade();

void main()
{
	Ka = inColor;
	Kd = inColor;
	N = normalize((normal*vec4(inNormal,0.0)).xyz); //Reescalar las normales a m�dulo 1 a espacio de vista
	//Se crea una variable local posici�n en coordenadas del espacio de vista (multiplicando por modelView)
	pos = (modelView*vec4(inPos,1.0)).xyz;
	color = shade();
	gl_Position =  modelViewProj * vec4 (inPos,1.0);

}

vec3 shade() {

	//PARTE AMBIENTAL//
	vec3 c = vec3(0.0); //Color negro por defecto
	c += Ia*Ka; //El color va a depender del color ambiental del objeto, junto con la intensidad de la luz ambiental

	//PARTE DIFUSA//
	vec3 L = normalize(lpos-pos); //Se normaliza la direcci�n de la luz, que se calcula restando la posici�n de la luz (lpos) a la posici�n del objeto(pos) (respecto a coordenadas de la c�mara)
	vec3 diffuse = Id*Kd*dot(L,N); //Para calcular la componente difusa, se multiplica la intensidad de la luz difusa, por la componente difusa del material del objeto, por el producto escalar de la normal del v�rtice y la direcci�n de la luz
	c+= clamp(diffuse,0.0,1.0); //Se acota la componente difusa entre 0 y 1

	//PARTE ESPECULAR// //Es la componente de la luz que refleja el objeto
	vec3 V = normalize (-pos); //El vector V es la direcci�n de la luz reflectada dependiendo del punto de vista, por lo que se toma como referencia el vector pos, que tiene direcci�n opuesta al objeto y por lo tanto se pasa negativa.
	vec3 R = normalize (reflect(-L,N)); //La funci�n reflect, dado un rayo incidente y una normal, calcula el vector reflejado R perfecto, independientemente del punto de vista. El vector L sale del objeto hacia fuera, por lo que en la f�rmula hay que pasarlo negativo
	
	//Por lo tanto, para la componente especular, hay que establecer una relaci�n entre los vectores V y R. Cuanto m�s cerca est�n, mayor ser� el reflejo observado

	float factor = clamp(dot(R,V),0.0,1.0);
	vec3 specular = Is*Ks*pow(factor, alpha); //Para calcular, se utiliza la f�rmula de Phong, multiplicando el factor obtenido de la relaci�n entre los vectores R y V, por la intensidad especular y la componente especular del material. El factor alfa es propio del material y se define arriba.
	c+= clamp(specular,0.0,1.0);


	return c;

}
