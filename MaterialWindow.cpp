#include <exception>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "MaterialWindow.h"
#include "GUI.h"

namespace PAG
{
    void MaterialWindow::addListener (Listener* listener)
    {
        _listeners.push_back(listener);
    }

    void MaterialWindow::DibujarContenido ()
    {
        const int n = Renderer::getInstancia().numModelos();
        if (n == 0)
        {
            ImGui::TextUnformatted("No hay modelos en la escena");
            return;
        }

        if (_seleccionado >= n) _seleccionado = n - 1;
        if (_seleccionado < 0)  _seleccionado = 0;

        const std::string actual = Renderer::getInstancia().nombreModelo(_seleccionado);
        if (ImGui::BeginCombo("Modelo", actual.c_str()))
        {
            for (int i = 0; i < n; ++i)
            {
                const std::string et = Renderer::getInstancia().nombreModelo(i)
                                       + "##mat" + std::to_string(i);
                if (ImGui::Selectable(et.c_str(), _seleccionado == i))
                {
                    _seleccionado = i;
                }
            }
            ImGui::EndCombo();
        }

        Material m = Renderer::getInstancia().materialModelo(_seleccionado);
        glm::vec3 amb = m.getAmbiente();
        glm::vec3 dif = m.getDifuso();
        glm::vec3 esp = m.getEspecular();
        float exponente = m.getExponente();

        bool cambiado = false;
        cambiado |= ImGui::ColorEdit3("Ambiente",  glm::value_ptr(amb));
        cambiado |= ImGui::ColorEdit3("Difuso",    glm::value_ptr(dif));
        cambiado |= ImGui::ColorEdit3("Especular", glm::value_ptr(esp));
        cambiado |= ImGui::SliderFloat("Exponente", &exponente, 1.0f, 256.0f);

        if (cambiado)
        {
            notificarMaterial(_seleccionado, glm::value_ptr(amb),
                              glm::value_ptr(dif), glm::value_ptr(esp), exponente);
        }
    }

    void MaterialWindow::notificarMaterial (int indice, float* amb, float* dif,
                                            float* esp, float exponente)
    {
        try
        {
            for (Listener* l : _listeners)
            {
                l->wakeUp(WindowType::ModelMaterial, indice, amb, dif, esp,
                          static_cast<double>(exponente));
            }
        }
        catch (const std::exception& e)
        {
            GUI::getInstancia().aniadirMensaje(e.what());
        }
    }
}