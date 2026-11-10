#version 410

out vec4 colorFragmento;

uniform vec3 uColorDifuso;   // color difuso del material, lo envia el Renderer
uniform vec3 uColorAlambre;  // color del alambre, lo envia el Renderer

// 1. Tipo de la subrutina
subroutine vec4 fCalcularColor ();

// 2. Uniform que representa la implementacion elegida
subroutine uniform fCalcularColor uMetodoColorElegido;

// 3. Implementaciones
subroutine ( fCalcularColor )
vec4 colorAlambre ()
{
    return vec4 ( uColorAlambre, 1.0 );    // color editable desde la GUI
}

subroutine ( fCalcularColor )
vec4 colorSolido ()
{
    return vec4 ( uColorDifuso, 1.0 );     // difuso del material
}

void main ()
{
    // 4. Llama a la que haya activado el programa C++
    colorFragmento = uMetodoColorElegido ();
}