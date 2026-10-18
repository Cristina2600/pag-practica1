//clase que encapsula la geometria de UN modelo: su VAO, sus buffers y su transformacion de modelado
//clase para manejar los modelos, su geometria, vao, buffers y transformaciones necesarias

#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace PAG
{
    /**
     * @brief todas las transformacion de modelado 
     *        La rotacion se expresa en GRADOS (uno por eje).
     */
    struct Transformacion
    {
        glm::vec3 posicion = glm::vec3(0.0f); //traslacion
        glm::vec3 rotacion = glm::vec3(0.0f); //rotacion
        glm::vec3 escala   = glm::vec3(1.0f); //escalado
    };

    /**
     * @brief Un modelo 3D necesita su geometria formada por VAO, VBO e IBO y su matriz de modelado.
     *
     */
    class Model
    {
        private:
            GLuint idVAO = 0;       // similar al manual de instrucciones que buffers usar y como leerlos
            GLuint idVBOPos = 0;    // datos: posiciones (location 0)
            GLuint idVBOColor = 0;  // datos: colores (location 1)
            GLuint idIBO = 0;       // indices de los triangulos
            GLsizei _numIndices = 0;

            std::string _nombre = "Modelo";
            Transformacion _transformacion;
            glm::mat4 _modelado = glm::mat4(1.0f);
            //esto es para dibujar en la malla 
            bool _alambre = false;

            void crearBuffers (const std::vector<GLfloat>& posiciones,
                               const std::vector<GLfloat>& colores,
                               const std::vector<GLuint>& indices); //estos son los buffers que definen toda la geometria
            void actualizarMatrizModelado (); //funcion auxiliar en privado

        public:
            Model () = default;
            ~Model ();

            // Posee recursos de la GPU: copiar daria dos objetos con los mismos identificadores
            Model (const Model&) = delete;
            Model& operator= (const Model&) = delete;

            /// Crea el triangulo con gradiente de la práctica anterior
            void crearTriangulo ();

            /**
             * Lee la ruta del fichero .obj para poder crear nuestro modelo 
             * @throw std::runtime_error si no se puede leer o no tiene triangulos
             */
            void cargarOBJ (const std::string& ruta);

            void dibujar () const;
            void liberar ();

            const glm::mat4& getMatrizModelado () const;
            const Transformacion& getTransformacion () const;
            void setTransformacion (const Transformacion& t);

            const std::string& getNombre () const;
            void setNombre (const std::string& nombre);
    };
}

#endif //MODEL_H