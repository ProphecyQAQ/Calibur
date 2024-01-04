#include "hzpch.h"
#include "Calibur/Core/LayerStack.h"

namespace Calibur
{
    LayerStack::~LayerStack()
    {
        for (Layer* layer : m_Layers)
        {
            layer->OnDetach();
            delete layer;
        }
    }

    void LayerStack::PushLayer(Layer* layer)
    {
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex ++;
    }

    void LayerStack::PushOverlay(Layer* layer) 
    {
        m_Layers.emplace_back(layer);
    }

    void LayerStack::PopLayer(Layer* layer)
    {
        auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
        if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
    }

    void LayerStack::PopOverlay(Layer* overlayer)
    {
        auto it = std::find(m_Layers.begin(), m_Layers.end(), overlayer);
        if (it != m_Layers.end())
        {
            m_Layers.erase(it);
        }
    }
}