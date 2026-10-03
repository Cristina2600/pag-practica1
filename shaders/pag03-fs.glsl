#version 410
in vec3 colorVertice;

out vec4 colorFragmento;

void main ()
{
    colorFragmento = vec4 ( colorVertice, 1.0 );
}