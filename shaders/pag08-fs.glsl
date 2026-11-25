#version 410

// Bloque de interfaz de entrada: mismo nombre de bloque que en el vertex shader
in SalidaVS
{
    vec3 posicionVision;
    vec3 normalVision;
} entrada;

out vec4 colorFragmento;

// ---- Material del modelo ----
uniform vec3  uKa;   // reflexion ambiente
uniform vec3  uKd;   // reflexion difusa
uniform vec3  uKs;   // reflexion especular
uniform float uNs;   // exponente especular

// ---- Luz que se esta procesando en esta pasada (en espacio de vision) ----
uniform vec3  uIa;
uniform vec3  uId;
uniform vec3  uIs;
uniform vec3  uPosicionLuz;
uniform vec3  uDireccionLuz;   // desde la fuente hacia la escena, normalizada
uniform float uCosGamma;       // coseno de la semiapertura del foco
uniform float uS;              // exponente de bordes suaves del foco

uniform vec3  uColorAlambre;

// Tipo de la subrutina y uniform que representa la implementacion elegida
subroutine vec3 fCalcularColor ();
subroutine uniform fCalcularColor uMetodoColorElegido;

// Reflexion difusa + especular para una direccion l hacia la luz
vec3 difusaYEspecular ( vec3 l, vec3 n, vec3 v )
{
    float nl = dot ( n, l );
    if ( nl <= 0.0 )
    {
        return vec3 ( 0.0 );          // la luz llega por detras de la superficie
    }
    vec3 difusa = uId * uKd * nl;

    vec3 r = reflect ( -l, n );
    vec3 especular = uIs * uKs * pow ( max ( dot ( r, v ), 0.0 ), uNs );

    return difusa + especular;
}

subroutine ( fCalcularColor )
vec3 colorAlambre ()
{
    return uColorAlambre;
}

subroutine ( fCalcularColor )
vec3 sinLuz ()
{
    return vec3 ( 0.0 );
}

subroutine ( fCalcularColor )
vec3 luzAmbiente ()
{
    return uIa * uKa;
}

subroutine ( fCalcularColor )
vec3 luzPuntual ()
{
    vec3 n = normalize ( entrada.normalVision );
    vec3 v = normalize ( -entrada.posicionVision );
    vec3 l = normalize ( uPosicionLuz - entrada.posicionVision );
    return difusaYEspecular ( l, n, v );
}

subroutine ( fCalcularColor )
vec3 luzDireccional ()
{
    vec3 n = normalize ( entrada.normalVision );
    vec3 v = normalize ( -entrada.posicionVision );
    vec3 l = -uDireccionLuz;
    return difusaYEspecular ( l, n, v );
}

subroutine ( fCalcularColor )
vec3 luzFoco ()
{
    vec3 n = normalize ( entrada.normalVision );
    vec3 v = normalize ( -entrada.posicionVision );
    vec3 l = normalize ( uPosicionLuz - entrada.posicionVision );

    float cosAlfa = dot ( -l, uDireccionLuz );
    if ( cosAlfa < uCosGamma )
    {
        return vec3 ( 0.0 );          // fuera del cono
    }
    float factor = pow ( max ( cosAlfa, 0.0 ), uS );   // bordes suaves
    return factor * difusaYEspecular ( l, n, v );
}

void main ()
{
    colorFragmento = vec4 ( uMetodoColorElegido (), 1.0 );
}