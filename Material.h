#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

namespace PAG
{
    /**
     * @brief Propiedades de material de un modelo: colores ambiente, difuso y
     *        especular, y exponente especular (brillo).
     *        Solo contiene tipos por valor, asi que las operaciones de copia
     *        generadas por el compilador son correctas (regla del cero).
     */
    class Material
    {
        private:
            glm::vec3 _ambiente  = glm::vec3(0.2f);
            glm::vec3 _difuso    = glm::vec3(0.8f);
            glm::vec3 _especular = glm::vec3(1.0f);
            float _exponente     = 32.0f;

        public:
            Material () = default;
            Material (const glm::vec3& ambiente, const glm::vec3& difuso,
                      const glm::vec3& especular, float exponente)
                : _ambiente(ambiente), _difuso(difuso),
                  _especular(especular), _exponente(exponente) {}

            const glm::vec3& getAmbiente () const  { return _ambiente; }
            const glm::vec3& getDifuso () const    { return _difuso; }
            const glm::vec3& getEspecular () const { return _especular; }
            float getExponente () const            { return _exponente; }

            void setAmbiente (const glm::vec3& c)  { _ambiente = c; }
            void setDifuso (const glm::vec3& c)    { _difuso = c; }
            void setEspecular (const glm::vec3& c) { _especular = c; }
            void setExponente (float e)            { _exponente = e; }
    };
}

#endif //MATERIAL_H