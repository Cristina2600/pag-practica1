#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace PAG
{
    /**
     * @brief Aqui vamos a cargar, compilar y enlazar los shaders el vertex y fragment para separar lógica y reutilizarla
     */
    class ShaderProgram
    {
        private:
            GLuint idVS = 0; //vertex shader
            GLuint idFS = 0; //fragment shader
            GLuint idSP = 0;

            // Cache nombre -> ubicacion del uniform, para no preguntar a OpenGL en cada envio
            mutable std::unordered_map<std::string, GLint> _ubicaciones;
            GLint ubicacion (const std::string& nombre) const;

            std::string leerFichero (const std::string& ruta);
            GLuint compilarShader (GLenum tipo, const std::string& fuente,
                                   const std::string& etiqueta);

        public:
            ShaderProgram () = default;
            ~ShaderProgram ();

            ShaderProgram (const ShaderProgram&) = delete;
            ShaderProgram& operator= (const ShaderProgram&) = delete;

            /**
             * Carga, compila y enlaza vs/fs a partir de nombreBase + "-vs.glsl"/"-fs.glsl"
             * @throw std::runtime_error si falla cualquier paso
             */
            void cargar (const std::string& nombreBase);

            /// Activa este shader program (glUseProgram)
            void activar () const;

            /// Ubicacion de un uniform dentro de este programa
            GLint getUniformLocation (const std::string& nombre) const;

            /// Envian un uniform al programa. El programa debe estar activo (glUseProgram).
            /// Si el shader no tiene ese uniform, OpenGL lo ignora sin error.
            void setUniform (const std::string& nombre, float v) const;
            void setUniform (const std::string& nombre, const glm::vec3& v) const;
            void setUniform (const std::string& nombre, const glm::mat3& m) const;
            void setUniform (const std::string& nombre, const glm::mat4& m) const;

            //pasando el nombre de la subrutina, activamoms su implementacion
            bool activarSubrutina (GLenum tipoShader, const std::string& nombre) const;

            bool estaCargado () const;

            /// Libera los recursos OpenGL (shaders y programa)
            void liberar ();
    };
}

#endif //SHADERPROGRAM_H