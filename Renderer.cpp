
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
    {
         if ( idVS != 0 ) 
   {  glDeleteShader ( idVS ); 
   } 
 
   if ( idFS != 0 ) 
   {  glDeleteShader ( idFS ); 
   } 
 
   if ( idSP != 0 ) 
   {  glDeleteProgram ( idSP ); 
   } 
 
   if ( idVBO != 0 ) 
   {  glDeleteBuffers ( 1, &idVBO ); 
   } 
 
   if ( idIBO != 0 ) 
   {  glDeleteBuffers ( 1, &idIBO ); 
   } 
 
   if ( idVAO != 0 ) 
   {  glDeleteVertexArrays ( 1, &idVAO ); 
   } 
     }

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
        glEnable ( GL_MULTISAMPLE ); //
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
        glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
        glUseProgram ( idSP ); 
        glBindVertexArray ( idVAO ); 
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, idIBO ); 
        glDrawElements ( GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr );
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

//esto solo aplica color al fragmento sin ningun cálculo de la geometría
/** 
 * Método para crear, compilar y enlazar el shader program 
 * @note No se incluye ninguna comprobación de errores 
 */ 
void PAG::Renderer::creaShaderProgram( ) 
{  std::string miVertexShader = 
   "#version 410\n" 
   "layout (location = 0) in vec3 posicion;\n" 
   "void main ()\n" 
   "{  gl_Position = vec4 ( posicion, 1 );\n" 
   "}\n"; 
 
   std::string miFragmentShader = 
   "#version 410\n" 
   "out vec4 colorFragmento;\n" 
   "void main ()\n" 
   "{  colorFragmento = vec4 ( 1.0, .4, .2, 1.0 );\n" 
   "}\n"; 
 
   idVS = glCreateShader ( GL_VERTEX_SHADER ); 
   const GLchar* fuenteVS = miVertexShader.c_str (); 
   glShaderSource ( idVS, 1, &fuenteVS, nullptr ); 
   glCompileShader ( idVS ); 
 
   idFS = glCreateShader ( GL_FRAGMENT_SHADER ); 
   const GLchar* fuenteFS = miFragmentShader.c_str (); 
   glShaderSource ( idFS, 1, &fuenteFS, nullptr ); 
   glCompileShader ( idFS ); 
 
   idSP = glCreateProgram (); 
   glAttachShader ( idSP, idVS ); 
   glAttachShader ( idSP, idFS ); 
   glLinkProgram ( idSP ); 
} 
//Ahora vamos a almacenar vertices e indices
/** 
 * Método para crear el VAO para el modelo a renderizar 
 * @note No se incluye ninguna comprobación de errores 
 */ 
void PAG::Renderer::creaModelo ( ) 
{  GLfloat vertices[] = { -.5, -.5, 0, //vertice 1
                           .5, -.5, 0,  //vertice 2
                           .0,  .5, 0 }; //vertice2
   GLuint indices[] = { 0, 1, 2 }; 
 
   glGenVertexArrays ( 1, &idVAO ); 
   glBindVertexArray ( idVAO ); 
    glGenBuffers ( 1, &idVBO ); 
   glBindBuffer ( GL_ARRAY_BUFFER, idVBO ); 
   glBufferData ( GL_ARRAY_BUFFER, 9*sizeof(GLfloat), vertices, GL_STATIC_DRAW ); 
   glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), nullptr ); 
   glEnableVertexAttribArray ( 0 ); 
   glGenBuffers ( 1, &idIBO ); 
   glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, idIBO ); 
   glBufferData ( GL_ELEMENT_ARRAY_BUFFER, 3*sizeof(GLuint), indices, GL_STATIC_DRAW ); 
} 