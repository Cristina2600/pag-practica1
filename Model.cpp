#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"

namespace
{
    /**
     * Anade al final de los arrays los vertices e indices de UNA malla de Assimp.
     * Como todas las mallas acaban en un unico VAO, los indices de cada malla hay
     * que desplazarlos tantos vertices como ya hubiera
     */
    void procesarMalla (const aiMesh* malla,
                        std::vector<GLfloat>& posiciones,
                        std::vector<GLfloat>& colores,
                        std::vector<GLuint>& indices)
    {
        //esto es para ver cuantos vertices hay y desplazarnos todos esos para iniciar la malla 
        const GLuint desplazamiento = static_cast<GLuint>(posiciones.size() / 3);//cuantos triangulos hay

        for (unsigned int v = 0; v < malla->mNumVertices; ++v)
        {
            const aiVector3D& p = malla->mVertices[v];
            posiciones.push_back(p.x);
            posiciones.push_back(p.y);
            posiciones.push_back(p.z);

            // Aun no hay luces ni materiales: el "color" del vertice se obtiene de
            // su normal (de [-1,1] a [0,1]) para que el modelo no se vea plano.
            if (malla->HasNormals())
            {
                const aiVector3D& n = malla->mNormals[v];
                colores.push_back(n.x * 0.5f + 0.5f);
                colores.push_back(n.y * 0.5f + 0.5f);
                colores.push_back(n.z * 0.5f + 0.5f);
            }
            else
            {
                colores.push_back(1.0f);
                colores.push_back(1.0f);
                colores.push_back(1.0f);
            }
        }

        for (unsigned int f = 0; f < malla->mNumFaces; ++f) //trabajamos las caras que define la malla
        {
            const aiFace& cara = malla->mFaces[f];
            if (cara.mNumIndices != 3)  // tras aiProcess_Triangulate solo deberian quedar triangulos
            {
                continue;
            }
            for (unsigned int j = 0; j < 3; ++j)
            {
                indices.push_back(cara.mIndices[j] + desplazamiento);
            }
        }
    }

    /// Recorre el arbol de nodos de Assimp procesando las mallas de cada nodo
    void procesarNodo (const aiNode* nodo, const aiScene* escena,
                       std::vector<GLfloat>& posiciones,
                       std::vector<GLfloat>& colores,
                       std::vector<GLuint>& indices)
    {
        for (unsigned int i = 0; i < nodo->mNumMeshes; ++i)
        {
            procesarMalla(escena->mMeshes[nodo->mMeshes[i]], posiciones, colores, indices);
        }
        for (unsigned int i = 0; i < nodo->mNumChildren; ++i)
        {
            procesarNodo(nodo->mChildren[i], escena, posiciones, colores, indices);
        }
    }
}

namespace PAG
{
    Model::~Model ()
    {
        liberar();
    }

    /**
     * Crea el VAO con sus VBOs e IBO a partir de arrays en CPU.
     * El VAO se activa PRIMERO: todo lo que se configura despues queda grabado en el.
     */
    void Model::crearBuffers (const std::vector<GLfloat>& posiciones,
                              const std::vector<GLfloat>& colores,
                              const std::vector<GLuint>& indices)
    {
        liberar();  // por si el modelo ya tenia geometria

        _numIndices = static_cast<GLsizei>(indices.size());

        glGenVertexArrays(1, &idVAO);
        glBindVertexArray(idVAO);

        // Atributo 0: posiciones
        glGenBuffers(1, &idVBOPos);
        glBindBuffer(GL_ARRAY_BUFFER, idVBOPos);
        glBufferData(GL_ARRAY_BUFFER, posiciones.size() * sizeof(GLfloat), posiciones.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);

        // Atributo 1: colores
        glGenBuffers(1, &idVBOColor);
        glBindBuffer(GL_ARRAY_BUFFER, idVBOColor);
        glBufferData(GL_ARRAY_BUFFER, colores.size() * sizeof(GLfloat), colores.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(1);

        // Indices (el VAO recuerda este IBO)
        glGenBuffers(1, &idIBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);  // dejamos de modificar este VAO
    }

    void Model::crearTriangulo ()
    {
        std::vector<GLfloat> posiciones = { -.5f, -.5f, 0.0f,
                                             .5f, -.5f, 0.0f,
                                             .0f,  .5f, 0.0f };
        std::vector<GLfloat> colores    = { 1.0f, 0.0f, 0.0f,
                                            0.0f, 1.0f, 0.0f,
                                            0.0f, 0.0f, 1.0f };
        std::vector<GLuint> indices     = { 0, 1, 2 };

        crearBuffers(posiciones, colores, indices);
        _alambre = false;
    }

    void Model::cargarOBJ (const std::string& ruta)
    {
        Assimp::Importer importador;
        const aiScene* escena = importador.ReadFile(ruta,
            aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals);

        if (escena == nullptr || (escena->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || escena->mRootNode == nullptr)
        {
            throw std::runtime_error("No se pudo cargar el modelo '" + ruta + "': " + importador.GetErrorString());
        }

        std::vector<GLfloat> posiciones;
        std::vector<GLfloat> colores;
        std::vector<GLuint> indices;
        procesarNodo(escena->mRootNode, escena, posiciones, colores, indices);

        if (indices.empty())
        {
            throw std::runtime_error("El modelo '" + ruta + "' no contiene triangulos");
        }

        // Solo si todo ha ido bien se tocan los recursos OpenGL del modelo
        crearBuffers(posiciones, colores, indices);
        _alambre = true;  // sin luces, la malla de alambre se aprecia mejor
    }

    /**
     * Dibuja el modelo. El Renderer ya ha activado el shader program y ha
     * enviado el uniform con la matriz de este modelo.
     */
    void Model::dibujar () const
    {
        if (idVAO == 0)
        {
            return;
        }
        glPolygonMode(GL_FRONT_AND_BACK, _alambre ? GL_LINE : GL_FILL);
        glBindVertexArray(idVAO);
        glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, nullptr);
    }

    /// Borra primero el VAO y despues los buffers (el VAO no borra los buffers por si solo)
    void Model::liberar ()
    {
        if (idVAO != 0)      { glDeleteVertexArrays(1, &idVAO);   idVAO = 0; }
        if (idVBOPos != 0)   { glDeleteBuffers(1, &idVBOPos);     idVBOPos = 0; }
        if (idVBOColor != 0) { glDeleteBuffers(1, &idVBOColor);   idVBOColor = 0; }
        if (idIBO != 0)      { glDeleteBuffers(1, &idIBO);        idIBO = 0; }
        _numIndices = 0;
    }

    /**
     * M = T * Rx * Ry * Rz * S
     * Al aplicarse sobre un vertice, el orden real es: primero escala, despues
     * rotacion en Z, Y, X y por ultimo traslacion.
     */
    void Model::actualizarMatrizModelado ()
    {
        glm::mat4 m(1.0f);
        m = glm::translate(m, _transformacion.posicion);
        m = glm::rotate(m, glm::radians(_transformacion.rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
        m = glm::rotate(m, glm::radians(_transformacion.rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
        m = glm::rotate(m, glm::radians(_transformacion.rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));
        m = glm::scale(m, _transformacion.escala);
        _modelado = m;
    }

    const glm::mat4& Model::getMatrizModelado () const
    {
        return _modelado;
    }

    const Transformacion& Model::getTransformacion () const
    {
        return _transformacion;
    }

    void Model::setTransformacion (const Transformacion& t)
    {
        _transformacion = t;
        actualizarMatrizModelado();
    }

    const std::string& Model::getNombre () const
    {
        return _nombre;
    }

    void Model::setNombre (const std::string& nombre)
    {
        _nombre = nombre;
    }
}