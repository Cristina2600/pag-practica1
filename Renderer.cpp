
#include <glad/glad.h>

#include "Renderer.h"

namespace PAG
{
    Renderer* Renderer::instancia = nullptr;

    /**
     * Constructor por defecto
     */
    Renderer::Renderer ()
    { }

    /**
     * Destructor
     */
    Renderer::~Renderer ()
    { }

    /**
     * Consulta del objeto único de la clase
     * @return La dirección de memoria del objeto
     */
    Renderer& Renderer::getInstancia ()
    {
        if (!instancia)
        {
            instancia = new Renderer;
        }
        return *instancia;
    }

    /**
     * Método para hacer el refresco de la escena
     */
    void Renderer::refrescar ()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}