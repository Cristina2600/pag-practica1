//esta clase va a ser abstracta para crear el observador

#ifndef LISTENER_H
#define LISTENER_H

#include "WindowType.h"

namespace PAG
{
    /**
     * @brief Interfaz que deben implementar todas las clases que quieran
     *        recibir notificaciones del renderer
     *.
     */
    class Listener
    {
        public:
            Listener () = default; //constructor
            virtual ~Listener () = default; //metodo virtual

            /**
             *esto es lo que se llama para notificar el cambio
             * El tipo de argumentos variables depende del
             * WindowType recibido.
             */
            virtual void wakeUp (WindowType t, ...) = 0;
    };
}

#endif 