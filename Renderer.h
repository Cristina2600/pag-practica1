
#ifndef RENDERER_H
#define RENDERER_H
#include <string>

//ahora si vamos a crear el espacio de nombres para las practias 
namespace PAG
{
    //clase renderer, que se ocupa de las funciones de dibujo de opengl usando singleton
    class Renderer
    {
        private:
            static Renderer* instancia;  //principio de singleton 
             float _bgColor[4];  //esto es lo que define el color del fondo, lo normalizaremos para no salir del rango de color
            Renderer ();

        public:
            virtual ~Renderer ();
            static Renderer& getInstancia ();
            void inicializarOpenGL ();
            std::string consultarCapacidadesOpenGL ();
            void refrescar ();
            void redimensionar (int width, int height);
            void cambiarColorFondo (float r, float g, float b);
    };
}

#endif //RENDERER_H