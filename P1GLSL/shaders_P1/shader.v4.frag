#version 330 core

out vec4 outColor;


void main()
{
	//La funci�n m�dulo calcula el resto y pintar� cada primitiva (tri�ngulo) de un color diferente

	if(mod (gl_PrimitiveID,4.0) == 0.0)
		outColor = vec4(1.0,0.0,0.0,1.0);
		
	else if (mod (gl_PrimitiveID, 4.0) == 1.0)
		outColor = vec4(0.0,1.0,0.0,1.0);

	else if (mod (gl_PrimitiveID, 4.0) == 2.0)
		outColor = vec4(0.0,0.0,1.0,1.0);

	else if (mod (gl_PrimitiveID, 4.0) == 3.0)
		outColor = vec4(1.0);
	
}
