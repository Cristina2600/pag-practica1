
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Renderer.h"
#include <sstream>
#include <cstdarg>
//include necesario para manejar los errores 
#include <stdexcept>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> 

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
        _shaderProgram = std::make_unique<ShaderProgram>();
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
    _modelos.clear(); //limpias el buffer que estamos cargandp
    // El ShaderProgram se libera solo, via su propio destructor (unique_ptr)
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
     * Refresco de la escena. Se realizan varias cosas a la vez activa el shader, calcula
     * P*V una sola vez y, para cada modelo, envia P*V*M y le pide que se dibuje.
     */
    void Renderer::refrescar ()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //limpiamos el buffer de color
 
        if (!_shaderProgram->estaCargado())
        {
            return;
        }
 
        _shaderProgram->activar();   // el programa debe estar activo ANTES de enviar uniforms
 
        const glm::mat4 vp = _camara.matrizVisionProyeccion();
 
        for (const std::unique_ptr<Model>& m : _modelos)
        {
            const glm::mat4 mvp = vp * m->getMatrizModelado();
            glUniformMatrix4fv(idUniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));
            m->dibujar();
        }
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
             _camara.setViewport(width, height); //ahora la cámara recalcula el aspecto 

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
        //ahora añadimos la opcion de cambiar shaders
        case WindowType::ShaderProgram:
        {
            va_list args;
            va_start(args, t);
            std::string nombre = va_arg(args, const char*);  // copiamos antes de cerrar
            va_end(args);

            creaShaderProgram("shaders/" + nombre);  // puede lanzar excepción
            break;
        }
        case WindowType::CameraSelect:
        {
            va_list args;
            va_start(args, t);
            int tipo = va_arg(args, int);
            va_end(args);
            _movimiento = static_cast<CameraMovement>(tipo);
            break;
        }
        case WindowType::CameraMove:
        {
                va_list args;
                va_start(args, t);
                double h = va_arg(args, double);
                double v = va_arg(args, double);
                va_end(args);
                moverCamara(h, v);
                break;
        }
        case WindowType::CameraZoom:
        {
                va_list args;
                va_start(args, t);
                double grados = va_arg(args, double);
                va_end(args);
                _camara.setZoom(static_cast<float>(grados));
                break;
        }
        //casos para tener varios tipos de modelos
        case WindowType::ModelLoad: //aqui cargamos los modelos recorriendo la ruta
        {
            va_list args;
            va_start(args, t);
            std::string ruta = va_arg(args, const char*);   // copiamos antes de cerrar la lista
            va_end(args); //hay que cerrar la lista antes de cargar los modelos 
            cargaModelo(ruta);                               // puede lanzar excepcion
            break;
        }
        case WindowType::ModelDelete: //eliminamos los modelos recorriendo sus indices 
        {
            va_list args;
            va_start(args, t);
            int indice = va_arg(args, int);
            va_end(args);
            eliminaModelo(indice);                           // lanza out_of_range si el indice no vale
            break;
        }   
        case WindowType::ModelTransform:
        {
            va_list args;
            va_start(args, t); //primero lees toca la lista 
            int indice = va_arg(args, int);
            Transformacion tr;
            tr.posicion.x = static_cast<float>(va_arg(args, double));
            tr.posicion.y = static_cast<float>(va_arg(args, double));
            tr.posicion.z = static_cast<float>(va_arg(args, double));
            tr.rotacion.x = static_cast<float>(va_arg(args, double));
            tr.rotacion.y = static_cast<float>(va_arg(args, double));
            tr.rotacion.z = static_cast<float>(va_arg(args, double));
            tr.escala.x   = static_cast<float>(va_arg(args, double));
            tr.escala.y   = static_cast<float>(va_arg(args, double));
            tr.escala.z   = static_cast<float>(va_arg(args, double));
            va_end(args); //cierras la lista antes de ejecutar nada 
            transformaModelo(indice, tr);   // lanza out_of_range si el indice no vale
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
cargaModelo("models/pokemon_sylveon.obj");
}
//esto solo aplica color al fragmento sin ningun cálculo de la geometría
/** 
 * Método para crear, compilar y enlazar el shader program 
 * @note ahora si incluye comprobación de errores 
 */ 
void Renderer::creaShaderProgram (const std::string& nombreBase)
{
    _shaderProgram->cargar(nombreBase);
    idUniformMVP = _shaderProgram->getUniformLocation("matrizMVP");    //busca los shaders con este tipo de matriz  
}



void Renderer::moverCamara (double horizontal, double vertical)
{
    const float gradosPorPixel   = 0.3f;
    const float unidadesPorPixel = 0.01f;
    const float x = static_cast<float>(horizontal);
    const float y = static_cast<float>(vertical);

    switch (_movimiento)
    {
        case CameraMovement::Pan:   _camara.pan(x * gradosPorPixel);                      break;
        case CameraMovement::Tilt:  _camara.tilt(y * gradosPorPixel);                     break;
        case CameraMovement::Dolly: _camara.dolly(x * unidadesPorPixel, y * unidadesPorPixel); break;
        case CameraMovement::Crane: _camara.crane(y * unidadesPorPixel);                  break;
        case CameraMovement::Orbit: _camara.orbit(y * gradosPorPixel, x * gradosPorPixel);break;
        case CameraMovement::Zoom:  _camara.zoom(-y * 0.2f);                              break;
    }
}

    // ------------------------------------------------------------------
    // Modelos
    // ------------------------------------------------------------------
 
    /**
     * Reparte los modelos en una rejilla para que no se solapen. Se usa un
     * contador creciente para que, al eliminar un
     * modelo, el siguiente no caiga encima de otro que sigue en la escena.
     * Cambia ESPACIADO si tus modelos son mas grandes o mas pequenos.
     */
    glm::vec3 Renderer::siguientePosicion ()
    {
        const float ESPACIADO = 1.2f; //separación entre los modelos
        const float columnas[3] = { 0.0f, 1.0f, -1.0f };   // centro, derecha, izquierda
 
        const int n = _huecosUsados++;
        return glm::vec3(columnas[n % 3] * ESPACIADO, -(n / 3) * ESPACIADO, 0.0f);
    }
 
    Model& Renderer::modelo (int indice)
    {
        if (indice < 0 || indice >= static_cast<int>(_modelos.size()))
        {
            throw std::out_of_range("Indice de modelo fuera de rango: " + std::to_string(indice));
        }
        return *_modelos[indice];
    }
 
    const Model& Renderer::modelo (int indice) const
    {
        if (indice < 0 || indice >= static_cast<int>(_modelos.size()))
        {
            throw std::out_of_range("Indice de modelo fuera de rango: " + std::to_string(indice));
        }
        return *_modelos[indice];
    }
 

 
    /**
     * Carga un OBJ y lo anade a la escena. Si Model::cargarOBJ lanza una excepcion,
     * el modelo local se destruye y NO se anade nada (el vector queda intacto).
     */
    void Renderer::cargaModelo (const std::string& ruta)
    {
        std::unique_ptr<Model> m = std::make_unique<Model>();
        m->cargarOBJ(ruta);
 
        // Nombre = nombre del fichero sin la carpeta
        const size_t separador = ruta.find_last_of("/\\");
        m->setNombre(separador == std::string::npos ? ruta : ruta.substr(separador + 1));
 
        Transformacion t;
        t.posicion = siguientePosicion();
        m->setTransformacion(t);
 
        _modelos.push_back(std::move(m));
    }
 
    void Renderer::eliminaModelo (int indice)
    {
        modelo(indice);  // solo para validar el indice (lanza std::out_of_range)
        _modelos.erase(_modelos.begin() + indice);
    }
 
    void Renderer::transformaModelo (int indice, const Transformacion& t)
    {
        modelo(indice).setTransformacion(t);
    }
 
    int Renderer::numModelos () const
    {
        return static_cast<int>(_modelos.size());
    }
 
    std::string Renderer::nombreModelo (int indice) const
    {
        return modelo(indice).getNombre();
    }
 
    Transformacion Renderer::transformacionModelo (int indice) const
    {
        return modelo(indice).getTransformacion();
    }
 //método para seguir manteniendo el triangulo
void Renderer::creaTriangulo ()
{
    std::unique_ptr<Model> m = std::make_unique<Model>();
    m->crearTriangulo();
    m->setNombre("Triangulo");

    Transformacion t;
    t.posicion = siguientePosicion();
    m->setTransformacion(t);

    _modelos.push_back(std::move(m));
}
}