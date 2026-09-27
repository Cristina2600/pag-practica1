

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "GUI.h"

namespace PAG
{
    GUI* GUI::instancia = nullptr; //reservamos memoria inicialmente para el atributo static

    /**
     * Constructor por defecto
     */
    GUI::GUI ()
    {
        _bgColorPicker[0] = 0.6f; //color gris
        _bgColorPicker[1] = 0.6f;
        _bgColorPicker[2] = 0.6f;
    }

    /**
     * Destructor
     */
    GUI::~GUI ()
    { }

    /**
     * Consulta del objeto unico de la clase
     */
    GUI& GUI::getInstancia ()
    {
        if (!instancia)
        {
            instancia = new GUI; //por esto teniamos que iniciar antes instancia
        }
        return *instancia;
    }

    /**
     * Inicializa Dear ImGui para su uso con GLFW y OpenGL3.
     * Debe llamarse una sola vez, tras crear la ventana y el
     * contexto OpenGL.
     */
    void GUI::inicializar (GLFWwindow* window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext(); //crea contexto, lo que hace es guardar el estado y se llama una sola vez 
        ImGuiIO& io = ImGui::GetIO(); //esto es para entradas y salidas 
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; //esto es para la navegacion por teclado 

        ImGui_ImplGlfw_InitForOpenGL(window, true); //aqui conectamos nuestra ventana para recibir los eventos que sucedan
        ImGui_ImplOpenGL3_Init(); //shaders y buffers de opengl
    }

    /**
     * Construye la ventana de la interfaz.
     * Debe llamarse en cada vuelta del bucle principal, antes de
     * renderizar() y antes de dibujar la escena OpenGL.
     */
    void GUI::dibujarControles ()
    {
        //esta parte siempre va en este orden al inicio 
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Donde aparecerá la ventana, primero le dices la posicion y
        //ImGuiCond_once te dice que solo la dibujes una vez al inicio, si no la pongo no podemos mover la ventana porque se redibujaria
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
        if (ImGui::Begin("Fondo")) //crea la ventana llamandola fondo
        {
            ImGui::SetWindowFontScale(1.0f); //le da un tamaño al texto dentro de la ventana
            if (ImGui::ColorEdit3("Actual", _bgColorPicker)) //aqui colorEdit3 dibuja el control y modifica el color que hemos definido antes
            {
                // ColorEdit3 devuelve true cuando el
                // usuario ha modificado el color. Es aqui donde
                // notificamos a los observadores (patron Observador).
                AvisarListenersCambioFondo();
            }
        }
        ImGui::End(); //esto se llama siempre se abra o no la ventana por si acaso

        // Aqui tenemos otra ventana igual que antes pero para ver los mensajes
        ImGui::SetNextWindowPos(ImVec2(300, 10), ImGuiCond_Once);
        if (ImGui::Begin("Mensajes"))
        {
            ImGui::SetWindowFontScale(1.0f); //le damos un tamaño a la letra
            for (const std::string& m : _mensajes)  //recorremos todos los mensajes
            {
                ImGui::TextUnformatted(m.c_str()); //aqui usamos texto sin formato por si hay algun caracter raro que pueda dar problemas
            }
        }
        ImGui::End();
    }

   //esto NO DIBUJA NADA solo calcula vertices y todo lo que hemos definido anteriormente, esto llama a opengl para que lo pinte
    void GUI::renderizar ()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    /**
     * Libera los recursos de Dear ImGui. Debe llamarse ANTES de
     * destruir la ventana GLFW.
     */
    void GUI::liberar ()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    /**
     * Suscribe un observador a las notificaciones de esta GUI
     */
    void GUI::anadirListener (Listener* listener)
    {
        _listeners.push_back(listener); //funciones de los vectores dinámicos
    }

    /**
     * Anade un mensaje al historial mostrado en la ventana "Mensajes"
     */
    void GUI::aniadirMensaje (const std::string& mensaje)
    {
        _mensajes.push_back(mensaje);
    }

    /**
     * Recorre la lista de observadores y notifica el nuevo color de fondo
     */
    void GUI::AvisarListenersCambioFondo ()
    {
        for (size_t i = 0; i < _listeners.size(); i++)
        {
            _listeners[i]->wakeUp(WindowType::Background, &_bgColorPicker[0]); //usamos la funcion wake up para notificarles el cambio
        }
    }
}