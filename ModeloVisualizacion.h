#ifndef MODOVISUALIZACION_H
#define MODOVISUALIZACION_H

namespace PAG
{
    //Vamos a cambiar entre el alamabre y el solido
    enum class ModoVisualizacion
    {
        Alambre = 0,   // Lo que ya teniamos la malla sin rellenar
        Solido  = 1    // relleno, color difuso del material
    };
}

#endif //MODOVISUALIZACION_H