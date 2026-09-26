//esto es un enum con el que vamos a saber de donde viene cada notificacion

#ifndef WINDOWTYPE_H
#define WINDOWTYPE_H

namespace PAG
{
    /**
     * @brief Identifica que ventana/control de la interfaz ha generado
     *        una notificacion. Se ira ampliando segun crezca la interfaz.
     */
    enum class WindowType
    {
        Background  ///< Notificacion del selector de color de fondo
    };
}

#endif //WINDOWTYPE_H