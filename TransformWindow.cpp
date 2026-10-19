#include <exception>
#include <string>

#include <imgui.h>

#include "TransformWindow.h"
#include "Renderer.h"
#include "GUI.h"

namespace PAG
{
    //metodos para registrar a los listeners 
    void TransformWindow::addListener (Listener* listener)
    {
        _listeners.push_back(listener);
    }
//dibuja la ventana para elegir las transofrmaciones
    void TransformWindow::dibujar ()
    {
        ImGui::SetNextWindowPos(ImVec2(700, 250), ImGuiCond_Once);
        if (ImGui::Begin("Transformaciones"))
        {
            ImGui::SetWindowFontScale(1.0f);

            const int n = Renderer::getInstancia().numModelos();
            if (n == 0)
            {
                ImGui::TextUnformatted("No hay modelos en la escena");
            }
            else
            {
                if (_seleccionado >= n)
                {
                    _seleccionado = n - 1;   // se elimino el ultimo modelo
                }

                // Combo para elegir modelo
                const std::string actual = Renderer::getInstancia().nombreModelo(_seleccionado);
                if (ImGui::BeginCombo("Modelo", actual.c_str()))
                {
                    for (int i = 0; i < n; ++i)
                    {
                        const std::string etiqueta = Renderer::getInstancia().nombreModelo(i)
                                                     + "##combo" + std::to_string(i);
                        if (ImGui::Selectable(etiqueta.c_str(), _seleccionado == i))
                        {
                            _seleccionado = i;
                        }
                    }
                    ImGui::EndCombo();
                }

                // Valores ACTUALES del modelo elegido: los controles los editan directamente
                Transformacion t = Renderer::getInstancia().transformacionModelo(_seleccionado);
                bool cambio = false;

                if (ImGui::DragFloat3("Traslacion", &t.posicion.x, 0.01f))
                {
                    cambio = true;
                }
                if (ImGui::DragFloat3("Rotacion (grados)", &t.rotacion.x, 0.5f))
                {
                    cambio = true;
                }
                if (ImGui::DragFloat3("Escala", &t.escala.x, 0.01f, 0.01f, 100.0f))
                {
                    cambio = true;
                }
                if (ImGui::Button("Reiniciar rotacion y escala"))
                {
                    t.rotacion = glm::vec3(0.0f);
                    t.escala   = glm::vec3(1.0f);
                    cambio = true;
                }

                if (cambio)
                {
                    notificar(_seleccionado, t);
                }
            }
        }
        ImGui::End();
    }

    /**
     * Las excepciones del Renderer se capturan AQUI para que no atraviesen la
     * ventana de ImGui. Es un control de interfaz, asi que puede escribir en
     * la ventana de mensajes.
     */
    void TransformWindow::notificar (int indice, const Transformacion& t)
    {
        try
        {
            for (Listener* l : _listeners)
            {
                // float -> double: promocion automatica en argumentos variables
                l->wakeUp(WindowType::ModelTransform, indice,
                          t.posicion.x, t.posicion.y, t.posicion.z,
                          t.rotacion.x, t.rotacion.y, t.rotacion.z,
                          t.escala.x,   t.escala.y,   t.escala.z);
            }
        }
        catch (const std::exception& e)
        {
            GUI::getInstancia().aniadirMensaje(e.what());
        }
    }
}