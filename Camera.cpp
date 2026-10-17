#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm> //para los cálculos
#include <cmath>

namespace PAG
{
    namespace
    {
        constexpr float LIMITE_POLO = 0.99f;
        constexpr float EPSILON = 1e-4f;
//movimientos que hará la cámara
        //rotar 
        glm::vec3 rotar (const glm::vec3& v, float grados, const glm::vec3& eje) //necesitamos vectores que definan el eje
        {
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(grados), eje);
            return glm::vec3(R * glm::vec4(v, 0.0f));   // w = 0: es un vector, no un punto
        }
//es para marcar los limites, es decir para saber si se pasa del límite por arriba o por abajo, si estuviese completamente en vertical al normalizar da error
        bool cercaDelPolo (const glm::vec3& direccion, const glm::vec3& up)
        {
            return std::abs(glm::dot(glm::normalize(direccion), glm::normalize(up))) > LIMITE_POLO;
        }
    }
//constructor donde definimos la posicion inicial y a donde mira 
    Camera::Camera ()
        : _posicion(0.0f, 0.0f, 3.0f), _puntoMira(0.0f, 0.0f, 0.0f), _up(0.0f, 1.0f, 0.0f),
          _fovX(glm::radians(60.0f)), _zNear(0.1f), _zFar(100.0f), _ancho(1024), _alto(576)
    { }
//vector normal es el que apunta hacia atras , opengl apunta ahcia atrás
    glm::vec3 Camera::vectorN () const { return glm::normalize(_posicion - _puntoMira);} //funciones para normalizar o productos
//vector que apunta a la derecha   
    glm::vec3 Camera::vectorU () const { return glm::normalize(glm::cross(_up, vectorN())); }
    //vector que apunta hacia arriba 
    glm::vec3 Camera::vectorV () const { return glm::cross(vectorN(), vectorU()); }
//matriz que pasa de las coordenadas del mundo a la cámara 
    glm::mat4 Camera::matrizVision () const
    {
        return glm::lookAt(_posicion, _puntoMira, _up);
    }
//coordenadas en cámara dentro del clip, recorte, aplicando perspectiva con los planos znear y zfar
    glm::mat4 Camera::matrizProyeccion () const
    {
        float aspecto = static_cast<float>(_ancho) / static_cast<float>(_alto);
        float fovY = 2.0f * std::atan(std::tan(_fovX / 2.0f) / aspecto);
        return glm::perspective(fovY, aspecto, _zNear, _zFar);
    }
//juntamos la sdos para hacerlo todo en un paso 
    glm::mat4 Camera::matrizVisionProyeccion () const
    {
        return matrizProyeccion() * matrizVision();   // el orden importa: P · V
    }

    void Camera::setViewport (int ancho, int alto)
    {
        if (ancho > 0 && alto > 0) { _ancho = ancho; _alto = alto; }
    }
//movimiento pan, movimiento en horizontal sin mover la cámara
    void Camera::pan (float grados)
    {
        glm::vec3 direccion = _puntoMira - _posicion;
        _puntoMira = _posicion + rotar(direccion, -grados, vectorV());
    }
//movimiento en vertical sin mover la cámara
    void Camera::tilt (float grados)
    {
        glm::vec3 direccion = _puntoMira - _posicion;
        glm::vec3 nueva = rotar(direccion, grados, vectorU());
        if (cercaDelPolo(nueva, _up)) return;       // evitamos la situación anómala
        _puntoMira = _posicion + nueva;
    }
//traslacion de la cámara en el eje horizontal
    void Camera::dolly (float lateral, float frontal)
    {
        glm::vec3 adelante = -vectorN();   adelante.y = 0.0f;   // proyectado al plano XZ
        glm::vec3 derecha  =  vectorU();   derecha.y  = 0.0f;
        adelante = (glm::length(adelante) > EPSILON) ? glm::normalize(adelante) : glm::vec3(0.0f);
        derecha  = (glm::length(derecha)  > EPSILON) ? glm::normalize(derecha)  : glm::vec3(0.0f);

        glm::vec3 desplazamiento = derecha * lateral + adelante * frontal;
        _posicion  += desplazamiento;     // se mueven los dos: la dirección no cambia
        _puntoMira += desplazamiento;
    }
//traslación en el eje vertical
    void Camera::crane (float dy)
    {
        glm::vec3 desplazamiento(0.0f, dy, 0.0f);
        _posicion  += desplazamiento;
        _puntoMira += desplazamiento;
    }
//giro de la cámara respecto un punto 
    void Camera::orbit (float latitud, float longitud)
    {
        glm::vec3 desde = _posicion - _puntoMira;   // vector del punto de mira a la cámara

        // Longitud: giro alrededor del eje Y del mundo
        desde = rotar(desde, longitud, glm::vec3(0.0f, 1.0f, 0.0f));

        // Latitud: giro alrededor de u (calculado con la posición ya girada)
        glm::vec3 u = glm::normalize(glm::cross(_up, glm::normalize(desde)));
        glm::vec3 nuevo = rotar(desde, -latitud, u);
        if (!cercaDelPolo(nuevo, _up)) desde = nuevo;

        _posicion = _puntoMira + desde;
    }
//Acercar la vista de la cámara
    void Camera::zoom (float incrementoGrados)
    {
        setZoom(glm::degrees(_fovX) + incrementoGrados);
    }

    void Camera::setZoom (float fovXGrados)
    {
        _fovX = glm::radians(std::clamp(fovXGrados, 10.0f, 120.0f));
    }
}