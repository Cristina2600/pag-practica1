#ifndef RENDERER_H
#define RENDERER_H
#include <string>
#include "Listener.h" // va a heredar de listener
#include <memory>
#include "ShaderProgram.h" // aqui estan las funciones para los shaders
#include "Model.h"
#include "Camera.h"
#include "CameraMovement.h"
#include "ModeloVisualizacion.h"
#include <glm/glm.hpp>
#include <vector>
#include "Luz.h"


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
            //atributos necesarios para la cámara
            Camera _camara;                                     // el Renderer es dueño de la cámara
            CameraMovement _movimiento = CameraMovement::Orbit; // movimiento elegido en la GUI
            GLint idUniformMVP = -1;
            //para definir el valor de color difuso
             GLint idUniformColorDifuso = -1;                       
            ModoVisualizacion _modo = ModoVisualizacion::Solido;   

            //color del alambre
            GLint idUniformColorAlambre = -1; //siempre empieza en menos 1
            glm::vec3 _colorAlambre = glm::vec3(1.0f, 0.0f, 0.0f);   // rojo por defecto

            int primerHuecoLibre () const;
            void colocarYAnadir (std::unique_ptr<Model> m);
            Model& modelo (int indice);
            const Model& modelo (int indice) const;
            //organizacion de los modelos en la pantalla
            std::vector<int> _huecoModelo;   // hueco de la cuadricula que ocupa cada modelo (paralelo a _modelos)

            //parámetros para la definicion de la luz

            std::vector<Luz> _luces;               // las luces de la escena

            Luz& luz (int indice);
            const Luz& luz (int indice) const;

            // Una pasada de iluminacion se reparte en dos pasos: configurar la luz y dibujar los modelos
            void aplicaLuz (const Luz& l, const glm::mat4& vista);
            void dibujaModelos (const glm::mat4& vista, const glm::mat4& vp);


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
            //como tenemos la clase material que se define a si misma y el modelo lleva un material por si mismo
            //aqui lo que hacemos es darle otros materiales al modelo
            void asignaMaterial (int indice, const Material& m);
            Material materialModelo (int indice) const;
            //necesitamos metodos para responder ante el cambio de alambre y solido
            void cambiaModoVisualizacion (ModoVisualizacion m);   
            ModoVisualizacion modoVisualizacion () const;  
            
            //métodos para cambiar los colores del alambre
            void cambiaColorAlambre (const glm::vec3& c);
            const glm::vec3& colorAlambre () const;

            //metodos para las definiciones de las luces

            void anadeLuz (const Luz& l);
            void eliminaLuz (int indice);
            void modificaLuz (int indice, const Luz& l);
            int numLuces () const;
            Luz consultaLuz (int indice) const;
            void creaLucesPorDefecto ();   // ambiente + una puntual a cada lado
        };
}

#endif //RENDERER_H