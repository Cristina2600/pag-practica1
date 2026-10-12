#ifndef RENDERER_H
#define RENDERER_H
#include <string>
#include "Listener.h" // va a heredar de listener
#include <memory>
#include "ShaderProgram.h" // aqui estan las funciones para los shaders

#include "Camera.h"
#include "CameraMovement.h"
#include <glm/glm.hpp>


namespace PAG
{
    class Renderer : public Listener  //herencia
    {
        private:
            static Renderer* instancia; //patron singleton, una sola instancia suya
            float _bgColor[4]; //en este caso si tenemos rgba no rgb
            Renderer ();//constructor
            //necesitamos identificadores como numeros porque es lo que devuelve opengl
        
            GLuint idVAO = 0;   // vertex array object(buffers y como leerlos)
            GLuint idVBO = 0;   // vertex buffer object 
            GLuint idIBO = 0;   // index buffer object 

            GLuint idVBOColores = 0;   // VBO de colores distinto del anterior
            //funcion auxiliar para separar la logica de redimension 
            void calcularMatrizAspecto (GLfloat* matriz) const;

            // atributos necesarios para mantener las proporciones del triangulo tras redimensionar
            GLint idUniformAspecto = -1;
            float _aspect = 1024.0f / 576.0f; //estas son las proporciones generales 

            //atributo para forzar un error, si se ponea true no se ve el triangulo y se depura por la ventana de la interfaz
            static const bool FORZAR_ERROR_DEMO = false;

            std::unique_ptr<ShaderProgram> _shaderProgram; //solo una instancia para diferencias los shaders

            //atributos necesarios para la cámara
            Camera _camara;                                     // el Renderer es dueño de la cámara
            CameraMovement _movimiento = CameraMovement::Orbit; // movimiento elegido en la GUI
            GLint idUniformMVP = -1;

        public:
            virtual ~Renderer (); //destructor virtual, lo necesita virtual porque hereda de listener y el si va a tener destructor virtual
            static Renderer& getInstancia (); //referencia a la instancia estática
            void inicializarOpenGL ();
            std::string consultarCapacidadesOpenGL ();
            void refrescar ();
            void redimensionar (int width, int height);
            void cambiarColorFondo (float r, float g, float b);
            void wakeUp (WindowType t, ...) override; // aqui tenemos la funcion que debemos redefinir desde la clase padre 
            void creaShaderProgram (const std::string& nombreBase);  //ahora podrá tener el nombre del fichero  
            void creaModelo();
          
            //funcion extra para asegurarnos de limpiar todo
            void liberarRecursos ();

            void moverCamara (double horizontal, double vertical);
        };
}

#endif //RENDERER_H