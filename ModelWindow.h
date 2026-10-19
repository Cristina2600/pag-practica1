#ifndef MODELWINDOW_H
#define MODELWINDOW_H

#include <string>
#include <vector>

#include <imfilebrowser.h>

#include "Listener.h"
#include "Renderer.h"
//clase que necesitamos para cargar los modelos pero separando la lógica para aplicar el patrón observador
//volvemos a tener un vector de listeners para notificar los cambios 
namespace PAG
{
    /**
     * @brief Ventana "Modelos": cargar un OBJ pudiendo seleccionar y elimianr modelos.
     *        No toca la escena: notifica al Renderer (patron Observador).
     */
    class ModelWindow
    {
        private:
            std::vector<Listener*> _listeners; //vector con todos los listeners
            ImGui::FileBrowser _selector;   // dialogo de seleccion de archivos
            int _seleccionado = -1;         // modelos seleccionado, iniciado en -1 que no es ninguno

            void notificarCarga (const std::string& ruta);
            void notificarBorrado (int indice);

        public:
            ModelWindow ();
            void addListener (Listener* listener);
            void dibujar ();
    };
}

#endif //MODELWINDOW_H