#include <exception>
#include <filesystem>

#include <imgui.h>

#include "ModelWindow.h"
#include "GUI.h"

namespace PAG
{
    ModelWindow::ModelWindow () //contructor donde definimos la ventana con la que podremos definir la ventana para cargar modelos
    {
        _selector.SetTitle("Selecciona un modelo OBJ");
        _selector.SetTypeFilters({ ".obj" });   // solo muestra ficheros .obj

        // hay que tener cuidado de que no exista la carpeta models 
        if (std::filesystem::exists("models")) 
        {
            _selector.SetPwd("models");
        }
    }
//método báasicos para el patron con los listeners
    void ModelWindow::addListener (Listener* listener)
    {
        _listeners.push_back(listener);
    }
//método para dibujar cargando los modelos .obj, cambiado para poder dibujar todo en una columna 
        void ModelWindow::dibujarContenido ()
    {
        if (ImGui::Button("Cargar OBJ..."))
        {
            _selector.Open();
        }

        const int n = Renderer::getInstancia().numModelos();
        ImGui::Text("Modelos en escena: %d", n);

        for (int i = 0; i < n; ++i)
        {
            const std::string etiqueta = Renderer::getInstancia().nombreModelo(i)
                                         + "##lista" + std::to_string(i);
            if (ImGui::Selectable(etiqueta.c_str(), _seleccionado == i))
            {
                _seleccionado = i;
            }
        }

        if (ImGui::Button("Eliminar seleccionado") && _seleccionado >= 0 && _seleccionado < n)
        {
            notificarBorrado(_seleccionado);
            _seleccionado = -1;
        }
    }

    void ModelWindow::dibujarSelector ()
    {
        _selector.Display();
        if (_selector.HasSelected())
        {
            const std::string ruta = _selector.GetSelected().string();
            _selector.ClearSelected();
            notificarCarga(ruta);
        }
    }

    /**
     * Las excepciones del Renderer se capturan AQUI para que no atraviesen la
     * ventana de ImGui. Es un control de interfaz, asi que puede escribir en
     * la ventana de mensajes.
     */
    void ModelWindow::notificarCarga (const std::string& ruta)
    {
        try
        {
            for (Listener* l : _listeners)
            {
                l->wakeUp(WindowType::ModelLoad, ruta.c_str());
            }
            GUI::getInstancia().aniadirMensaje("Modelo cargado: " + ruta);
        }
        catch (const std::exception& e)
        {
            GUI::getInstancia().aniadirMensaje(e.what());
        }
    }

    void ModelWindow::notificarBorrado (int indice)
    {
        try
        {
            for (Listener* l : _listeners)
            {
                l->wakeUp(WindowType::ModelDelete, indice);
            }
        }
        catch (const std::exception& e)
        {
            GUI::getInstancia().aniadirMensaje(e.what());
        }
    }
}