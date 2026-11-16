#ifndef TIPOLUZ_H
#define TIPOLUZ_H

namespace PAG
{
    /// Tipos de luz soportados. Los valores son explicitos porque viajaran como int por wakeUp
    enum class TipoLuz
    {
        Ambiente    = 0,
        Puntual     = 1,
        Direccional = 2,
        Foco        = 3
    };

    /// Nombre legible del tipo, para mensajes y para la interfaz
    inline const char* nombreTipoLuz (TipoLuz t)
    {
        switch (t)
        {
            case TipoLuz::Ambiente:    return "Ambiente";
            case TipoLuz::Puntual:     return "Puntual";
            case TipoLuz::Direccional: return "Direccional";
            case TipoLuz::Foco:        return "Foco";
        }
        return "Desconocida";
    }
}

#endif //TIPOLUZ_H