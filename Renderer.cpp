
#include <glad/glad.h>

#include "Renderer.h"
#include <sstream>
#include <cstdarg>

namespace PAG
{
    Renderer* Renderer::instancia = nullptr; //debemos declarar la instancia por el atributo estático

    /**
     * Constructor por defecto
     */
    Renderer::Renderer ()
    {
         _bgColor[0] = 0.6f; //gris
        _bgColor[1] = 0.6f;
        _bgColor[2] = 0.6f;
        _bgColor[3] = 1.0f;
     }

    /**
     * Destructor
     */
    Renderer::~Renderer ()
    { }

    /**
     * consulta el unico objeto que hay
     * @return La dirección de memoria del objeto
     */
    Renderer& Renderer::getInstancia ()
    {
        if (!instancia) //si no hay ==nullptr
        {
            instancia = new Renderer; //la crea
        }
        return *instancia; //devuelve la direccion de memoria
    }

  /**
     * Inicializa las opciones de OpenGL que se mantienen constantes
     * durante toda la ejecución de la aplicación
     */
    void Renderer::inicializarOpenGL ()
    {
        glClearColor(_bgColor[0], _bgColor[1], _bgColor[2], _bgColor[3]); //aplicamos el clor de fondo que hemos declarado en el cosntructor
        glEnable(GL_DEPTH_TEST); //activamos opengl
    }


    /**
     * Consulta las capacidades del contexto OpenGL activo
     * @return Cadena con renderer, vendor, version y version de GLSL
     */
    std::string Renderer::consultarCapacidadesOpenGL ()
    {
        std::stringstream ss;
        ss << glGetString(GL_RENDERER) << std::endl
           << glGetString(GL_VENDOR) << std::endl
           << glGetString(GL_VERSION) << std::endl
           << glGetString(GL_SHADING_LANGUAGE_VERSION);
        return ss.str();
    }

    /**
     * Método para hacer el refresco de la escena
     */
    void Renderer::refrescar ()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    /**
     * Ajusta el viewport cuando cambia el tamaño de la ventana
     */
    void Renderer::redimensionar (int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    /**
     * Cambia el color de fondo con el que se limpia el framebuffer
     */
    void Renderer::cambiarColorFondo (float r, float g, float b)
    {
        _bgColor[0] = r;
        _bgColor[1] = g;
        _bgColor[2] = b;
        glClearColor(_bgColor[0], _bgColor[1], _bgColor[2], _bgColor[3]);
    }
    /**
    *Aplicando el patron observador, recibe y procesa todo lo que le mande GUI
    */
void Renderer::wakeUp (WindowType t, ...)
{
    switch (t) //ahora solo hay un case pero luego te permitirá trabajar con mas tipos segun el enum
    {
        case WindowType::Background: //tipo en el enum
        {
            va_list args;
            va_start(args, t); //esto dice, los argumentos variables van despues de t
            float* color = va_arg(args, float*); //ahora va a tomar el siguiente argumento y lo tomará como un float 
            va_end(args); //cierras la lista

            cambiarColorFondo(color[0], color[1], color[2]); //llamas a la funcion para que aplique los cambios 
            break;
        }
    }
}
}