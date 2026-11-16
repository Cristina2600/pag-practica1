#ifndef LUZ_H
#define LUZ_H

#include <glm/glm.hpp>

#include "TipoLuz.h"

namespace PAG
{
    /**
     * @brief Una fuente de luz. Una unica clase con un atributo de tipo: cambiar el
     *        tipo de una luz no pierde los datos de los demas tipos (si pasas de
     *        direccional a puntual y vuelves, la direccion sigue ahi).
     *        Solo contiene tipos por valor, asi que las copias generadas por el
     *        compilador son correctas (regla del cero).
     *
     *        Posicion y direccion estan en espacio del MUNDO. La direccion va desde
     *        la fuente hacia la escena. Los angulos estan en GRADOS.
     */
    class Luz
    {
        private:
            TipoLuz   _tipo      = TipoLuz::Puntual;
            bool      _encendida = true;
            glm::vec3 _ia        = glm::vec3(0.2f);              // color ambiente
            glm::vec3 _id        = glm::vec3(0.6f);              // color para reflexion difusa
            glm::vec3 _is        = glm::vec3(0.6f);              // color para reflexion especular
            glm::vec3 _posicion  = glm::vec3(0.0f, 0.0f, 3.0f);  // puntual y foco
            glm::vec3 _direccion = glm::vec3(0.0f, 0.0f, -1.0f); // direccional y foco
            float     _gamma     = 30.0f;                        // foco: angulo de apertura (grados)
            float     _s         = 8.0f;                         // foco: exponente de bordes suaves

        public:
            Luz () = default;
            explicit Luz (TipoLuz tipo) : _tipo(tipo) {}

            TipoLuz getTipo () const              { return _tipo; }
            bool estaEncendida () const           { return _encendida; }
            const glm::vec3& getIa () const       { return _ia; }
            const glm::vec3& getId () const       { return _id; }
            const glm::vec3& getIs () const       { return _is; }
            const glm::vec3& getPosicion () const { return _posicion; }
            const glm::vec3& getDireccion () const{ return _direccion; }
            float getGamma () const               { return _gamma; }
            float getS () const                   { return _s; }

            void setTipo (TipoLuz t)                    { _tipo = t; }
            void setEncendida (bool e)                  { _encendida = e; }
            void setIa (const glm::vec3& c)             { _ia = c; }
            void setId (const glm::vec3& c)             { _id = c; }
            void setIs (const glm::vec3& c)             { _is = c; }
            void setPosicion (const glm::vec3& p)       { _posicion = p; }
            void setDireccion (const glm::vec3& d)      { _direccion = d; }
            void setGamma (float g)                     { _gamma = g; }
            void setS (float s)                         { _s = s; }

            // Que atributos tienen sentido segun el tipo (los usara la interfaz y el envio al shader)
            bool usaIa () const        { return _tipo == TipoLuz::Ambiente; }
            bool usaIdIs () const      { return _tipo != TipoLuz::Ambiente; }
            bool usaPosicion () const  { return _tipo == TipoLuz::Puntual || _tipo == TipoLuz::Foco; }
            bool usaDireccion () const { return _tipo == TipoLuz::Direccional || _tipo == TipoLuz::Foco; }
            bool usaCono () const      { return _tipo == TipoLuz::Foco; }
    };
}

#endif //LUZ_H