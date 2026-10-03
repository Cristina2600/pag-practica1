
#include <glad/glad.h>

#include "Renderer.h"
#include <sstream>
#include <cstdarg>
//include necesario para manejar los errores 
#include <stdexcept>
#include <fstream>

namespace PAG
{
    Renderer* Renderer::instancia = nullptr; //debemos declarar la instancia por el atributo estático

    /**
     * Constructor por defecto
     */
    Renderer::Renderer ()
    {
         _bgColor[0] = 0.6f; //gris
        _bgColor[1] = 0.6f;
        _bgColor[2] = 0.6f;
        _bgColor[3] = 1.0f;
     }

    /**
     * Destructor
     */
Renderer::~Renderer ()
{
    liberarRecursos();
}

void Renderer::liberarRecursos ()
{
    if (idVS != 0)  { glDeleteShader(idVS);          idVS = 0; }
    if (idFS != 0)  { glDeleteShader(idFS);          idFS = 0; }
    if (idSP != 0)  { glDeleteProgram(idSP);         idSP = 0; }
    if (idVBO != 0) { glDeleteBuffers(1, &idVBO);    idVBO = 0; }
    if (idIBO != 0) { glDeleteBuffers(1, &idIBO);    idIBO = 0; }
    if (idVAO != 0) { glDeleteVertexArrays(1, &idVAO); idVAO = 0; }
    if (idVBOColores != 0) { glDeleteBuffers(1, &idVBOColores); idVBOColores = 0; }
}

    /**
     * consulta el unico objeto que hay
     * @return La dirección de memoria del objeto
     */
    Renderer& Renderer::getInstancia ()
    {
        if (!instancia) //si no hay ==nullptr
        {
            instancia = new Renderer; //la crea
        }
        return *instancia; //devuelve la direccion de memoria
    }

  /**
     * Inicializa las opciones de OpenGL que se mantienen constantes
     * durante toda la ejecución de la aplicación
     */
    void Renderer::inicializarOpenGL ()
    {
        glClearColor(_bgColor[0], _bgColor[1], _bgColor[2], _bgColor[3]); //aplicamos el clor de fondo que hemos declarado en el cosntructor
        glEnable(GL_DEPTH_TEST); //activamos opengl
        glEnable ( GL_MULTISAMPLE ); //
    }


    /**
     * Consulta las capacidades del contexto OpenGL activo
     * @return Cadena con renderer, vendor, version y version de GLSL
     */
    std::string Renderer::consultarCapacidadesOpenGL ()
    {
        std::stringstream ss;
        ss << glGetString(GL_RENDERER) << std::endl
           << glGetString(GL_VENDOR) << std::endl
           << glGetString(GL_VERSION) << std::endl
           << glGetString(GL_SHADING_LANGUAGE_VERSION);
        return ss.str();
    }

    /**
     * Método para hacer el refresco de la escena
     */
    void Renderer::refrescar ()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (idSP == 0 || idVAO == 0)   // si se falla al cargar shaders, no hay nada que dibujar 
        {
            return; //si se lanza la excepcion no llegaremos a crear el modelo
        }
        //si al crear los shaders hay algun error, creaModelo no se ejecutará por lo que idsp, idvao y idibo se quedarán sin valor
        //aunque tras las pruebas al asignarle un valor no generan un problema mayor que detenga el problema 
        //Se queda congelado opengl ya que no tendrá geometría que dibujar
        glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
        glUseProgram ( idSP ); 

        // Matriz de escalado 4x4, apta para opengl
        GLfloat matriz[16];
        //llamamos a la funcion auxiliar de calculo
        calcularMatrizAspecto(matriz);
        glUniformMatrix4fv(idUniformAspecto, 1, GL_FALSE, matriz); 

        glBindVertexArray ( idVAO ); 
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, idIBO ); 
        glDrawElements ( GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr );
    }
    /**
     * Ajusta el viewport cuando cambia el tamaño de la ventana
     * @note Ahora hemos añadido una proporcion del aspecto para que aunque cambie la pantalla siemore se mantenga en valores proporcionales  
     */
    void Renderer::redimensionar (int width, int height)
    {
         glViewport(0, 0, width, height);

        if (height != 0) //evitar dividir entre 0
        {
            _aspect = static_cast<float>(width) / static_cast<float>(height); //aspect se aplicará en el siguiente refrescar
        }
    }

    /**
     * Cambia el color de fondo con el que se limpia el framebuffer
     */
    void Renderer::cambiarColorFondo (float r, float g, float b)
    {
        _bgColor[0] = r;
        _bgColor[1] = g;
        _bgColor[2] = b;
        glClearColor(_bgColor[0], _bgColor[1], _bgColor[2], _bgColor[3]);
    }
    /**
    *Aplicando el patron observador, recibe y procesa todo lo que le mande GUI
    */
void Renderer::wakeUp (WindowType t, ...)
{
    switch (t) //ahora solo hay un case pero luego te permitirá trabajar con mas tipos segun el enum
    {
        case WindowType::Background: //tipo en el enum
        {
            va_list args;
            va_start(args, t); //esto dice, los argumentos variables van despues de t
            float* color = va_arg(args, float*); //ahora va a tomar el siguiente argumento y lo tomará como un float 
            va_end(args); //cierras la lista

            cambiarColorFondo(color[0], color[1], color[2]); //llamas a la funcion para que aplique los cambios 
            break;
        }
    }
}


//Ahora vamos a almacenar vertices e indices, esto trabaja con ls CPU
/** 
 * Método para crear el VAO para el modelo a renderizar 
 * @note No se incluye ninguna comprobación de errores 
 * @note ahora tendremos las dos versiones inplementadas
 */ 
void Renderer::creaModelo ()
{
    GLuint indices[] = { 0, 1, 2 }; //vertices que se definen

    glGenVertexArrays(1, &idVAO);
    glBindVertexArray(idVAO);

    // VBOs NO entrelazados (un VBO por atributo)
    //simplementar definimos la posición de cada vertice y su color exacto
    GLfloat posiciones[] = { -.5f, -.5f, 0.0f,
                              .5f, -.5f, 0.0f,
                              .0f,  .5f, 0.0f };
    GLfloat colores[]    = { 1.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 1.0f };

    glGenBuffers(1, &idVBO);
    glBindBuffer(GL_ARRAY_BUFFER, idVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posiciones), posiciones, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &idVBOColores);
    glBindBuffer(GL_ARRAY_BUFFER, idVBOColores);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colores), colores, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);

    /* UN SOLO VBO ENTRELAZADO
       Para probarla: comenta el bloque de la VERSION A de arriba y descomenta esto.

    GLfloat interleaved[] = { -.5f, -.5f, 0.0f,   1.0f, 0.0f, 0.0f,
                               .5f, -.5f, 0.0f,   0.0f, 1.0f, 0.0f,
                               .0f,  .5f, 0.0f,   0.0f, 0.0f, 1.0f };
    //Como se ve solo hay un buffer saltando de 6 en 6 para leer el vertice completo
    //el color se define tras los tres floats de posicion
    glGenBuffers(1, &idVBO);
    glBindBuffer(GL_ARRAY_BUFFER, idVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(interleaved), interleaved, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    */

    glGenBuffers(1, &idIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(GLuint), indices, GL_STATIC_DRAW);
} 
/**
 * Compila un shader y comprueba el resultado
 * @param tipo      GL_VERTEX_SHADER o GL_FRAGMENT_SHADER para saber con que
 * @param fuente    codigo GLSL
 * @param mensaje  nombre para el mensaje de error ("vertex shader"...)
 * @throw std::runtime_error si no compila
 * @note Aqui vamos a manejar las excepciones
 */
GLuint Renderer::compilarShader (GLenum tipo, const std::string& fuente,
                                 const std::string& mensaje)
{
    GLuint id = glCreateShader(tipo); //vertex o fragment
    const GLchar* texto = fuente.c_str(); //mensaje de error
    glShaderSource(id, 1, &texto, nullptr);
    glCompileShader(id);

    GLint resultado = GL_FALSE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &resultado); //esto pregunta si se ha compilado correctamente (por eso pasamos el status)
    if (resultado == GL_FALSE) //si ha fallado
    {
        GLint longitud = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &longitud); //preguntas la longitud
        std::string log(static_cast<size_t>(longitud), '\0'); //string de ese tam
        glGetShaderInfoLog(id, longitud, nullptr, &log[0]); //escribes el texto para el log

        glDeleteShader(id);   // no dejamos un shader roto en la GPU
        throw std::runtime_error("Error al compilar el " + mensaje + ":\n" + log); //lanzamos el log
    }
    return id;
}
//esto solo aplica color al fragmento sin ningun cálculo de la geometría
/** 
 * Método para crear, compilar y enlazar el shader program 
 * @note ahora si incluye comprobación de errores 
 */ 
void Renderer::creaShaderProgram (const std::string& nombreBase)
{
    //primero tienes que leer los shaders y almacenar vs (vertex shaders) y fs (fragment shader)
    std::string miVertexShader = leerFichero(nombreBase + "-vs.glsl");
    GLuint vs = compilarShader(GL_VERTEX_SHADER, miVertexShader, "vertex shader");
    GLuint fs = 0;

    //vamos a forzar un error COMENTAR ESTO PARA LA EJECUCION
    std::string miFragmentShader = leerFichero(nombreBase + "-fs.glsl");
    if (FORZAR_ERROR_DEMO)
    {
        miFragmentShader += "\nesto_no_es_glsl_valido";
    }
    fs = compilarShader(GL_FRAGMENT_SHADER, miFragmentShader, "fragment shader");
   
    try
    {
        std::string miFragmentShader = leerFichero(nombreBase + "-fs.glsl");
        fs = compilarShader(GL_FRAGMENT_SHADER, miFragmentShader, "fragment shader");
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
    glGetProgramiv(sp, GL_LINK_STATUS, &resultado); //aqui comprueba si enlazan los shaders entre si
    if (resultado == GL_FALSE) //si no se conectan entre si
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
    //correccion al redimensionarf la pantalla 
    idUniformAspecto = glGetUniformLocation(idSP, "correccionAspecto");
}

//funcion auxiliar para leer el fichero 
/**
 * Lee un fichero de texto completo
 * @param ruta ruta del fichero
 * @return el contenido completo como string
 * @throw std::runtime_error si no se puede abrir
 */
std::string Renderer::leerFichero (const std::string& ruta)
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
/**
 * Calcula la matriz de escalado (column-major) que compensa la relacion
 * de aspecto de la ventana, para que el triangulo no se deforme
 */
void Renderer::calcularMatrizAspecto (GLfloat* matriz) const
{
    float sx = (_aspect > 1.0f) ? 1.0f / _aspect : 1.0f;
    float sy = (_aspect > 1.0f) ? 1.0f : _aspect;

    GLfloat m[16] = {
        sx,   0.0f, 0.0f, 0.0f,
        0.0f, sy,   0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    for (int i = 0; i < 16; i++)
    {
        matriz[i] = m[i];
    }
}
}