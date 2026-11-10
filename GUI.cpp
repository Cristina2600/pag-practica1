#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "GUI.h"
#include <imgui_stdlib.h>

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

        //vamos a aplicar un cambio en la interfaz
        aplicarEstiloMorado();

        ImGui_ImplGlfw_InitForOpenGL(window, true); //aqui conectamos nuestra ventana para recibir los eventos que sucedan
        ImGui_ImplOpenGL3_Init(); //shaders y buffers de opengl
    }

    /**
     * Construye el panel unico de la interfaz: una ventana fija arriba a la
     * izquierda con una seccion desplegable por cada grupo de controles.
     * Debe llamarse en cada vuelta del bucle principal, antes de
     * renderizar() y antes de dibujar la escena OpenGL.
     */
    void GUI::dibujarControles ()
    {
        //esta parte siempre va en este orden al inicio 
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Posicion y tamaño forzados cada fotograma + flags: el panel no se puede mover,
        // redimensionar ni colapsar
        const ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::SetNextWindowSize(ImVec2(360, io.DisplaySize.y - 20));
        const ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove
                                     | ImGuiWindowFlags_NoResize
                                     | ImGuiWindowFlags_NoCollapse
                                     | ImGuiWindowFlags_NoSavedSettings;

        if (ImGui::Begin("Panel de control", nullptr, flags))
        {
            // PushID/PopID: todos los controles estan en la misma ventana, asi que
            // etiquetas repetidas (p.ej. dos combos "Modelo") no deben chocar
            if (ImGui::CollapsingHeader("Fondo"))
            {
                ImGui::PushID("fondo");
                dibujarSeccionFondo();
                ImGui::PopID();
            }

            if (ImGui::CollapsingHeader("Shader Program"))
            {
                ImGui::PushID("shader");
                dibujarSeccionShader();
                ImGui::PopID();
            }

            if (ImGui::CollapsingHeader("Camera"))
            {
                ImGui::PushID("camara");
                dibujarSeccionCamara();
                ImGui::PopID();
            }

            if (ImGui::CollapsingHeader("Modelos", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::PushID("modelos");
                _ventanaModelos.dibujarContenido();   // cargar y eliminar
                if (ImGui::TreeNode("Transformaciones"))
                {
                    _ventanaTransformaciones.dibujarContenido();
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }

            if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::PushID("material");
                if (ImGui::TreeNode("Propiedades del material"))
                {
                    _ventanaMaterial.DibujarContenido();
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Visualizacion"))
                {
                    dibujarSeccionVisualizacion();
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Luces"))
                {
                    ImGui::TextUnformatted("Pendiente: se anadiran con la iluminacion");
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }

            if (ImGui::CollapsingHeader("Mensajes", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::PushID("mensajes");
                dibujarSeccionMensajes();
                ImGui::PopID();
            }
        }
        ImGui::End(); //esto se llama siempre se abra o no la ventana

        // El selector de archivos es una ventana flotante propia: va FUERA del panel
        _ventanaModelos.dibujarSelector();
    }

    // ------------------------------------------------------------------
    // Secciones del panel (solo controles, sin Begin/End)
    // ------------------------------------------------------------------

    void GUI::dibujarSeccionFondo ()
    {
        if (ImGui::ColorEdit3("Actual", _bgColorPicker)) //colorEdit3 devuelve true cuando el usuario cambia el color
        {
            AvisarListenersCambioFondo(); // patron Observador
        }
    }

    void GUI::dibujarSeccionShader ()
    {
        ImGui::InputText("##nombre", &_nombreShader, ImGuiInputTextFlags_AutoSelectAll);
        if (ImGui::Button("Load")) //así se llama el boton
        {
            try
            {
                for (size_t i = 0; i < _listeners.size(); i++)
                {
                    _listeners[i]->wakeUp(WindowType::ShaderProgram, _nombreShader.c_str());
                }
                aniadirMensaje("Shader program '" + _nombreShader + "' cargado correctamente");
            }
            catch (const std::exception& e)
            {
                aniadirMensaje(e.what());
            }
        }
    }

    void GUI::dibujarSeccionMensajes ()
    {
        // Zona con scroll propio para que el historial no alargue el panel
        ImGui::BeginChild("##historial", ImVec2(0, 160), true);
        for (const std::string& m : _mensajes)
        {
            ImGui::TextWrapped("%s", m.c_str()); //%s: asi un '%' dentro del mensaje no se interpreta
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);   // baja solo si ya estabas al final
        }
        ImGui::EndChild();
    }

    void GUI::dibujarSeccionCamara ()
    {
        static const char* nombres[] = { "Zoom", "Pan", "Tilt", "Dolly", "Crane", "Orbit" };
        ImGui::Text("Movement");
        if (ImGui::Combo("##movimiento", &_movimiento, nombres, IM_ARRAYSIZE(nombres)))
        {
            for (size_t i = 0; i < _listeners.size(); i++)
                _listeners[i]->wakeUp(WindowType::CameraSelect, _movimiento);   // int
        }

        const double P = 10.0;   // "píxeles" equivalentes por pulsación
        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);  // mantener pulsado repite

        switch (static_cast<CameraMovement>(_movimiento))
        {
            case CameraMovement::Zoom:
                ImGui::Text("Angle");
                if (ImGui::SliderFloat("##fov", &_fovX, 10.0f, 120.0f, "%.0f"))
                {
                    for (size_t i = 0; i < _listeners.size(); i++)
                        _listeners[i]->wakeUp(WindowType::CameraZoom, static_cast<double>(_fovX));
                }
                break;
            case CameraMovement::Pan:
                ImGui::Text("Direction");
                if (ImGui::Button("<- Left"))  notificarMovimiento(-P, 0.0);
                ImGui::SameLine();
                if (ImGui::Button("Right ->")) notificarMovimiento( P, 0.0);
                break;
            case CameraMovement::Tilt:
            case CameraMovement::Crane:
                ImGui::Text("Direction");
                if (ImGui::Button("^ Up ^"))   notificarMovimiento(0.0,  P);
                ImGui::SameLine();
                if (ImGui::Button("v Down v")) notificarMovimiento(0.0, -P);
                break;
            case CameraMovement::Dolly:
                ImGui::Text("Direction");
                if (ImGui::Button("^ Forward ^")) notificarMovimiento(0.0,  P);
                if (ImGui::Button("<- Left"))     notificarMovimiento(-P, 0.0);
                ImGui::SameLine();
                if (ImGui::Button("Right ->"))    notificarMovimiento( P, 0.0);
                if (ImGui::Button("v Back v"))    notificarMovimiento(0.0, -P);
                break;
            case CameraMovement::Orbit:
                ImGui::Text("Latitude");
                if (ImGui::Button("^ North ^")) notificarMovimiento(0.0,  P);
                ImGui::SameLine();
                if (ImGui::Button("v South v")) notificarMovimiento(0.0, -P);
                ImGui::Text("Longitude");
                if (ImGui::Button("<- West"))   notificarMovimiento(-P, 0.0);
                ImGui::SameLine();
                if (ImGui::Button("East ->"))   notificarMovimiento( P, 0.0);
                break;
        }
        ImGui::PopItemFlag();
    }

    void GUI::dibujarSeccionVisualizacion ()
    {
        bool cambiado = false;
        cambiado |= ImGui::RadioButton("Alambre", &_modoVisualizacion,
                                       static_cast<int>(ModoVisualizacion::Alambre));
        ImGui::SameLine();
        cambiado |= ImGui::RadioButton("Solido", &_modoVisualizacion,
                                       static_cast<int>(ModoVisualizacion::Solido));

        if (cambiado)
        {
            for (size_t i = 0; i < _listeners.size(); i++)
            {
                _listeners[i]->wakeUp(WindowType::RenderMode, _modoVisualizacion);
            }
        }
        if (ImGui::ColorEdit3("Color alambre", _colorAlambre))
        {
            for (size_t i = 0; i < _listeners.size(); i++)
            {
                _listeners[i]->wakeUp(WindowType::WireColor, &_colorAlambre[0]);
            }
        }
    }

    // ------------------------------------------------------------------

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
        _ventanaModelos.addListener(listener); //aqui añadimos los listeners de los modelos obj 
        _ventanaTransformaciones.addListener(listener);  //registramos los listeners para amnejar lo cambios en transformaciones 
        _ventanaMaterial.addListener(listener);
    }

    /**
     * Anade un mensaje al historial mostrado en la seccion "Mensajes"
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

    //le cambiamos los colores a morado al inicializar toda la interfaz, para que se haga una sola vez
    void GUI::aplicarEstiloMorado ()
    {
        ImGuiStyle& estilo = ImGui::GetStyle();
        ImVec4* colores = estilo.Colors;

        // Barra de titulo: morado MEDIO (ni muy oscuro ni muy claro, para que el texto negro se lea)
        colores[ImGuiCol_TitleBg]          = ImVec4(0.62f, 0.45f, 0.75f, 1.00f);
        colores[ImGuiCol_TitleBgActive]    = ImVec4(0.68f, 0.52f, 0.80f, 1.00f);
        colores[ImGuiCol_TitleBgCollapsed] = ImVec4(0.62f, 0.45f, 0.75f, 0.75f);
        colores[ImGuiCol_PopupBg]      = ImVec4(0.80f, 0.90f, 1.00f, 1.00f); //color del desplegable

        colores[ImGuiCol_WindowBg] = ImVec4(0.88f, 0.82f, 0.94f, 1.00f);

        colores[ImGuiCol_FrameBg]        = ImVec4(0.78f, 0.68f, 0.90f, 1.00f);
        colores[ImGuiCol_FrameBgHovered] = ImVec4(0.70f, 0.58f, 0.85f, 1.00f);
        colores[ImGuiCol_Button]         = ImVec4(0.45f, 0.20f, 0.60f, 1.00f);
        colores[ImGuiCol_ButtonHovered]  = ImVec4(0.55f, 0.28f, 0.70f, 1.00f);
        colores[ImGuiCol_ButtonActive]   = ImVec4(0.35f, 0.14f, 0.48f, 1.00f);

        // Cabeceras de las secciones desplegables (CollapsingHeader / TreeNode) en morado
        colores[ImGuiCol_Header]        = ImVec4(0.62f, 0.45f, 0.75f, 1.00f);
        colores[ImGuiCol_HeaderHovered] = ImVec4(0.68f, 0.52f, 0.80f, 1.00f);
        colores[ImGuiCol_HeaderActive]  = ImVec4(0.55f, 0.38f, 0.70f, 1.00f);

        colores[ImGuiCol_Text] = ImVec4(0.10f, 0.05f, 0.15f, 1.00f);   // negro, legible en ambas zonas
    }

    void GUI::notificarMovimiento (double horizontal, double vertical)
    {
        for (size_t i = 0; i < _listeners.size(); i++)
            _listeners[i]->wakeUp(WindowType::CameraMove, horizontal, vertical); //recorres todos los listeners para notificar los cambios en la cámara
    }

    std::string GUI::recogerMensajePendiente ()
    {
        std::string m = _mensajePendiente;
        _mensajePendiente.clear();
        return m;
    }
}