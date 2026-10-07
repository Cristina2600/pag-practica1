#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <string>
#include <glad/glad.h>

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

            bool estaCargado () const;

            /// Libera los recursos OpenGL (shaders y programa)
            void liberar ();
    };
}

#endif //SHADERPROGRAM_H