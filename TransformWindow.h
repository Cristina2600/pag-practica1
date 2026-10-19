#ifndef TRANSFORMWINDOW_H
#define TRANSFORMWINDOW_H

#include <vector>

#include "Listener.h"
#include "Model.h"
//Ahora podremos cargar los modelos y aplicarles transformaciones , nuevamente tendremos una lista de listeners 
namespace PAG
{
    /**
     * @brief Ventana "Transformaciones": elegir un modelo y aplicarle traslacion,
     *        rotacion y escala. Notifica al Renderer (patron Observador).
     */
    class TransformWindow
    {
        private:
            std::vector<Listener*> _listeners;
            int _seleccionado = 0;   // modelo elegido en el combo

            void notificar (int indice, const Transformacion& t);

        public:
            void addListener (Listener* listener);
            void dibujar ();
    };
}

#endif //TRANSFORMWINDOW_H