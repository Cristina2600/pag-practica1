//encapsulamos la funcionalidad de dear imgui para usar el patrón singleton

#ifndef GUI_H
#define GUI_H

#include <vector>
#include <string>

struct GLFWwindow; //hay que ponerlo antes de listener porque lo necesitará pero no necesita saber que hay dentro

#include "Listener.h"

namespace PAG
{
    /**
     * @brief Clase encargada de encapsular toda la comunicacion con
     *        Dear ImGui. Se implementa como Singleton, igual que
     *        Renderer, para que sea accesible desde cualquier modulo.
     *
     * Ademas actua como "sujeto" del patron Observador: cuando el
     * usuario modifica un control, notifica a los objetos suscritos
     * (Listener) a traves de wakeUp().
     */
    class GUI
    {
        private:
            static GUI* instancia;  ///< Puntero al unico objeto

            std::vector<Listener*> _listeners;  ///< todos los observadores
            float _bgColorPicker[3];             ///< Color mostrado en el selector
            std::vector<std::string> _mensajes;  ///< Historial para la ventana de mensajes

            GUI ();

            /// Notifica a todos los observadores un cambio en el color de fondo
            void warnListenersBackground ();

        public:
            virtual ~GUI ();
            static GUI& getInstancia ();

            void inicializar (GLFWwindow* window);
            void dibujarControles ();
            void renderizar ();
            void liberar ();

            void addListener (Listener* listener);
            void aniadirMensaje (const std::string& mensaje);
    };
}

#endif //GUI_H