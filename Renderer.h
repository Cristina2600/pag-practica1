#ifndef RENDERER_H
#define RENDERER_H
#include <string>
#include "Listener.h" // va a heredar de listener
#include <memory>
#include "ShaderProgram.h" // aqui estan las funciones para los shaders
#include "Model.h"
#include "Camera.h"
#include "CameraMovement.h"
#include <glm/glm.hpp>
#include <vector>


namespace PAG
{
    class Renderer : public Listener  //herencia
    {
        private:
            static Renderer* instancia; //patron singleton, una sola instancia suya
            float _bgColor[4]; //en este caso si tenemos rgba no rgb
            Renderer ();//constructor
            //necesitamos identificadores como numeros porque es lo que devuelve opengl

            std::unique_ptr<ShaderProgram> _shaderProgram; //solo una instancia para diferencias los shaders

            //ahora cargamos todos los modelos
            std::vector<std::unique_ptr<Model>> _modelos;
            int _huecosUsados=0; //distintos modelos sin que se solapen
            //atributos necesarios para la cámara
            Camera _camara;                                     // el Renderer es dueño de la cámara
            CameraMovement _movimiento = CameraMovement::Orbit; // movimiento elegido en la GUI
            GLint idUniformMVP = -1;

            glm::vec3 siguientePosicion ();
            Model& modelo (int indice);
            const Model& modelo (int indice) const;
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
            void creaModelo(); //este es el triangulo de las prácticas anteriores

            void cargaModelo (const std::string& ruta);  // OBJ con Assimp
            void eliminaModelo (int indice);
            void transformaModelo (int indice, const Transformacion& t);
          
            //funcion extra para asegurarnos de limpiar todo
            void liberarRecursos ();

            void moverCamara (double horizontal, double vertical);

            // Consultas de solo lectura (para que las ventanas de la interfaz se dibujen)
            int numModelos () const;
            std::string nombreModelo (int indice) const;
            Transformacion transformacionModelo (int indice) const;

            //método para mantener el triangulo 
            void creaTriangulo();
        };
}

#endif //RENDERER_H