#ifndef LIGHTWINDOW_H
#define LIGHTWINDOW_H

#include <vector>

#include "Listener.h"
#include "Luz.h"

namespace PAG
{
    /**
     * @brief Contenido de la seccion "Luces": lista, alta, baja y edicion de las luces.
     *        No toca la escena: notifica al Renderer (patron Observador).
     */
    class LightWindow
    {
        private:
            std::vector<Listener*> _listeners;
            int _seleccion = 0;    // luz elegida en la lista
            int _tipoNueva = 1;    // tipo de la proxima luz que se anada (por defecto Puntual)

            void notificarAlta (int tipo);
            void notificarBaja (int indice);
            void notificarCambio (int indice, const Luz& luz);

        public:
            void addListener (Listener* listener);
            void dibujarContenido ();
    };
}

#endif //LIGHTWINDOW_H