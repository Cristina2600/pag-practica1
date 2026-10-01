#ifndef RENDERER_H
#define RENDERER_H
#include <string>
#include "Listener.h" // va a heredar de listener

namespace PAG
{
    class Renderer : public Listener  //herencia
    {
        private:
            static Renderer* instancia; //patron singleton, una sola instancia suya
            float _bgColor[4]; //en este caso si tenemos rgba no rgb
            Renderer ();//constructor
            GLuint idVS = 0;    // Identificador del vertex shader 
            GLuint idFS = 0;    // Identificador del fragment shader 
            GLuint idSP = 0;    // Identificador del shader program 
            GLuint idVAO = 0;   // Identificador del vertex array object 
            GLuint idVBO = 0;   // Identificador del vertex buffer object 
            GLuint idIBO = 0;   // Identificador del index buffer object 

        public:
            virtual ~Renderer (); //destructor virtual, lo necesita virtual porque hereda de listener y el si va a tener destructor virtual
            static Renderer& getInstancia (); //referencia a la instancia estática
            void inicializarOpenGL ();
            std::string consultarCapacidadesOpenGL ();
            void refrescar ();
            void redimensionar (int width, int height);
            void cambiarColorFondo (float r, float g, float b);
            void wakeUp (WindowType t, ...) override; // aqui tenemos la funcion que debemos redefinir desde la clase padre 
            void creaShaderProgram();
            void creaModelo();
            //vamos a crear las funciones necesarias para controlar los errores
            GLuint compilarShader (GLenum tipo, const std::string& fuente, const std::string& etiqueta);
        };
}

#endif //RENDERER_H