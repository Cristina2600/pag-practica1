#version 410

layout (location = 0) in vec3 posicion;
layout (location = 1) in vec3 normal;

uniform mat4 matrizMVP;      // proyeccion * vision * modelado
uniform mat4 matrizMV;       // vision * modelado
uniform mat3 matrizNormal;   // transpose(inverse(mat3(MV)))

// Bloque de interfaz de salida: agrupa lo que se envia al fragment shader
out SalidaVS
{
    vec3 posicionVision;
    vec3 normalVision;
} salida;

void main ()
{
    salida.posicionVision = vec3 ( matrizMV * vec4 ( posicion, 1 ) );
    salida.normalVision   = normalize ( matrizNormal * normal );
    gl_Position           = matrizMVP * vec4 ( posicion, 1 );
}