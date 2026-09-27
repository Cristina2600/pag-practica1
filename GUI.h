//encapsulamos la funcionalidad de dear imgui para usar el patrón singleton

#ifndef GUI_H
#define GUI_H

#include <vector> //vamos a necesitar un vector dinámico para almacenar a todos los listeners
#include <string>

struct GLFWwindow; //forward declaration, es solo para declarar la funcion inicializar más abajo

#include "Listener.h" //vamos a necesitar un vector de objetos que escuchen los cambios 

namespace PAG //vamos a usar siempre nuestro espacio de nombres
{
    /** @brief aqui vamos a tener toda la comunicacion Dear ImGui para el patron singleton
     * Ademas tendremos todos los objetos que cambian de estado y modifica a los que escuchan a traves de los listeners
     * 
     */

    class GUI
    {
        private:
            static GUI* instancia;  //puntero a si mismo, base del patron singleton

            std::vector<Listener*> _listeners;  // todos los observadores
            float _bgColorPicker[3];             // Color mostrado en el selector, esta es la variable que se modifica directamente mediante imgui
            std::vector<std::string> _mensajes;  // todos los mensajes para que se muestren en una ventanita

            GUI (); //el constructor es privado para que no se pueda instanciar desde ningun lado

            // Notifica a todos los observadores un cambio en el color de fondo
            void AvisarListenersCambioFondo ();

        public:
            virtual ~GUI (); //vamos a tener un destructor virtual por si luego hay clases hijas 
            static GUI& getInstancia (); //así es la única forma de acceder al objeto

            void inicializar (GLFWwindow* window);
            void dibujarControles ();
            void renderizar ();
            void liberar ();

            void anadirListener (Listener* listener); //metodo para que los objetos renderer se subscriban
            void aniadirMensaje (const std::string& mensaje);
    };
}

#endif //GUI_H