#ifndef MATERIALWINDOW_H
#define MATERIALWINDOW_H

#include <vector>
#include "Listener.h"
#include "Renderer.h"

namespace PAG
{
    // Ventana para elegir el material que añadiremos: edita el material del modelo elegido. Notifica al Renderer.
    class MaterialWindow
    {
        private:
            std::vector<Listener*> _listeners; //necesitamos los listeners para notificar
            int _seleccionado = 0;

            void notificarMaterial (int indice, float* amb, float* dif,
                                    float* esp, float exponente); //valores para definir el material 
        public:
            void addListener (Listener* listener);
            void dibujar ();
    };
}

#endif //MATERIALWINDOW_H