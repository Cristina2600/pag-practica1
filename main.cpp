#include <iostream>
//primero tiene que ir el include de glad
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//vamos a incluir renderer para las llamadas necesarias
#include "Renderer.h"
//Todo lo necesario para imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

//TRABAJO AUTONOMO
//laas variables globales son aquellas que necesitamos recordar entre distintas llamadas
float bgColor[4]= {0.6f, 0.6f, 0.6f, 1.0f};//esto guarda el color del fondo que lo guardamos gris
//creamos un nuevo callback para cuando hagamos scroll con el raton
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    // La lógica de "cuánto cambia el color" pasa a la interfaz,
    // el cambio estará Renderer.
    static float r = 0.6f;
    r += static_cast<float>(yoffset) * 0.05f;
    if (r > 1.0f) r = 1.0f;
    if (r < 0.0f) r = 0.0f;

     PAG::Renderer::getInstancia().cambiarColorFondo(r, 0.6f, 0.6f);  
    PAG::Renderer::getInstancia().refrescar();
    glfwSwapBuffers(window);


    std::cout << "Movida la rueda: " << xoffset << " horiz, "
              << yoffset << " vert" << std::endl;   
}
//  Esta función callback será llamada cuando GLFW produzca algún error 
void error_callback ( int errno, const char* desc ) 
{ 
    std::string aux (desc); 
    std::cout << "Error de GLFW número " << errno << ": " << aux << std::endl; 
} 
// - Esta función callback será llamada cada vez que el área de dibujo 
// OpenGL deba ser redibujada. 
void window_refresh_callback ( GLFWwindow *ventana ) 
{ 
        PAG::Renderer::getInstancia().refrescar();
        glfwSwapBuffers(ventana);
        std::cout << "Finaliza el callback de refresco" << std::endl;

}
 
// - Esta función callback será llamada cada vez que se cambie el tamaño 
// del área de dibujo OpenGL. 
void framebuffer_size_callback( GLFWwindow *window, int width, int height )
{  
    PAG::Renderer::getInstancia().redimensionar(width, height);
    std::cout << "Resize callback called" << std::endl; 
} 
 
// - Esta función callback será llamada cada vez que se pulse una tecla 
// dirigida al área de dibujo OpenGL. 
void key_callback ( GLFWwindow *window, int key, int scancode, int action, int mods ) 
{  if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS ) 
   {  glfwSetWindowShouldClose(window, GLFW_TRUE); 
   } 
   std::cout << "Key callback called" << std::endl; 
} 
 
//usaremos imgui para notificar todos los eventos de taton 
void mouse_button_callback ( GLFWwindow *window, int button, int action, int mods ) 
{ 
    ImGuiIO& io = ImGui::GetIO();
    if (action == GLFW_PRESS)
    {
        io.AddMouseButtonEvent(button, true);
    }
    else if (action == GLFW_RELEASE)
    {
        io.AddMouseButtonEvent(button, false);
    }
} 


int main()
{
    std::cout << "PAG: practica 1 " << std::endl;

     // - Este callback hay que registrarlo ANTES de llamar a glfwInit 
   glfwSetErrorCallback ( (GLFWerrorfun) error_callback );

    //ahora hay que iniciar anted glfw
    if(glfwInit()!= GLFW_TRUE)
    {
        std::cout<<"Fallo al iniciar glfw" << std::endl;
        return -1;
    }

    //vamos a crear el contexto de opengl para cuando GLFW cree la ventana principal
    glfwWindowHint(GLFW_SAMPLES, 4); //usamos 4 muestras por pixel para el antialiasing
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //esto lo usamos para trabajar con los shaders
    //versiones y subversiones de opengl
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow *window;//esto conformará la pantalla en sí que vemos
    window = glfwCreateWindow(1024, 576, "Pantalla practica 1 PAG", nullptr, nullptr);
    //definimos el ancho en pixeles, alto en pixeles, un titulo para la ventana
    //el cuarto parámetro es el modo de la pantalla, al pasarle null le decimos que sea flotante
    //el ultimo parámetro define si la ventana comparte parámetros con otra ventana, en este caso al ser null es una ventana independiente
    if (window == nullptr)//da error al crearlo
    {
        std::cout<<"Error al crear la ventana" << std::endl;
        glfwTerminate();
        return -2;

    }

    glfwMakeContextCurrent(window);

    //ahora vamos a inicializar GLAD, al reves que en los includes ya que necesita el contexto
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout<<"inicializacion fallida de GLAD" << std::endl;
        glfwDestroyWindow(window);
        window=nullptr;
        glfwTerminate();
        return -3;
    }
    // Pasamos a pedirle todo a Renderer, que es quien conoce OpenGL, y solo
    // imprimimos el resultado (eso si es responsabilidad de main.cpp).
    std::cout << PAG::Renderer::getInstancia().consultarCapacidadesOpenGL() << std::endl;


      // - Registramos los callbacks que responderán a los eventos principales 
   glfwSetWindowRefreshCallback ( window, window_refresh_callback ); 
   glfwSetFramebufferSizeCallback ( window, framebuffer_size_callback ); 
   glfwSetKeyCallback ( window, key_callback ); 
   glfwSetMouseButtonCallback ( window, mouse_button_callback ); 
   glfwSetScrollCallback ( window, scroll_callback ); 

    // Será renderer quien trabaje ocn el buffer de color
    PAG::Renderer::getInstancia().inicializarOpenGL();

        //iniciamos imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    //ahora vamos a definir el bucle principal hasta que se cierre
     while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        // imgui creará el nuevo frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        // Dibujado de la escena OpenGL (delegado en Renderer)
        PAG::Renderer::getInstancia().refrescar();

        // la interfaz de imgui va encima de la escena
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    //limpieza de recursos
    std::cout << "Fin de la practica 2" << std::endl;
       //liberamos recursos imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    window = nullptr;
    glfwTerminate();



}