#include <iostream>
//primero tiene que ir el include de glad
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main()
{
    std::cout << "PAG: practica 1 " << std::endl;

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
    //mostramos toda la informacion, el modelo de mi tarjeta, el fabricante, la version del driver y la version para el lenguaje de los shaders
    std::cout << glGetString(GL_RENDERER) << std::endl
            << glGetString(GL_VENDOR) << std::endl
            << glGetString(GL_VERSION) << std::endl
            << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;


    //Vamos a definir el color del fondo primero 
    glClearColor(0.6, 0.6, 0.6, 1.0);
    //aqui definimos el eje z para la profundidad
    glEnable(GL_DEPTH_TEST);
    //ahora vamos a definir el bucle principal hasta que se cierre
     while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //primero borramos lo que se encuentre en pantalla y la profundidad
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //limpieza de recursos
    std::cout << "Fin de la practica 1" << std::endl;
    glfwDestroyWindow(window);
    window = nullptr;
    glfwTerminate();



}