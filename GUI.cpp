

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "GUI.h"

namespace PAG
{
    GUI* GUI::instancia = nullptr;

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
            instancia = new GUI;
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
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();
    }

    /**
     * Construye el contenido de todas las ventanas de la interfaz.
     * Debe llamarse en cada vuelta del bucle principal, antes de
     * renderizar() y antes de dibujar la escena OpenGL.
     */
    void GUI::dibujarControles ()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- Ventana: selector de color de fondo ---
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
        if (ImGui::Begin("Fondo"))
        {
            ImGui::SetWindowFontScale(1.0f);
            if (ImGui::ColorEdit3("Actual", _bgColorPicker))
            {
                // ColorEdit3 devuelve true SOLO en el frame en que el
                // usuario ha modificado el color. Es aqui donde
                // notificamos a los observadores (patron Observador).
                warnListenersBackground();
            }
        }
        ImGui::End();

        // --- Ventana: mensajes de la aplicacion ---
        ImGui::SetNextWindowPos(ImVec2(300, 10), ImGuiCond_Once);
        if (ImGui::Begin("Mensajes"))
        {
            ImGui::SetWindowFontScale(1.0f);
            for (const std::string& m : _mensajes)
            {
                ImGui::TextUnformatted(m.c_str());
            }
        }
        ImGui::End();
    }

    /**
     * Renderiza la interfaz construida en dibujarControles().
     * Debe llamarse DESPUES de dibujar la escena OpenGL, porque
     * ImGui se dibuja por encima del contenido del viewport.
     */
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
    void GUI::addListener (Listener* listener)
    {
        _listeners.push_back(listener);
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
    void GUI::warnListenersBackground ()
    {
        for (size_t i = 0; i < _listeners.size(); i++)
        {
            _listeners[i]->wakeUp(WindowType::Background, &_bgColorPicker[0]);
        }
    }
}