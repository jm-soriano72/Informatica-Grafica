# Informatica-Grafica
En este repositorio se encuentran diversos proyectos en C++, que utilizan GLSL y OpenGL e implementan distintas técnicas de iluminación, texturizado...

![Imagen de ejemplo](https://github.com/jm-soriano72/Informatica-Grafica/blob/main/Im%C3%A1genes/1.PNG)
![Imagen de ejemplo 2](https://github.com/jm-soriano72/Informatica-Grafica/blob/main/Im%C3%A1genes/2.PNG)
![Imagen de ejemplo 3](https://github.com/jm-soriano72/Informatica-Grafica/blob/main/Im%C3%A1genes/3.PNG)

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
## P2 - GLSL
En este proyecto se lleva a cabo la iluminación y el texturizado de un modelo sencillo mediante distintas técnicas.
### Iluminación con distintas fuentes de luz
Se definen dos fuentes de luz en el shader de fragmentos (shader.v4.frag). Las coordenadas de las luces se transforman a coordenadas de cámara y se calcula el sombreado combinando componentes especulares y difusas.
### Atenuación de la intensidad lumínica
Se atenúa la intensidad de la luz en función de la distancia entre la fuente de luz y el objeto.
### Fuentes direccionales y focales
Luz Direccional: Implementada con parámetros de intensidad y dirección
Luz Focal: Con parámetros de intensidad, dirección, posición y ángulo de apertura
### Implementación de niebla
Se añade niebla que difumina el objeto con el fondo usando una ecuación de niebla. Se definen factores de densidad para niebla de profundidad y altura
## P3 - OpenGL
En este proyecto, se implementan la mayor parte de funcionalidades anteriores en el lenguaje de OpenGL, además de alguna adicional
### Control dinámico de las propiedades de la luz mediante teclado
### Matriz de proyección
### Control de la cámara con teclado y ratón
### Nuevo objeto en la escena
Se añade un nuevo cubo, con una textura diferente al del primero
### Implementación de un filtro anisotrópico
### Iluminación avanzada con distintos tipos de fuentes
### Efecto de niebla
## P4 - OpenGL
En este proyecto se añaden distintos filtros y efectos a una escena
### Control dinámico del Motion Blur
Se modifican los parámetros del desenfoque mediante teclado
### Manejo del Depth of Field
Se controla el desenfoque de los objetos en función de su distancia a la cámara
### Buffer de Profundidad
### Máscaras de convolución
Se implementa la elección entre diversas opciones mediante teclado

