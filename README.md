# Informatica-Grafica
En este repositorio se encuentran diversos proyectos en C++, que utilizan GLSL y OpenGL e implementan distintas técnicas de iluminación, texturizado...
## P1 - GLSL
En este proyecto se realiza una escena sencilla, con el movimiento de cuerpos geométricos simples. Las funcionalidades que implementa son las siguientes:
### Matriz de proyección con Aspect Ratio
De esta forma, se conserva la proporción de la escena al reescalar la ventana
### Segundo cubo orbitante
Se crea un nuevo objeto, que se mueve alrededor del primer cubo
### Control de la cámara en primera persona con teclado y ratón
### Shaders condicionales
Se crean shaders que alteran el color de los fragmentos en función del ID de la primitiva. También se descartan fragmentos en función de las coordenadas de textura.
### Creación de un nuevo modelo
Se implementan manualmente nuevas coordenadas para crear un nuevo modelo, en este caso, una pirámide
