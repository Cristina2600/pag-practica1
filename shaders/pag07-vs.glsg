#version 410
layout (location = 0) in vec3 posicion;
layout (location = 1) in vec3 color;   // el VAO lo sigue enviando, pero ya no se usa

uniform mat4 matrizMVP;

void main ()
{
    gl_Position = matrizMVP * vec4 ( posicion, 1 );
}