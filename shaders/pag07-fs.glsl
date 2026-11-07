#version 410

out vec4 colorFragmento;

uniform vec3 uColorDifuso;   // color difuso del material, lo envia el Renderer

// Definimos la subrutina
subroutine vec4 fCalcularColor ();

// 2.implementacion para la subrutina, declaramos
subroutine uniform fCalcularColor uMetodoColorElegido;

// 3. Implementaciones
subroutine ( fCalcularColor )
vec4 colorAlambre ()
{
    return vec4 ( 1.0, 0.0, 0.0, 1.0 );    // rojo fijo
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