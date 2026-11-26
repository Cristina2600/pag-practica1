#include <exception>
#include <string>

#include <imgui.h>

#include "LightWindow.h"
#include "Renderer.h"
#include "GUI.h"

namespace PAG
{
    namespace
    {
        // El orden coincide con los valores del enum TipoLuz (0..3): se usa como indice
        const char* NOMBRES_TIPO[] = { "Ambiente", "Puntual", "Direccional", "Foco" };
    }

    void LightWindow::addListener (Listener* listener)
    {
        _listeners.push_back(listener);
    }

    void LightWindow::dibujarContenido ()
    {
        Renderer& renderer = Renderer::getInstancia();

        // ---------------- Alta de luces ----------------
        ImGui::Combo("Tipo nueva", &_tipoNueva, NOMBRES_TIPO, IM_ARRAYSIZE(NOMBRES_TIPO));
        if (ImGui::Button("Anadir luz"))
        {
            notificarAlta(_tipoNueva);
            _seleccion = renderer.numLuces() - 1;   // seleccionamos la recien creada
        }

        const int n = renderer.numLuces();
        if (n == 0)
        {
            ImGui::TextUnformatted("No hay luces en la escena");
            return;
        }

        // Si se elimino la ultima, el indice seleccionado puede haberse quedado fuera
        if (_seleccion >= n) _seleccion = n - 1;
        if (_seleccion < 0)  _seleccion = 0;

        // ---------------- Lista de luces ----------------
        ImGui::Separator();
        for (int i = 0; i < n; ++i)
        {
            const Luz li = renderer.consultaLuz(i);
            std::string etiqueta = std::to_string(i) + ": "
                                 + NOMBRES_TIPO[static_cast<int>(li.getTipo())];
            if (!li.estaEncendida())
            {
                etiqueta += " (apagada)";
            }
            etiqueta += "##luz" + std::to_string(i);   // "##": id unico sin mostrarse

            if (ImGui::Selectable(etiqueta.c_str(), _seleccion == i))
            {
                _seleccion = i;
            }
        }

        if (ImGui::Button("Eliminar seleccionada"))
        {
            notificarBaja(_seleccion);
            return;   // la lista ha cambiado: no seguimos dibujando con indices viejos
        }

        // ---------------- Edicion de la luz seleccionada ----------------
        // Copia local: los controles la editan y, si algo cambia, se envia entera
        Luz luz = renderer.consultaLuz(_seleccion);
        bool cambio = false;

        ImGui::Separator();

        bool encendida = luz.estaEncendida();
        if (ImGui::Checkbox("Encendida", &encendida))
        {
            luz.setEncendida(encendida);
            cambio = true;
        }

        int tipo = static_cast<int>(luz.getTipo());
        if (ImGui::Combo("Tipo", &tipo, NOMBRES_TIPO, IM_ARRAYSIZE(NOMBRES_TIPO)))
        {
            luz.setTipo(static_cast<TipoLuz>(tipo));   // los demas datos se conservan
            cambio = true;
        }

        // Solo se muestran los parametros que tienen sentido para el tipo actual
        if (luz.usaIa())
        {
            glm::vec3 ia = luz.getIa();
            if (ImGui::ColorEdit3("Ia (ambiente)", &ia.x))
            {
                luz.setIa(ia);
                cambio = true;
            }
        }
        if (luz.usaIdIs())
        {
            glm::vec3 id = luz.getId();
            if (ImGui::ColorEdit3("Id (difusa)", &id.x))
            {
                luz.setId(id);
                cambio = true;
            }
            glm::vec3 is = luz.getIs();
            if (ImGui::ColorEdit3("Is (especular)", &is.x))
            {
                luz.setIs(is);
                cambio = true;
            }
        }
        if (luz.usaPosicion())
        {
            glm::vec3 pos = luz.getPosicion();
            if (ImGui::DragFloat3("Posicion", &pos.x, 0.05f))
            {
                luz.setPosicion(pos);
                cambio = true;
            }
        }
        if (luz.usaDireccion())
        {
            glm::vec3 dir = luz.getDireccion();
            if (ImGui::DragFloat3("Direccion", &dir.x, 0.01f))
            {
                luz.setDireccion(dir);   // el Renderer protege el vector nulo
                cambio = true;
            }
        }
        if (luz.usaCono())
        {
            float gamma = luz.getGamma();
            if (ImGui::SliderFloat("Gamma (grados)", &gamma, 1.0f, 89.0f, "%.0f"))
            {
                luz.setGamma(gamma);
                cambio = true;
            }
            float s = luz.getS();
            if (ImGui::SliderFloat("s (bordes suaves)", &s, 0.0f, 128.0f, "%.1f"))
            {
                luz.setS(s);
                cambio = true;
            }
        }

        if (cambio)
        {
            notificarCambio(_seleccion, luz);
        }
    }

    /**
     * Las excepciones del Renderer se capturan AQUI para que no atraviesen
     * la ventana de ImGui (igual que en ModelWindow y TransformWindow).
     */
    void LightWindow::notificarAlta (int tipo)
    {
        try
        {
            for (Listener* l : _listeners)
            {
                l->wakeUp(WindowType::LightAdd, tipo);
            }
        }
        catch (const std::exception& e)
        {
            GUI::getInstancia().aniadirMensaje(e.what());
        }
    }

    void LightWindow::notificarBaja (int indice)
    {
        try
        {
            for (Listener* l : _listeners)
            {
                l->wakeUp(WindowType::LightRemove, indice);
            }
        }
        catch (const std::exception& e)
        {
            GUI::getInstancia().aniadirMensaje(e.what());
        }
    }

    void LightWindow::notificarCambio (int indice, const Luz& luz)
    {
        try
        {
            const Luz* puntero = &luz;   // tipo exacto que leera el wakeUp
            for (Listener* l : _listeners)
            {
                l->wakeUp(WindowType::LightModify, indice, puntero);
            }
        }
        catch (const std::exception& e)
        {
            GUI::getInstancia().aniadirMensaje(e.what());
        }
    }
}