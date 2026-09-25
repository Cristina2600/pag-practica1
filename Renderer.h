
#ifndef RENDERER_H
#define RENDERER_H

//ahora si vamos a crear el espacio de nombres para las practias 
namespace PAG
{
    //clase renderer, que se ocupa de las funciones de dibujo de opengl usando singleton
    class Renderer
    {
        private:
            static Renderer* instancia;  //principio de singleton 

            Renderer ();

        public:
            virtual ~Renderer ();
            static Renderer& getInstancia ();
            void refrescar ();
    };
}

#endif //RENDERER_H