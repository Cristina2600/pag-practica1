#include <fstream>
#include <sstream>
#include <stdexcept>

#include "ShaderProgram.h"

namespace PAG
{
    ShaderProgram::~ShaderProgram ()
    {
        liberar();
    }

    std::string ShaderProgram::leerFichero (const std::string& ruta)
    {
        std::ifstream fichero (ruta);
        if (!fichero.is_open())
        {
            throw std::runtime_error("No se pudo abrir el fichero: " + ruta);
        }
        std::stringstream ss;
        ss << fichero.rdbuf();
        return ss.str();
    }

    GLuint ShaderProgram::compilarShader (GLenum tipo, const std::string& fuente,
                                          const std::string& etiqueta)
    {
        GLuint id = glCreateShader(tipo);
        const GLchar* texto = fuente.c_str();
        glShaderSource(id, 1, &texto, nullptr);
        glCompileShader(id);

        GLint resultado = GL_FALSE;
        glGetShaderiv(id, GL_COMPILE_STATUS, &resultado);
        if (resultado == GL_FALSE)
        {
            GLint longitud = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &longitud);
            std::string log(static_cast<size_t>(longitud), '\0');
            glGetShaderInfoLog(id, longitud, nullptr, &log[0]);

            glDeleteShader(id);
            throw std::runtime_error("Error al compilar el " + etiqueta + ":\n" + log);
        }
        return id;
    }

    void ShaderProgram::cargar (const std::string& nombreBase)
    {
        // Si ya habia un programa cargado (el usuario pulsa "Load" dos veces),
        // liberamos el anterior antes de crear uno nuevo
        liberar();

        std::string fuenteVS = leerFichero(nombreBase + "-vs.glsl");
        GLuint vs = compilarShader(GL_VERTEX_SHADER, fuenteVS, "vertex shader");

        GLuint fs = 0;
        try
        {
            std::string fuenteFS = leerFichero(nombreBase + "-fs.glsl");
            fs = compilarShader(GL_FRAGMENT_SHADER, fuenteFS, "fragment shader");
        }
        catch (...)
        {
            glDeleteShader(vs);
            throw;
        }

        GLuint sp = glCreateProgram();
        glAttachShader(sp, vs);
        glAttachShader(sp, fs);
        glLinkProgram(sp);

        GLint resultado = GL_FALSE;
        glGetProgramiv(sp, GL_LINK_STATUS, &resultado);
        if (resultado == GL_FALSE)
        {
            GLint longitud = 0;
            glGetProgramiv(sp, GL_INFO_LOG_LENGTH, &longitud);
            std::string log(static_cast<size_t>(longitud), '\0');
            glGetProgramInfoLog(sp, longitud, nullptr, &log[0]);

            glDeleteProgram(sp);
            glDeleteShader(vs);
            glDeleteShader(fs);
            throw std::runtime_error("Error al enlazar el shader program:\n" + log);
        }

        idVS = vs;
        idFS = fs;
        idSP = sp;
    }

    void ShaderProgram::activar () const
    {
        glUseProgram(idSP);
    }

    GLint ShaderProgram::getUniformLocation (const std::string& nombre) const
    {
        return glGetUniformLocation(idSP, nombre.c_str());
    }

    bool ShaderProgram::estaCargado () const
    {
        return idSP != 0;
    }

    void ShaderProgram::liberar ()
    {
        if (idVS != 0) { glDeleteShader(idVS);  idVS = 0; }
        if (idFS != 0) { glDeleteShader(idFS);  idFS = 0; }
        if (idSP != 0) { glDeleteProgram(idSP); idSP = 0; }
    }
}