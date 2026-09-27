//esta clase va a ser abstracta para crear el observador

#ifndef LISTENER_H
#define LISTENER_H

#include "WindowType.h" //aqui tenemos que incluirlo entero porque luego manejaremos distintos tipos en el enum

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
            Listener () = default; //constructor por defecto, en este caso no debe ser privado porque habrá más de una instancia
            virtual ~Listener () = default; //destructor obligatoriamente virtual por el poliformismo que implementamos con renderer 

            //este es el método que va a notificar, es virtual puro para que el resto de clases lo tengan que implementar
            //
            virtual void wakeUp (WindowType t, ...) = 0;
    };
}

#endif 