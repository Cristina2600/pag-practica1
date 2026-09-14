#include <iostream>
//primero tiene que ir el include de glad
#include <glad/glad.h>
#include <GLFW/glfw3.h>


//TRABAJO AUTONOMO
//laas variables globales son aquellas que necesitamos recordar entre distintas llamadas
float bgColor[4]= {0.6f, 0.6f, 0.6f, 1.0f};//esto guarda el color del fondo que lo guardamos gris
//creamos un nuevo callback para cuando hagamos scroll con el raton
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    //vamos a modificar el color dependiendo del movimiento de la rueda por eso necesitamos el offset
    //siendo en el eje y, +1 si la movemos hacia arriba o -1 si la hacemos hacia abajo
    //vamos a multiplicar por un valor pequeño porque si no el cambio se hace de golpe
     bgColor[0] += static_cast<float>(yoffset) * 0.05f;
     //ahora tenemos que asegurar que el color siempre sea válido porque se nos puede salir del rango por que vamos a normalizarlo
     if (bgColor[0] > 1.0f) bgColor[0] = 1.0f;
    if (bgColor[0] < 0.0f) bgColor[0] = 0.0f;
    //ahora aplicamos el nuevo color
    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
    std::cout << "Movida la rueda: " << xoffset << " horiz, "
              << yoffset << " vert. Nuevo R = " << bgColor[0] << std::endl;
}
//  Esta función callback será llamada cuando GLFW produzca algún error 
void error_callback ( int errno, const char* desc ) 
{  std::string aux (desc); 
std::cout << "Error de GLFW número " << errno << ": " << aux << std::endl; 
} 
// - Esta función callback será llamada cada vez que el área de dibujo 
// OpenGL deba ser redibujada. 
void window_refresh_callback ( GLFWwindow *window ) 
{  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
// - GLFW usa un doble buffer para que no haya parpadeo. Esta orden 
// intercambia el buffer back (que se ha estado dibujando) por el 
// que se mostraba hasta ahora front. Debe ser la última orden de 
// este callback 
glfwSwapBuffers ( window ); 
std::cout << "Refresh callback called" << std::endl; 

}
 
// - Esta función callback será llamada cada vez que se cambie el tamaño 
// del área de dibujo OpenGL. 
void framebuffer_size_callback( GLFWwindow *window, int width, int height )
{  glViewport ( 0, 0, width, height ); 
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
 
// - Esta función callback será llamada cada vez que se pulse algún botón 
// del ratón sobre el área de dibujo OpenGL. 
void mouse_button_callback ( GLFWwindow *window, int button, int action, int mods ) 
{  if ( action == GLFW_PRESS ) 
   {  std::cout << "Pulsado el botón: " << button << std::endl; 
   } 
   else if ( action == GLFW_RELEASE ) 
   {  std::cout << "Soltado el botón: " << button << std::endl; 
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
    //mostramos toda la informacion, el modelo de mi tarjeta, el fabricante, la version del driver y la version para el lenguaje de los shaders
    std::cout << glGetString(GL_RENDERER) << std::endl
            << glGetString(GL_VENDOR) << std::endl
            << glGetString(GL_VERSION) << std::endl
            << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

      // - Registramos los callbacks que responderán a los eventos principales 
   glfwSetWindowRefreshCallback ( window, window_refresh_callback ); 
   glfwSetFramebufferSizeCallback ( window, framebuffer_size_callback ); 
   glfwSetKeyCallback ( window, key_callback ); 
   glfwSetMouseButtonCallback ( window, mouse_button_callback ); 
   glfwSetScrollCallback ( window, scroll_callback ); 

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