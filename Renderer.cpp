
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Renderer.h"
#include <sstream>
#include <cstdarg>
//include necesario para manejar los errores 
#include <stdexcept>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> 
#include <algorithm>

namespace
{
    /**
     * Posicion del centro de un hueco. Cada capa es una cuadricula de 3x3 y las capas
     * se apilan hacia atras en z. El orden de relleno de cada capa es: fila central,
     * fila de arriba, fila de abajo; y en cada fila: centro, derecha, izquierda.
     */
    glm::vec3 posicionDeHueco (int hueco)
    {
        const float ESPACIADO = 1.5f;   // debe ser mayor que el diametro de los modelos (1.0)
        const int COLUMNAS = 3;
        const int FILAS = 3;
        const float columnas[COLUMNAS] = { 0.0f, 1.0f, -1.0f };
        const float filas[FILAS]       = { 0.0f, 1.0f, -1.0f };

        const int porCapa = COLUMNAS * FILAS;
        const int capa  = hueco / porCapa;
        const int celda = hueco % porCapa;

        return glm::vec3(columnas[celda % COLUMNAS] * ESPACIADO,
                         filas[celda / COLUMNAS] * ESPACIADO,
                         -capa * ESPACIADO);
    }
        /// Nombre de la subrutina del fragment shader que implementa cada tipo de luz
    const char* nombreSubrutina (PAG::TipoLuz t)
    {
        switch (t)
        {
            case PAG::TipoLuz::Ambiente:    return "luzAmbiente";
            case PAG::TipoLuz::Puntual:     return "luzPuntual";
            case PAG::TipoLuz::Direccional: return "luzDireccional";
            case PAG::TipoLuz::Foco:        return "luzFoco";
        }
        return "sinLuz";
    }
}


namespace PAG
{

//metodos para definir las luces

    Luz& Renderer::luz (int indice)
    {
        if (indice < 0 || indice >= static_cast<int>(_luces.size()))
        {
            throw std::out_of_range("Indice de luz fuera de rango: " + std::to_string(indice));
        }
        return _luces[indice];
    }

    const Luz& Renderer::luz (int indice) const
    {
        if (indice < 0 || indice >= static_cast<int>(_luces.size()))
        {
            throw std::out_of_range("Indice de luz fuera de rango: " + std::to_string(indice));
        }
        return _luces[indice];
    }

    void Renderer::anadeLuz (const Luz& l)
    {
        _luces.push_back(l);
    }

    void Renderer::eliminaLuz (int indice)
    {
        luz(indice);   // solo para validar el indice (lanza std::out_of_range)
        _luces.erase(_luces.begin() + indice);
    }

    void Renderer::modificaLuz (int indice, const Luz& l)
    {
        luz(indice) = l;   // lanza out_of_range si el indice no vale
    }

    int Renderer::numLuces () const
    {
        return static_cast<int>(_luces.size());
    }

    Luz Renderer::consultaLuz (int indice) const
    {
        return luz(indice);
    }

    /**
     * Escena inicial: una luz ambiente y una puntual a cada lado de los modelos,
     * para poder apreciar el efecto de cada una por separado.
     */
        /**
     * Escena inicial: una puntual a cada lado de los modelos y una luz ambiente.
     * De momento solo se usa la primera luz, asi que va una puntual primero para
     * que se aprecie el sombreado. Con multipasada el orden no importara.
     */
    void Renderer::creaLucesPorDefecto ()
    {
        Luz izquierda(TipoLuz::Puntual);
        izquierda.setPosicion(glm::vec3(-3.0f, 2.0f, 4.0f));

        Luz derecha(TipoLuz::Puntual);
        derecha.setPosicion(glm::vec3(3.0f, 2.0f, 4.0f));

        _luces.push_back(izquierda);
        _luces.push_back(derecha);
        _luces.push_back(Luz(TipoLuz::Ambiente));
    }
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
    _huecoModelo.clear();
    //liberamos las luces tambien
    _luces.clear();
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
        glEnable ( GL_MULTISAMPLE ); 
        glDepthFunc(GL_LEQUAL);   // las pasadas repiten la misma profundidad: deben pasar el test
        glEnable(GL_BLEND);       // permite sumar cada pasada al color acumulado
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
     * Refresco de la escena.
     *  - Alambre: una sola pasada con color fijo.
     *  - Solido: una pasada por cada luz encendida; los resultados se suman con mezcla.
     *  - Solido sin luces: una pasada con la subrutina sinLuz (modelos negros).
     */
    void Renderer::refrescar ()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!_shaderProgram->estaCargado())
        {
            return;
        }

        _shaderProgram->activar();   // ANTES de enviar uniforms y de activar subrutinas

        const bool alambre = (_modo == ModoVisualizacion::Alambre);
        glPolygonMode(GL_FRONT_AND_BACK, alambre ? GL_LINE : GL_FILL);

        const glm::mat4 vista = _camara.matrizVision();
        const glm::mat4 vp    = _camara.matrizVisionProyeccion();

        // ---- Alambre: una unica pasada, sin iluminacion ----
        if (alambre)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            _shaderProgram->activarSubrutina(GL_FRAGMENT_SHADER, "colorAlambre");
            _shaderProgram->setUniform("uColorAlambre", _colorAlambre);
            dibujaModelos(vista, vp);
            return;
        }

        // ---- Solido: una pasada por luz encendida ----
        bool primeraPasada = true;
        for (const Luz& l : _luces)
        {
            if (!l.estaEncendida())
            {
                continue;
            }

            if (primeraPasada)
            {
                // Sustituye lo que hay en el framebuffer (alfa = 1)
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
            else
            {
                // Suma esta luz a lo ya acumulado
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            }

            aplicaLuz(l, vista);
            dibujaModelos(vista, vp);
            primeraPasada = false;
        }

        // ---- Ninguna luz encendida: modelos negros en vez de desaparecer ----
        if (primeraPasada)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            if (!_shaderProgram->activarSubrutina(GL_FRAGMENT_SHADER, "sinLuz"))
            {
                _shaderProgram->activarSubrutina(GL_FRAGMENT_SHADER, "colorSolido");  // pag07
            }
            dibujaModelos(vista, vp);
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
        //asignamos todas las luces que definen el material
        case WindowType::ModelMaterial:
        {
            va_list args;
            va_start(args, t);
            int indice     = va_arg(args, int);
            float* amb     = va_arg(args, float*);
            float* dif     = va_arg(args, float*);
            float* esp     = va_arg(args, float*);
            double exp     = va_arg(args, double);
            Material m(glm::vec3(amb[0], amb[1], amb[2]),
                    glm::vec3(dif[0], dif[1], dif[2]),
                    glm::vec3(esp[0], esp[1], esp[2]),
                    static_cast<float>(exp));
            va_end(args);
            asignaMaterial(indice, m);
            break;
        }
        case WindowType::RenderMode: //render segun sea alambre o solido
        {
            va_list args;
            va_start(args, t);
            int modo = va_arg(args, int);
            va_end(args);
            cambiaModoVisualizacion(static_cast<ModoVisualizacion>(modo));
            break;
        }

        case WindowType::WireColor:
        {
            va_list args;
            va_start(args, t);
            float* c = va_arg(args, float*);
            glm::vec3 color(c[0], c[1], c[2]);   // copiar antes de cerrar la lista
            va_end(args);
            cambiaColorAlambre(color);
            break;
        }
                case WindowType::LightAdd:
        {
            va_list args;
            va_start(args, t);
            int tipo = va_arg(args, int);
            va_end(args);
            anadeLuz(Luz(static_cast<TipoLuz>(tipo)));
            break;
        }
        case WindowType::LightRemove:
        {
            va_list args;
            va_start(args, t);
            int indice = va_arg(args, int);
            va_end(args);
            eliminaLuz(indice);                  // lanza out_of_range si el indice no vale
            break;
        }
        case WindowType::LightModify:
        {
            va_list args;
            va_start(args, t);
            int indice = va_arg(args, int);
            const Luz* recibida = va_arg(args, const Luz*);
            Luz copia = *recibida;               // copiar ANTES de cerrar la lista
            va_end(args);
            modificaLuz(indice, copia);          // lanza out_of_range si el indice no vale
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
    _shaderProgram->cargar(nombreBase);   // las ubicaciones de los uniforms las gestiona ShaderProgram
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


     /// Primer hueco que no ocupa ningun modelo: al borrar uno, su hueco se reutiliza
    int Renderer::primerHuecoLibre () const
    {
        int hueco = 0;
        while (std::find(_huecoModelo.begin(), _huecoModelo.end(), hueco) != _huecoModelo.end())
        {
            ++hueco;
        }
        return hueco;
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

    /// Coloca el modelo en el primer hueco libre de la cuadricula y lo anade a la escena
    void Renderer::colocarYAnadir (std::unique_ptr<Model> m)
    {
        const int hueco = primerHuecoLibre();

        Transformacion t;
        t.posicion = posicionDeHueco(hueco);
        m->setTransformacion(t);

        _modelos.push_back(std::move(m));
        _huecoModelo.push_back(hueco);
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

        colocarYAnadir(std::move(m));
    }
 
    void Renderer::eliminaModelo (int indice)
    {
        modelo(indice);  // solo para validar el indice (lanza std::out_of_range)
        _modelos.erase(_modelos.begin() + indice);
        _huecoModelo.erase(_huecoModelo.begin() + indice);
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

    colocarYAnadir(std::move(m));
}

    void Renderer::asignaMaterial (int indice, const Material& m)
    {
        modelo(indice).setMaterial(m);   // lanza out_of_range si el índice no vale
    }

    Material Renderer::materialModelo (int indice) const
    {
        return modelo(indice).getMaterial();
    }

    void Renderer::cambiaModoVisualizacion (ModoVisualizacion m)
    {
        _modo = m;
    }

    ModoVisualizacion Renderer::modoVisualizacion () const
    {
        return _modo;
    }

        void Renderer::cambiaColorAlambre (const glm::vec3& c)
    {
        _colorAlambre = c;
    }

    const glm::vec3& Renderer::colorAlambre () const
    {
        return _colorAlambre;
    }
        /**
     * Configura el shader para procesar UNA luz: elige la subrutina de su tipo y envia
     * sus datos. Los calculos de iluminacion se hacen en espacio de VISION, asi que
     * posicion y direccion pasan del mundo a la vision aqui, en C++.
     */
    void Renderer::aplicaLuz (const Luz& l, const glm::mat4& vista)
    {
        if (!_shaderProgram->activarSubrutina(GL_FRAGMENT_SHADER, nombreSubrutina(l.getTipo())))
        {
            _shaderProgram->activarSubrutina(GL_FRAGMENT_SHADER, "colorSolido");  // pag07
        }

        _shaderProgram->setUniform("uIa", l.getIa());
        _shaderProgram->setUniform("uId", l.getId());
        _shaderProgram->setUniform("uIs", l.getIs());

        // Direccion protegida contra el vector nulo (normalize(0) daria NaN)
        glm::vec3 direccion = l.getDireccion();
        if (glm::length(direccion) < 1e-6f)
        {
            direccion = glm::vec3(0.0f, 0.0f, -1.0f);
        }

        // Posicion: es un PUNTO (w = 1), le afecta la traslacion de la camara.
        // Direccion: es un VECTOR, solo le afecta la rotacion (parte 3x3).
        const glm::vec3 posicionVision  = glm::vec3(vista * glm::vec4(l.getPosicion(), 1.0f));
        const glm::vec3 direccionVision = glm::normalize(glm::mat3(vista) * direccion);

        _shaderProgram->setUniform("uPosicionLuz", posicionVision);
        _shaderProgram->setUniform("uDireccionLuz", direccionVision);
        _shaderProgram->setUniform("uCosGamma", glm::cos(glm::radians(l.getGamma())));
        _shaderProgram->setUniform("uS", l.getS());
    }

    /**
     * Dibuja todos los modelos con la luz y la subrutina que haya configurado el shader.
     */
    void Renderer::dibujaModelos (const glm::mat4& vista, const glm::mat4& vp)
    {
        for (const std::unique_ptr<Model>& m : _modelos)
        {
            const glm::mat4 modelado = m->getMatrizModelado();
            const glm::mat4 mv  = vista * modelado;
            const glm::mat4 mvp = vp * modelado;
            const glm::mat3 matrizNormal = glm::transpose(glm::inverse(glm::mat3(mv)));

            _shaderProgram->setUniform("matrizMVP", mvp);
            _shaderProgram->setUniform("matrizMV", mv);
            _shaderProgram->setUniform("matrizNormal", matrizNormal);

            const Material& mat = m->getMaterial();
            _shaderProgram->setUniform("uKa", mat.getAmbiente());
            _shaderProgram->setUniform("uKd", mat.getDifuso());
            _shaderProgram->setUniform("uKs", mat.getEspecular());
            _shaderProgram->setUniform("uNs", mat.getExponente());
            _shaderProgram->setUniform("uColorDifuso", mat.getDifuso());   // compatibilidad con pag07

            m->dibujar();
        }
    }
}