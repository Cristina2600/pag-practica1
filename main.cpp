#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

#include "Renderer.h"
#include "GUI.h"

void error_callback(int errno_, const char* desc)
{
    std::string aux(desc);
    PAG::GUI::getInstancia().aniadirMensaje("Error de GLFW numero " + std::to_string(errno_) + ": " + aux);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    PAG::Renderer::getInstancia().redimensionar(width, height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (action == GLFW_PRESS)
    {
        io.AddMouseButtonEvent(button, true);
        PAG::GUI::getInstancia().aniadirMensaje("Pulsado el boton: " + std::to_string(button));
    }
    else if (action == GLFW_RELEASE)
    {
        io.AddMouseButtonEvent(button, false);
        PAG::GUI::getInstancia().aniadirMensaje("Soltado el boton: " + std::to_string(button));
    }
}

int main()
{
    if (glfwInit() != GLFW_TRUE)
    {
        std::cout << "Fallo al iniciar GLFW" << std::endl;
        return -1;
    }

    glfwSetErrorCallback((GLFWerrorfun) error_callback);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow *window = glfwCreateWindow(1024, 576, "Practica 2 PAG", nullptr, nullptr);

    if (window == nullptr)
    {
        std::cout << "Error al crear la ventana" << std::endl;
        glfwTerminate();
        return -2;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Inicializacion fallida de GLAD" << std::endl;
        glfwDestroyWindow(window);
        window = nullptr;
        glfwTerminate();
        return -3;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    PAG::Renderer::getInstancia().inicializarOpenGL();
    PAG::GUI::getInstancia().inicializar(window);

    // --- Patron Observador: Renderer se suscribe a las notificaciones de GUI ---
    PAG::GUI::getInstancia().addListener(&PAG::Renderer::getInstancia());

    // Los mensajes ya no van a consola, van a la ventana "Mensajes" de ImGui
    PAG::GUI::getInstancia().aniadirMensaje(PAG::Renderer::getInstancia().consultarCapacidadesOpenGL());
    PAG::GUI::getInstancia().aniadirMensaje("Aplicacion PAG - Practica 2 iniciada");

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        PAG::GUI::getInstancia().dibujarControles();

        PAG::Renderer::getInstancia().refrescar();

        PAG::GUI::getInstancia().renderizar();

        glfwSwapBuffers(window);
    }

    PAG::GUI::getInstancia().aniadirMensaje("Finalizando aplicacion");

    PAG::GUI::getInstancia().liberar();

    glfwDestroyWindow(window);
    window = nullptr;
    glfwTerminate();
}