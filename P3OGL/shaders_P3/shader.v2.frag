#version 330 core

out vec4 outColor;

in vec2 texCoord; //Atributo de UVs en vértices de objeto
in vec3 pos; //Posición en coordenadas de vista, calculada en los vértices
in vec3 norm; //Normal en espacio de cámara, también calculada en los vértices

uniform mat4 modelView; //Matriz de vista, para modificar la posición de la luz en coordenadas de la cámara
uniform mat4 view; //Matriz para averiguar la posición de la cámara
uniform sampler2D colorTex; //Color de la textura
uniform sampler2D emiTex;
uniform sampler2D specTex;

uniform vec3 lightPosition;
uniform vec3 lightIntensity;

vec3 Ia = vec3(0.4); //Intensidad de la luz ambiental, que no depende de las distintas luces

//Propiedades Luz 1 -> Luz puntual que se controla 

vec3 Id = lightIntensity; //Intensidad de la luz difusa a una distancia d0
vec3 Is = lightIntensity; //Intensidad de la luz especular a una distancia d0
vec3 lpos = (view*vec4(lightPosition,1)*inverse(view)).xyz; //Posición de la luz en coordenadas del mundo, que habrá que convertir a coordenadas de la cámara


//Propiedades Luz 3 -> Luz direccional

vec3 Id3 = lightIntensity; //Intensidad difusa de la luz direccional
vec3 Is3 = lightIntensity; //Intensidad especular de la luz direccional
vec3 dirl3 = vec3(0.2,-1.0,0.0); //Dirección de la luz direccional


//Propiedades Luz 4 -> Luz focal

vec3 Id4 = lightIntensity; //Intensidad difusa de la luz focal
vec3 Is4 = lightIntensity; //Intensidad especular de la luz focal
vec3 lpos4 = (view*vec4(lightPosition,1)*inverse(view)).xyz; //Posición del foco
float apertura = cos(radians(20.0)); // Se calcula directamente el coseno del ángulo de apertura del foco
float expFocal = 2.0; //Parámetro del foco
vec3 dirl4 = vec3(0.0,-0.4,1.0); //Dirección del foco


// Parámetros de atenuación

float d0 = 1.0; //Distancia a la que se conoce la intensidad de la luz
float d_max = 7.0; //Distancia máxima a la que llega la luz


// Propiedades Material-Objeto

vec3 Ka; //Color ambiental del material
vec3 Kd; //Componente difusa del objeto o material
vec3 Ks; //Componente especular del material, que no tiene que ser el mismo que el ambiental, por lo que se puede inicializar aquí directamente de manera independiente
vec3 Ke; //Componente emisiva
vec3 N; //Normal en espacio de vista

float alpha = 3.0; //Propio del material

// Parámetros niebla

float df = 0.4; // Mide la densidad de la niebla (profundidad)
vec3 cFog = vec3(0.0); // Color de la niebla 
float posCamara_z = view[3].z; // Posición de la cámara en el eje Z
float df2 = 0.3; //Mide la densidad de la niebla (altura)



// Funciones auxiliares

float fDist (float d0, float d, float d_max);
vec3 shadePuntual();
vec3 shadeDireccional();
vec3 shadeFocal();
vec3 calcNiebla(vec3 ci);


void main()
{

	Ka = texture(colorTex, texCoord).rgb; //Componente ambiental obtenida a partir de la textura
	Kd = texture(colorTex, texCoord).rgb; //Componente difusa obtenida a partir de la textura
	Ke = texture(emiTex, texCoord).rgb; //Componente emisiva obtenida a partir de la textura
	Ks = texture(specTex, texCoord).rgb; //Componente especular obtenida a partir de la textura

	N = normalize(norm);

	// Para calcular el color resultante, por definición, se suman las componentes especulares y difusas de cada fuente lumínica. El componente ambiental y emisivo se suman una única vez
	outColor = vec4(shadePuntual(), 1.0);
	outColor += vec4(shadeDireccional(), 0.0);
	outColor+= vec4(shadeFocal(),0.0);

	outColor = vec4(calcNiebla((outColor).xyz),1.0);

}

// Para aplicar la iluminación de Phong, la función de shade que la aplica se debe calcular en los fragmentos, no en los vértices. En los vértices solo se recalcula la normal y la posición en coordenadas de la cámara
// Se añaden parámetros a la función shade para poder utilizar la misma función para diferentes luces

vec3 shadePuntual () {

	vec3 c = vec3(0.0); //Color negro por defecto

    // FUNCIÓN DE ATENUACIÓN 1 //
	float d1 = distance(lpos, pos); //Distancia entre el foco de luz y la posición del objeto 
	float atenuacion1 = fDist(d0, d1, d_max); //Se calcula el factor de atenuación con la distancia a través de la función auxiliar "fDist"

	// PARTE DIFUSA LUZ 1 //
	vec3 L1 = normalize(lpos-pos); //Se normaliza la dirección de la luz, que se calcula restando la posición de la luz (lpos) a la posición del objeto(pos) (respecto a coordenadas de la cámara)
	vec3 diffuse1 = Id*Kd*dot(L1,N); //Para calcular la componente difusa, se multiplica la intensidad de la luz difusa, por la componente difusa del material del objeto, por el producto escalar de la normal del vértice y la dirección de la luz
	c+=diffuse1*atenuacion1;

	// PARTE ESPECULAR LUZ 1 // //Es la componente de la luz que refleja el objeto
	vec3 V1 = normalize (-pos); //El vector V es la dirección de la luz reflectada dependiendo del punto de vista, por lo que se toma como referencia el vector pos, que tiene dirección opuesta al objeto y por lo tanto se pasa negativa.
	vec3 R1 = normalize (reflect(-L1,N)); //La función reflect, dado un rayo incidente y una normal, calcula el vector reflejado R perfecto, independientemente del punto de vista. El vector L sale del objeto hacia fuera, por lo que en la fórmula hay que pasarlo negativo
	//Por lo tanto, para la componente especular, hay que establecer una relación entre los vectores V y R. Cuanto más cerca estén, mayor será el reflejo observado
	float factor1 = clamp(dot(R1,V1),0.0,1.0);
	vec3 specular1 = Is*Ks*pow(factor1, alpha); //Para calcular, se utiliza la fórmula de Phong, multiplicando el factor obtenido de la relación entre los vectores R y V, por la intensidad especular y la componente especular del material. El factor alfa es propio del material y se define arriba.
	c+= specular1*atenuacion1;


	//PARTE AMBIENTAL// //Se calcula después, porque la atenuación no le afecta
	c += Ia*Ka; //El color va a depender del color ambiental del objeto, junto con la intensidad de la luz ambiental


	c+= Ke; //Se le añade la componente emisiva

	c = clamp(c, 0.0, 1.0);

	return c;

}

vec3 shadeDireccional () {

	vec3 c = vec3(0.0); //Color negro por defecto

	// PARTE DIFUSA LUZ 3 //
	vec3 L3 = normalize(dirl3); //Se normaliza la dirección de la luz, que al ser direccional, viene dada como parámetro
	vec3 diffuse3 = Id3*Kd*dot(L3,N); 
	c+=diffuse3;

	// PARTE ESPECULAR LUZ 3 // 
	vec3 V3 = normalize (-pos); 
	vec3 R3 = normalize (reflect(-L3,N)); 
	float factor3 = clamp(dot(R3,V3),0.0,1.0);
	vec3 specular3 = Is3*Ks*pow(factor3, alpha); 
	c+= specular3;

	// Por definición, la luz direccional no tiene factor de atenuación

	c = clamp(c,0.0,1.0);

	return c;

}

vec3 shadeFocal () {

	vec3 c = vec3(0.0);

	vec3 D4 = normalize(dirl4); //Se normaliza el vector de la dirección de la luz focal

	// PARTE DIFUSA LUZ 4 //
	vec3 L4 = normalize(lpos4-pos);
	vec3 diffuse4 = Id4*Kd*dot(L4,N);

	// PARTE ESPECULAAR LUZ 4 //
	vec3 V4 = normalize (-pos);
	vec3 R4 = normalize (reflect(-L4,N));
	float factor4 = clamp(dot(R4,V4),0.0,1.0);
	vec3 specular4 = Is4*Ks*pow(factor4,alpha);

	// Se calcula el producto escalar entre la dirección de la luz y la dirección del propio foco (-L·D). Si este es mayor que el coseno del ángulo de apertura, el punto se encuentra dentro del foco y se le puede aplicar la iluminación

	if (dot(-L4,D4) > apertura) {
		c+=diffuse4;
		c+=specular4;
		// Se le aplica una atenuación diferente a las de las luces puntuales
		c*=pow((dot(-L4,D4)-apertura)/(1-apertura),expFocal);
		c = clamp(c,0.0,1.0);
	}

	return c;

}

float fDist (float d0, float d, float d_max) {
	
	float fdist;
	float fwin;

	fwin = pow(max(0,(1-pow((d/d_max),4))),2);
	fdist = pow((d0/d),2)*fwin;

	return fdist;

}

// Función auxiliar que calcule el color resultante aplicándole niebla
vec3 calcNiebla (vec3 ci) {

	// Niebla que se hace más densa con la profundidad
	float zs = distance(vec3(0.0,0.0,pos.z),vec3(0.0,0.0,posCamara_z)); //Calcula la distancia en el eje Z entre la cámara y el objeto
	float f_niebla = pow(2.7182,(-df*zs)); //Se calcula el factor niebla
	
	//Niebla que aumenta con la altura
	float ys = distance(vec3(0.0,pos.y,0.0),vec3(0.0,-2.0,0.0)); //Calcula la distancia en el eje Y entre el objeto y un punto establecido como referencia
	float f_niebla2 = pow(2.7182,(-df2*ys));

	//Se calcula el factor final multiplicando el factor de la altura por el de la profundidad

	float f = f_niebla*f_niebla2;
	f = clamp(f,0.0,1.0);

	vec3 c = ((f*ci) + ((1-f)*cFog));

	c = clamp(c,0.0,1.0);

	return c;

}