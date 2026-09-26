#ifndef RENDERER_H
#define RENDERER_H
#include <string>
#include "Listener.h"

namespace PAG
{
    class Renderer : public Listener   // <-- AÑADIR ": public Listener"
    {
        private:
            static Renderer* instancia;
            float _bgColor[4];
            Renderer ();

        public:
            virtual ~Renderer ();
            static Renderer& getInstancia ();
            void inicializarOpenGL ();
            std::string consultarCapacidadesOpenGL ();
            void refrescar ();
            void redimensionar (int width, int height);
            void cambiarColorFondo (float r, float g, float b);
            void wakeUp (WindowType t, ...) override;
    };
}

#endif //RENDERER_H