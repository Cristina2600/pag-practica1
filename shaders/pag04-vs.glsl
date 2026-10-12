#version 410
layout (location = 0) in vec3 posicion;
layout (location = 1) in vec3 color;
// matriz uniforme para amntener las proporciones del triangulo
uniform mat4 correccionAspecto;
out vec3 colorVertice;

void main ()
{
    colorVertice = color;
    // aqui se recalcula la posicion de los vertices
    gl_Position = correccionAspecto * vec4 ( posicion, 1 );
}