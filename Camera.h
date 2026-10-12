#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

namespace PAG
{
    /**
     *Esta es la clase virtual de la cámara, solo hace calculos y cuentas por si se cambia algun mov de la cámara
     *separar la lógica
     */
    class Camera
    {
        private:
            glm::vec3 _posicion;
            glm::vec3 _puntoMira;
            glm::vec3 _up;
            float _fovX;            // ángulo de visión horizontal, en radianes
            float _zNear, _zFar;
            int _ancho, _alto;      // viewport

            glm::vec3 vectorN () const;   // hacia atrás
            glm::vec3 vectorU () const;   // derecha
            glm::vec3 vectorV () const;   // arriba

        public:
            Camera ();

            glm::mat4 matrizVision () const;
            glm::mat4 matrizProyeccion () const;
            glm::mat4 matrizVisionProyeccion () const;

            void setViewport (int ancho, int alto);

            void pan   (float grados);                // + = mira a la derecha
            void tilt  (float grados);                // + = mira arriba
            void dolly (float lateral, float frontal);// + = derecha / adelante
            void crane (float dy);                    // + = sube
            void orbit (float latitud, float longitud);// + = sube / hacia la derecha
            void zoom  (float incrementoGrados);      // modifica el fovX
            void setZoom (float fovXGrados);          // fija el fovX
    };
}

#endif