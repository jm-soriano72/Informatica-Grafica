#version 330 core

out vec4 outColor;

in vec2 texCoord; //Atributo de UVs en vértices de objeto
in vec3 pos; //Posición en coordenadas de vista, calculada en los vértices
in vec3 norm; //Normal en espacio de cámara, también calculada en los vértices

uniform mat4 modelView; //Matriz de vista, para modificar la posición de la luz en coordenadas de la cámara
uniform sampler2D colorTex; //Color de la textura
uniform sampler2D emiTex;
uniform sampler2D specularTex;

//Propiedades Luz

vec3 Ia = vec3(0.4); //Intensidad de la luz ambiental
vec3 Id = vec3(1.0); //Intensidad de la luz difusa
vec3 Is = vec3(1.0); //Intensidad de la luz especular
vec3 lpos = vec3(2.0); //Posición de la luz en coordenadas del mundo, que habrá que convertir a coordenadas de la cámara

//Propiedades Material-Objeto

vec3 Ka; //Color ambiental del material
vec3 Kd; //Componente difusa del objeto o material
vec3 Ks; //Componente especular del material, que no tiene que ser el mismo que el ambiental, por lo que se puede inicializar aquí directamente de manera independiente
vec3 Ke; //Componente emisiva
vec3 N; //Normal en espacio de vista
float alpha = 3.0; //Propio del material

vec3 shade();

void main()
{
	lpos = (modelView*vec4(lpos,1.0)).xyz; //Se transforman las cordenadas de la posición de la luz a coordenadas de la cámara
	Ka = texture(colorTex, texCoord).rgb; //Componente ambiental obtenida a partir de la textura
	Kd = texture(colorTex, texCoord).rgb; //Componente difusa obtenida a partir de la textura
	Ke = texture(emiTex, texCoord).rgb; //Componente emisiva obtenida a partir de la textura
	Ks = texture(specularTex, texCoord).rgb; //Componente especular obtenida a partir de la textura
	N = normalize(norm);
	outColor = vec4(shade(), 1.0);   
}

//Para aplicar la iluminación de Phong, la función de shade que la aplica se debe calcular en los fragmentos, no en los vértices. En los vértices solo se recalcula la normal y la posición en coordenadas de la cámara
vec3 shade() {

	//PARTE AMBIENTAL//
	vec3 c = vec3(0.0); //Color negro por defecto
	c += Ia*Ka; //El color va a depender del color ambiental del objeto, junto con la intensidad de la luz ambiental

	//PARTE DIFUSA//
	vec3 L = normalize(lpos-pos); //Se normaliza la dirección de la luz, que se calcula restando la posición de la luz (lpos) a la posición del objeto(pos) (respecto a coordenadas de la cámara)
	vec3 diffuse = Id*Kd*dot(L,N); //Para calcular la componente difusa, se multiplica la intensidad de la luz difusa, por la componente difusa del material del objeto, por el producto escalar de la normal del vértice y la dirección de la luz
	c+= clamp(diffuse,0.0,1.0); //Se acota la componente difusa entre 0 y 1

	//PARTE ESPECULAR// //Es la componente de la luz que refleja el objeto
	vec3 V = normalize (-pos); //El vector V es la dirección de la luz reflectada dependiendo del punto de vista, por lo que se toma como referencia el vector pos, que tiene dirección opuesta al objeto y por lo tanto se pasa negativa.
	vec3 R = normalize (reflect(-L,N)); //La función reflect, dado un rayo incidente y una normal, calcula el vector reflejado R perfecto, independientemente del punto de vista. El vector L sale del objeto hacia fuera, por lo que en la fórmula hay que pasarlo negativo
	
	//Por lo tanto, para la componente especular, hay que establecer una relación entre los vectores V y R. Cuanto más cerca estén, mayor será el reflejo observado

	float factor = clamp(dot(R,V),0.0,1.0);
	vec3 specular = Is*Ks*pow(factor, alpha); //Para calcular, se utiliza la fórmula de Phong, multiplicando el factor obtenido de la relación entre los vectores R y V, por la intensidad especular y la componente especular del material. El factor alfa es propio del material y se define arriba.
	c+= clamp(specular,0.0,1.0);

	c+= Ke; //Se le añade la componente emisiva

	return c;

}